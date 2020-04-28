#pragma once
#include <cstdint>
#include <array>
#include <variant>
#include <unordered_map>

namespace riscv
{
	namespace instruction
	{
		enum class type_identifier : uint8_t
		{
			R, R4, I, S, B, U, J
		};

		union type_r {
			uint32_t instruction;
			struct
			{
				uint32_t opcode : 7;
				uint32_t rd : 5;
				uint32_t funct3 : 3;
				uint32_t rs1 : 5;
				uint32_t rs2 : 5;
				uint32_t funct7 : 7;
			};
		};

		union type_r4 {
			uint32_t instruction;
			struct 
			{
				uint32_t opcode : 7;
				uint32_t rd : 5;
				uint32_t funct3 : 3;
				uint32_t rs1 : 5;
				uint32_t rs2 : 5;
				uint32_t funct2 : 2;
				uint32_t rs3 : 5;
			};
		};

		union type_i {
			uint32_t instruction;
			struct 
			{
				uint32_t opcode : 7;
				uint32_t rd : 5;
				uint32_t funct3 : 3;
				uint32_t rs1 : 5;
				signed int imm : 12;
			};
		};

		union type_s {
			uint32_t instruction;
			struct 
			{
				uint32_t opcode : 7;
				signed int imm_a : 5;
				uint32_t funct3 : 3;
				uint32_t rs1 : 5;
				uint32_t rs2 : 5;
				signed int imm_b : 7;
			};
		};

		union type_b {
			uint32_t instruction;
			struct 
			{
				uint32_t opcode : 7;
				signed int imm_a : 1;
				signed int imm_b : 4;
				uint32_t funct3 : 3;
				uint32_t rs1 : 5;
				uint32_t rs2 : 5;
				signed int imm_c : 6;
				signed int imm_d : 1;
			};
		};

		union type_u {
			uint32_t instruction;
			struct 
			{
				uint32_t opcode : 7;
				uint32_t rd : 5;
				signed int imm : 20;
			};
		};

		union type_j {
			uint32_t instruction;
			struct
			{
				uint32_t opcode : 7;
				uint32_t rd : 5;
				signed int imm_a : 8;
				signed int imm_b : 1;
				signed int imm_c : 10;
				signed int imm_d : 1;
			};
		};

		class object
		{
			using instruction_format = std::variant<type_r, type_r4, type_i, type_s, type_b, type_u, type_j>;

			const type_identifier m_type;
			const instruction_format m_data;

			const uint8_t get_opcode(const uint32_t instruction) const;
			const type_identifier set_instruction_format(const uint32_t instruction) const;
			const instruction_format set_instruction_data(const type_identifier type, const uint32_t instruction);

		public:
			object() = delete;
			object(const object& obj) = default;
			object(object&& obj) = default;

			explicit object(const uint32_t inst) : m_type { set_instruction_format(inst) }, m_data{ set_instruction_data(m_type, inst) }
			{}

			const type_identifier get_type() const;
			const instruction_format get_data() const;
		};

		//Probably better (and faster) to generate an array filled with null spaces for potential instructions
		//Can be done later tho
		const std::unordered_map<uint8_t, const type_identifier> opcode_instruction_type {
			{0x37, type_identifier::U},
			{0x17, type_identifier::U},
			{0x6F, type_identifier::J},
			{0x67, type_identifier::I},
			{0x63, type_identifier::B},
			{0x23, type_identifier::S},
			{0x13, type_identifier::I}, //its a mutated I format! architecture dependent ;-; check x64 vs x86
			{0x33, type_identifier::R},
			{0x0F, type_identifier::I}, //Fence fucking instruction damnit, its either I, or its custom bullshit, FENCE.I is I, FENCE is garbo custom (its I but the fields are garbo)
			{0x73, type_identifier::I}, //CSR or ECALL/EBREAK
			{0x03, type_identifier::I},
			{0x1B, type_identifier::I}, //modified I, uses shamt bs on 64bit
			{0x3B, type_identifier::R}
		};

		/*
		Have another array/unordered_map based on opcode index/key where you have a list of matches and masks, basically you can detect which type of instruction you want, ...
		then detect the specific instruction and do work based on that, which is quite sweet if you ask me. I attached a python script which helped me generate this bullshit.
		*/
		
		//essentially maps each opcode to a set of instructions with said opcode, which each have a { match, mask, mnemonic } tuple that we can use for parsing and checking which instruction it actually is
		//change to constexpr when msvc decides to fucking implement it -_-
		const std::unordered_map<uint8_t, const std::vector<std::tuple<uint32_t, uint32_t, std::string>> > instruction_table {
			//RV32I Instructions
			{ 0x63, {
						{ 0x63, 0x707f, "BEQ" }, 
						{ 0x1063, 0x707f, "BNE" }, 
						{ 0x4063, 0x707f, "BLT" }, 
						{ 0x5063, 0x707f, "BGE" }, 
						{ 0x6063, 0x707f, "BLTU" }, 
						{ 0x7063, 0x707f, "BGEU" } 
					} 
			},

			{ 0x67, { 
						{ 0x67, 0x707f, "JALR" } 
					} 
			},

			{ 0x6f, { 
						{ 0x6f, 0x7f, "JAL" } 
					} 
			},

			{ 0x37, { 
						{ 0x37, 0x7f, "LUI" } 
					} 
			},

			{ 0x17, { 
						{ 0x17, 0x7f, "AUIPC" } 
					}
			},

			{ 0x13, { 
						{ 0x13, 0x707f, "ADDI" }, 
						{ 0x1013, 0xfc00707f, "SLLI" }, 
						{ 0x2013, 0x707f, "SLTI" }, 
						{ 0x3013, 0x707f, "SLTIU" }, 
						{ 0x4013, 0x707f, "XORI" }, 
						{ 0x5013, 0xfc00707f, "SRLI" }, 
						{ 0x40005013, 0xfc00707f, "SRAI" }, 
						{ 0x6013, 0x707f, "ORI" }, 
						{ 0x7013, 0x707f, "ANDI" } 
					} 
			},
			
			{ 0x33, { 
						{ 0x33, 0xfe00707f, "ADD" }, 
						{ 0x40000033, 0xfe00707f, "SUB" }, 
						{ 0x1033, 0xfe00707f, "SLL" }, 
						{ 0x2033, 0xfe00707f, "SLT" }, 
						{ 0x3033, 0xfe00707f, "SLTU" }, 
						{ 0x4033, 0xfe00707f, "XOR" }, 
						{ 0x5033, 0xfe00707f, "SRL" }, 
						{ 0x40005033, 0xfe00707f, "SRA" }, 
						{ 0x6033, 0xfe00707f, "OR" }, 
						{ 0x7033, 0xfe00707f, "AND" } 
					} 
			},

			{ 0x03, { 
						{ 0x3, 0x707f, "LB" }, 
						{ 0x1003, 0x707f, "LH" }, 
						{ 0x2003, 0x707f, "LW" }, 
						{ 0x4003, 0x707f, "LBU" }, 
						{ 0x5003, 0x707f, "LHU" },

						//RV64I Instructions
						{ 0x3003, 0x707f, "LD" }, 
						{ 0x6003, 0x707f, "LWU" }
					} 
			},

			{ 0x23, { 
						{ 0x23, 0x707f, "SB" }, 
						{ 0x1023, 0x707f, "SH" }, 
						{ 0x2023, 0x707f, "SW" }, 

						//RV64I Instruction
						{ 0x3023, 0x707f, "SD" }
					} 
			},

			{ 0x0f, { 
						{ 0xf, 0x707f, "FENCE" }, 

						//Zfencei instruction
						{ 0x100f, 0x707f, "FENCE.I" } 
					} 
			},

			//Zicsr Instructions
			{ 0x73, {
						{ 0x73, 0xffffffff, "ECALL" },
						{ 0x100073, 0xffffffff, "EBREAK" },
						{ 0x1073, 0x707f, "CSRRW" },
						{ 0x2073, 0x707f, "CSRRS" },
						{ 0x3073, 0x707f, "CSRRC" },
						{ 0x5073, 0x707f, "CSRRWI" },
						{ 0x6073, 0x707f, "CSRRSI" },
						{ 0x7073, 0x707f, "CSRRCI" } 
					} 
			},

			//RV64I Instructions
			{ 0x1b, { 
						{ 0x1b, 0x707f, "ADDIW" }, 
						{ 0x101b, 0xfe00707f, "SLLIW" }, 
						{ 0x501b, 0xfe00707f, "SRLIW" }, 
						{ 0x4000501b, 0xfe00707f, "SRAIW" } 
					} 
			},

			{ 0x3b, { 
						{ 0x3b, 0xfe00707f, "ADDW" }, 
						{ 0x4000003b, 0xfe00707f, "SUBW" }, 
						{ 0x103b, 0xfe00707f, "SLLW" }, 
						{ 0x503b, 0xfe00707f, "SRLW" }, 
						{ 0x4000503b, 0xfe00707f, "SRAW" } 
					} 
			}
		};
	}
}