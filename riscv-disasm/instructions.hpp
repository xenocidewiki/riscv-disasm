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

		enum class extensions
		{
			I,
			ZIFENCEI,
			ZICSR,
			M,
			A,
			F,
			D,
			Q
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
			{0x0F, type_identifier::I}, //Fence fucking instruction damnit, its either I, or its custom bullshit, FENCE.I is I, FENCE is garbo custom (its I but the fields are garbo) APPARENTLY THERE IS FENCE.TSO TOO?
			{0x73, type_identifier::I}, //CSR or ECALL/EBREAK
			{0x03, type_identifier::I},
			{0x1B, type_identifier::I}, //modified I, uses shamt bs on 64bit
			{0x3B, type_identifier::R},
			{0x2F, type_identifier::R}, //RV32/64A extension

			//RV(32/64)(F/D/Q) Extensions
			{0x07, type_identifier::I}, //FLW/FLD/FLQ
			{0x27, type_identifier::S}, //FSW/FSD/FSQ
			{0x43, type_identifier::R4}, //FMADD.S/D/Q
			{0x47, type_identifier::R4}, //FMSUB.S/D/Q
			{0x4B, type_identifier::R4}, //FNMSUB.S/D/Q
			{0x4F, type_identifier::R4}, //FNMADD.S/D/Q
			{0x53, type_identifier::R} //Everything else
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
						{ 0x7033, 0xfe00707f, "AND" },

						//RV32M Extension
						{ 0x2000033, 0xfe00707f, "MUL" }, 
						{ 0x2001033, 0xfe00707f, "MULH" }, 
						{ 0x2002033, 0xfe00707f, "MULHSU" }, 
						{ 0x2003033, 0xfe00707f, "MULHU" }, 
						{ 0x2004033, 0xfe00707f, "DIV" }, 
						{ 0x2005033, 0xfe00707f, "DIVU" }, 
						{ 0x2006033, 0xfe00707f, "REM" }, 
						{ 0x2007033, 0xfe00707f, "REMU" }
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
						{ 0x4000503b, 0xfe00707f, "SRAW" }, 

						//RV64M Extension
						{ 0x200003b, 0xfe00707f, "MULW" }, 
						{ 0x200403b, 0xfe00707f, "DIVW" }, 
						{ 0x200503b, 0xfe00707f, "DIVUW" }, 
						{ 0x200603b, 0xfe00707f, "REMW" }, 
						{ 0x200703b, 0xfe00707f, "REMUW" }
					} 
			},

			{ 0x2f, {
						//RV32A Extension
						{ 0x202f, 0xf800707f, "AMOADD.W"}, 
						{ 0x2000202f, 0xf800707f, "AMOXOR.W"}, 
						{ 0x4000202f, 0xf800707f, "AMOOR.W"}, 
						{ 0x6000202f, 0xf800707f, "AMOAND.W"}, 
						{ 0x8000202f, 0xf800707f, "AMOMIN.W"}, 
						{ 0xa000202f, 0xf800707f, "AMOMAX.W"}, 
						{ 0xc000202f, 0xf800707f, "AMOMINU.W"}, 
						{ 0xe000202f, 0xf800707f, "AMOMAXU.W"}, 
						{ 0x800202f, 0xf800707f, "AMOSWAP.W"}, 
						{ 0x1000202f, 0xf9f0707f, "LR.W"}, 
						{ 0x1800202f, 0xf800707f, "SC.W"},

						//RV64A Extension
						{ 0x302f, 0xf800707f, "AMOADD.D"}, 
						{ 0x2000302f, 0xf800707f, "AMOXOR.D"},
						{ 0x4000302f, 0xf800707f, "AMOOR.D"}, 
						{ 0x6000302f, 0xf800707f, "AMOAND.D"}, 
						{ 0x8000302f, 0xf800707f, "AMOMIN.D"}, 
						{ 0xa000302f, 0xf800707f, "AMOMAX.D"}, 
						{ 0xc000302f, 0xf800707f, "AMOMINU.D"}, 
						{ 0xe000302f, 0xf800707f, "AMOMAXU.D"}, 
						{ 0x800302f, 0xf800707f, "AMOSWAP.D"}, 
						{ 0x1000302f, 0xf9f0707f, "LR.D"}, 
						{ 0x1800302f, 0xf800707f, "SC.D"}
					} 
			},

			//RV(32/64)(F/D/Q) Extensions
			{ 0x07, {} }, 

			{ 0x27, {} },

			{ 0x43, {} },

			{ 0x47, {} },

			{ 0x4B, {} }, 

			{ 0x4F, {} }, 

			{ 0x53, {} } 
		};
	}
}

//F
/*
{ 0x53, { {0x53, 0xfe00007f, "FADD.S" }, { 0x8000053, 0xfe00007f, "FSUB.S" }, { 0x10000053, 0xfe00007f, "FMUL.S" }, { 0x18000053, 0xfe00007f, "FDIV.S" }, { 0x20000053, 0xfe00707f, "FSGNJ.S" }, { 0x20001053, 0xfe00707f, "FSGNJN.S" }, { 0x20002053, 0xfe00707f, "FSGNJX.S" }, { 0x28000053, 0xfe00707f, "FMIN.S" }, { 0x28001053, 0xfe00707f, "FMAX.S" }, { 0x58000053, 0xfff0007f, "FSQRT.S" }, { 0xa0000053, 0xfe00707f, "FLE.S" }, { 0xa0001053, 0xfe00707f, "FLT.S" }, { 0xa0002053, 0xfe00707f, "FEQ.S" }, { 0xc0000053, 0xfff0007f, "FCVT.W.S" }, { 0xc0100053, 0xfff0007f, "FCVT.WU.S" }, { 0xe0000053, 0xfff0707f, "FMV.X.W" }, { 0xe0001053, 0xfff0707f, "FCLASS.S" }, { 0xd0000053, 0xfff0007f, "FCVT.S.W" }, { 0xd0100053, 0xfff0007f, "FCVT.S.WU" }, { 0xf0000053, 0xfff0707f, "FMV.W.X" } } },
{ 0x7, { {0x2007, 0x707f, "FLW" } } },
{ 0x27, { {0x2027, 0x707f, "FSW" } } },
{ 0x43, { {0x43, 0x600007f, "FMADD.S" } } },
{ 0x47, { {0x47, 0x600007f, "FMSUB.S" } } },
{ 0x4b, { {0x4b, 0x600007f, "FNMSUB.S" } } },
{ 0x4f, { {0x4f, 0x600007f, "FNMADD.S" } } },
{ 0x53, { {0xc0200053, 0xfff0007f, "FCVT.L.S" }, { 0xc0300053, 0xfff0007f, "FCVT.LU.S" }, { 0xd0200053, 0xfff0007f, "FCVT.S.L" }, { 0xd0300053, 0xfff0007f, "FCVT.S.LU" }
*/


//D
/*
{ 0x53, { {0x2000053, 0xfe00007f, "FADD.D" }, { 0xa000053, 0xfe00007f, "FSUB.D" }, { 0x12000053, 0xfe00007f, "FMUL.D" }, { 0x1a000053, 0xfe00007f, "FDIV.D" }, { 0x22000053, 0xfe00707f, "FSGNJ.D" }, { 0x22001053, 0xfe00707f, "FSGNJN.D" }, { 0x22002053, 0xfe00707f, "FSGNJX.D" }, { 0x2a000053, 0xfe00707f, "FMIN.D" }, { 0x2a001053, 0xfe00707f, "FMAX.D" }, { 0x40100053, 0xfff0007f, "FCVT.S.D" }, { 0x42000053, 0xfff0007f, "FCVT.D.S" }, { 0x5a000053, 0xfff0007f, "FSQRT.D" }, { 0xa2000053, 0xfe00707f, "FLE.D" }, { 0xa2001053, 0xfe00707f, "FLT.D" }, { 0xa2002053, 0xfe00707f, "FEQ.D" }, { 0xc2000053, 0xfff0007f, "FCVT.W.D" }, { 0xc2100053, 0xfff0007f, "FCVT.WU.D" }, { 0xe2001053, 0xfff0707f, "FCLASS.D" }, { 0xd2000053, 0xfff0007f, "FCVT.D.W" }, { 0xd2100053, 0xfff0007f, "FCVT.D.WU" } } },
{ 0x7, { {0x3007, 0x707f, "FLD" } } },
{ 0x27, { {0x3027, 0x707f, "FSD" } } },
{ 0x43, { {0x2000043, 0x600007f, "FMADD.D" } } },
{ 0x47, { {0x2000047, 0x600007f, "FMSUB.D" } } },
{ 0x4b, { {0x200004b, 0x600007f, "FNMSUB.D" } } },
{ 0x4f, { {0x200004f, 0x600007f, "FNMADD.D" } } },
{ 0x53, { {0xc2200053, 0xfff0007f, "FCVT.L.D" }, { 0xc2300053, 0xfff0007f, "FCVT.LU.D" }, { 0xe2000053, 0xfff0707f, "FMV.X.D" }, { 0xd2200053, 0xfff0007f, "FCVT.D.L" }, { 0xd2300053, 0xfff0007f, "FCVT.D.LU" }, { 0xf2000053, 0xfff0707f, "FMV.D.X" }
*/

//Q
/*
{ 0x53, { {0x6000053, 0xfe00007f, "FADD.Q" }, { 0xe000053, 0xfe00007f, "FSUB.Q" }, { 0x16000053, 0xfe00007f, "FMUL.Q" }, { 0x1e000053, 0xfe00007f, "FDIV.Q" }, { 0x26000053, 0xfe00707f, "FSGNJ.Q" }, { 0x26001053, 0xfe00707f, "FSGNJN.Q" }, { 0x26002053, 0xfe00707f, "FSGNJX.Q" }, { 0x2e000053, 0xfe00707f, "FMIN.Q" }, { 0x2e001053, 0xfe00707f, "FMAX.Q" }, { 0x40300053, 0xfff0007f, "FCVT.S.Q" }, { 0x46000053, 0xfff0007f, "FCVT.Q.S" }, { 0x42300053, 0xfff0007f, "FCVT.D.Q" }, { 0x46100053, 0xfff0007f, "FCVT.Q.D" }, { 0x5e000053, 0xfff0007f, "FSQRT.Q" }, { 0xa6000053, 0xfe00707f, "FLE.Q" }, { 0xa6001053, 0xfe00707f, "FLT.Q" }, { 0xa6002053, 0xfe00707f, "FEQ.Q" }, { 0xc6000053, 0xfff0007f, "FCVT.W.Q" }, { 0xc6100053, 0xfff0007f, "FCVT.WU.Q" }, { 0xe6001053, 0xfff0707f, "FCLASS.Q" }, { 0xd6000053, 0xfff0007f, "FCVT.Q.W" }, { 0xd6100053, 0xfff0007f, "FCVT.Q.WU" } } },
{ 0x7, { {0x4007, 0x707f, "FLQ" } } },
{ 0x27, { {0x4027, 0x707f, "FSQ" } } },
{ 0x43, { {0x6000043, 0x600007f, "FMADD.Q" } } },
{ 0x47, { {0x6000047, 0x600007f, "FMSUB.Q" } } },
{ 0x4b, { {0x600004b, 0x600007f, "FNMSUB.Q" } } },
{ 0x4f, { {0x600004f, 0x600007f, "FNMADD.Q" } } },
{ 0x53, { {0xc6200053, 0xfff0007f, "FCVT.L.Q" }, { 0xc6300053, 0xfff0007f, "FCVT.LU.Q" }, { 0xd6200053, 0xfff0007f, "FCVT.Q.L" }, { 0xd6300053, 0xfff0007f, "FCVT.Q.LU" }
*/