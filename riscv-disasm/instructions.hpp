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
		
		//essentially maps each opcode to a set of instructions with said opcode, which each have a { match, mask, mnemonic, is_load_or_store_instr, is_floating_pt } tuple that we can use for parsing and checking which instruction it actually is
		//change to constexpr when msvc decides to fucking implement it -_-
		const std::unordered_map<uint8_t, const std::vector<std::tuple<uint32_t, uint32_t, std::string, bool, bool>> > instruction_table {
			//RV32I Instructions
			{ 0x63, {
						{ 0x63, 0x707f, "BEQ", false, false }, 
						{ 0x1063, 0x707f, "BNE", false, false },
						{ 0x4063, 0x707f, "BLT", false, false },
						{ 0x5063, 0x707f, "BGE", false, false },
						{ 0x6063, 0x707f, "BLTU", false, false },
						{ 0x7063, 0x707f, "BGEU", false, false }
					} 
			},

			{ 0x67, { 
						{ 0x67, 0x707f, "JALR", false, false }
					} 
			},

			{ 0x6f, { 
						{ 0x6f, 0x7f, "JAL", false, false }
					} 
			},

			{ 0x37, { 
						{ 0x37, 0x7f, "LUI", true, false }
					} 
			},

			{ 0x17, { 
						{ 0x17, 0x7f, "AUIPC", false, false }
					}
			},

			{ 0x13, { 
						{ 0x13, 0x707f, "ADDI", false, false },
						{ 0x1013, 0xfc00707f, "SLLI", false, false },
						{ 0x2013, 0x707f, "SLTI", false, false },
						{ 0x3013, 0x707f, "SLTIU", false, false },
						{ 0x4013, 0x707f, "XORI", false, false },
						{ 0x5013, 0xfc00707f, "SRLI", false, false },
						{ 0x40005013, 0xfc00707f, "SRAI", false, false },
						{ 0x6013, 0x707f, "ORI", false, false },
						{ 0x7013, 0x707f, "ANDI", false, false }
					} 
			},
			
			{ 0x33, { 
						{ 0x33, 0xfe00707f, "ADD", false, false },
						{ 0x40000033, 0xfe00707f, "SUB", false, false },
						{ 0x1033, 0xfe00707f, "SLL", false, false },
						{ 0x2033, 0xfe00707f, "SLT", false, false },
						{ 0x3033, 0xfe00707f, "SLTU", false, false },
						{ 0x4033, 0xfe00707f, "XOR", false, false },
						{ 0x5033, 0xfe00707f, "SRL", false, false },
						{ 0x40005033, 0xfe00707f, "SRA", false, false },
						{ 0x6033, 0xfe00707f, "OR", false, false },
						{ 0x7033, 0xfe00707f, "AND", false, false },

						//RV32M Extension
						{ 0x2000033, 0xfe00707f, "MUL", false, false },
						{ 0x2001033, 0xfe00707f, "MULH", false, false },
						{ 0x2002033, 0xfe00707f, "MULHSU", false, false },
						{ 0x2003033, 0xfe00707f, "MULHU", false, false },
						{ 0x2004033, 0xfe00707f, "DIV", false, false },
						{ 0x2005033, 0xfe00707f, "DIVU", false, false },
						{ 0x2006033, 0xfe00707f, "REM", false, false },
						{ 0x2007033, 0xfe00707f, "REMU", false, false }
					} 
			},

			{ 0x03, { 
						{ 0x3, 0x707f, "LB", true, false },
						{ 0x1003, 0x707f, "LH", true, false },
						{ 0x2003, 0x707f, "LW", true, false },
						{ 0x4003, 0x707f, "LBU", true, false },
						{ 0x5003, 0x707f, "LHU", true, false },

						//RV64I Instructions
						{ 0x3003, 0x707f, "LD", true, false },
						{ 0x6003, 0x707f, "LWU", true, false }
					} 
			},

			{ 0x23, { 
						{ 0x23, 0x707f, "SB", true, false },
						{ 0x1023, 0x707f, "SH", true, false },
						{ 0x2023, 0x707f, "SW", true, false },

						//RV64I Instruction
						{ 0x3023, 0x707f, "SD", true, false }
					} 
			},

			{ 0x0f, { 
						{ 0xf, 0x707f, "FENCE", false, false },

						//Zfencei instruction
						{ 0x100f, 0x707f, "FENCE.I", false, false }
					} 
			},

			//Zicsr Instructions
			{ 0x73, {
						{ 0x73, 0xffffffff, "ECALL", false, false },
						{ 0x100073, 0xffffffff, "EBREAK", false, false },
						{ 0x1073, 0x707f, "CSRRW", false, false },
						{ 0x2073, 0x707f, "CSRRS", false, false },
						{ 0x3073, 0x707f, "CSRRC", false, false },
						{ 0x5073, 0x707f, "CSRRWI", false, false },
						{ 0x6073, 0x707f, "CSRRSI", false, false },
						{ 0x7073, 0x707f, "CSRRCI", false, false }
					} 
			},

			//RV64I Instructions
			{ 0x1b, { 
						{ 0x1b, 0x707f, "ADDIW", false, false },
						{ 0x101b, 0xfe00707f, "SLLIW", false, false },
						{ 0x501b, 0xfe00707f, "SRLIW", false, false },
						{ 0x4000501b, 0xfe00707f, "SRAIW", false, false }
					} 
			},

			{ 0x3b, { 
						{ 0x3b, 0xfe00707f, "ADDW", false, false },
						{ 0x4000003b, 0xfe00707f, "SUBW", false, false },
						{ 0x103b, 0xfe00707f, "SLLW", false, false },
						{ 0x503b, 0xfe00707f, "SRLW", false, false },
						{ 0x4000503b, 0xfe00707f, "SRAW", false, false },

						//RV64M Extension
						{ 0x200003b, 0xfe00707f, "MULW", false, false },
						{ 0x200403b, 0xfe00707f, "DIVW", false, false },
						{ 0x200503b, 0xfe00707f, "DIVUW", false, false },
						{ 0x200603b, 0xfe00707f, "REMW", false, false },
						{ 0x200703b, 0xfe00707f, "REMUW", false, false }
					} 
			},

			{ 0x2f, {
						//RV32A Extension
						{ 0x202f, 0xf800707f, "AMOADD.W", false, false },
						{ 0x2000202f, 0xf800707f, "AMOXOR.W", false, false },
						{ 0x4000202f, 0xf800707f, "AMOOR.W", false, false },
						{ 0x6000202f, 0xf800707f, "AMOAND.W", false, false },
						{ 0x8000202f, 0xf800707f, "AMOMIN.W", false, false },
						{ 0xa000202f, 0xf800707f, "AMOMAX.W", false, false },
						{ 0xc000202f, 0xf800707f, "AMOMINU.W", false, false },
						{ 0xe000202f, 0xf800707f, "AMOMAXU.W", false, false },
						{ 0x800202f, 0xf800707f, "AMOSWAP.W", false, false },
						{ 0x1000202f, 0xf9f0707f, "LR.W", true, false },
						{ 0x1800202f, 0xf800707f, "SC.W", true, false },

						//RV64A Extension
						{ 0x302f, 0xf800707f, "AMOADD.D", false, false },
						{ 0x2000302f, 0xf800707f, "AMOXOR.D", false, false },
						{ 0x4000302f, 0xf800707f, "AMOOR.D", false, false },
						{ 0x6000302f, 0xf800707f, "AMOAND.D", false, false },
						{ 0x8000302f, 0xf800707f, "AMOMIN.D", false, false },
						{ 0xa000302f, 0xf800707f, "AMOMAX.D", false, false },
						{ 0xc000302f, 0xf800707f, "AMOMINU.D", false, false },
						{ 0xe000302f, 0xf800707f, "AMOMAXU.D", false, false },
						{ 0x800302f, 0xf800707f, "AMOSWAP.D", false, false },
						{ 0x1000302f, 0xf9f0707f, "LR.D", true, false },
						{ 0x1800302f, 0xf800707f, "SC.D", true, false }
					} 
			},

			//RV(32/64)(F/D/Q) Extensions
			{ 0x07, {
						//RV32/64F Extension
						{ 0x2007, 0x707f, "FLW", true, true },
						{ 0x3007, 0x707f, "FLD", true, true },
						{ 0x4007, 0x707f, "FLQ", true, true }
					} 
			}, 

			{ 0x27, {
						//RV32/64F Extension
						{ 0x2027, 0x707f, "FSW", true, true },
						{ 0x3027, 0x707f, "FSD", true, true },
						{ 0x4027, 0x707f, "FSQ", true, true }
					} 
			},

			{ 0x43, {
						//RV32/64F Extension
						{ 0x43, 0x600007f, "FMADD.S", false, true },
						{ 0x2000043, 0x600007f, "FMADD.D", false, true },
						{ 0x6000043, 0x600007f, "FMADD.Q", false, true }
					} 
			},

			{ 0x47, {
						//RV32/64F Extension
						{ 0x47, 0x600007f, "FMSUB.S", false, true },
						{ 0x2000047, 0x600007f, "FMSUB.D", false, true },
						{ 0x6000047, 0x600007f, "FMSUB.Q", false, true }
					} 
			},

			{ 0x4B, {
						//RV32/64F Extension
						{ 0x4b, 0x600007f, "FNMSUB.S", false, true },
						{ 0x200004b, 0x600007f, "FNMSUB.D", false, true },
						{ 0x600004b, 0x600007f, "FNMSUB.Q", false, true }
					} 
			}, 

			{ 0x4F, {
						//RV32/64F Extension
						{ 0x4f, 0x600007f, "FNMADD.S", false, true },
						{ 0x200004f, 0x600007f, "FNMADD.D", false, true },
						{ 0x600004f, 0x600007f, "FNMADD.Q", false, true }
					}
			}, 

			{ 0x53, {
						//RV32/64F Extension
						{ 0x53, 0xfe00007f, "FADD.S", false, true }, 
						{ 0x8000053, 0xfe00007f, "FSUB.S", false, true },
						{ 0x10000053, 0xfe00007f, "FMUL.S", false, true },
						{ 0x18000053, 0xfe00007f, "FDIV.S", false, true },
						{ 0x20000053, 0xfe00707f, "FSGNJ.S", false, true },
						{ 0x20001053, 0xfe00707f, "FSGNJN.S", false, true },
						{ 0x20002053, 0xfe00707f, "FSGNJX.S", false, true },
						{ 0x28000053, 0xfe00707f, "FMIN.S", false, true },
						{ 0x28001053, 0xfe00707f, "FMAX.S", false, true },
						{ 0x58000053, 0xfff0007f, "FSQRT.S", false, true },
						{ 0xa0000053, 0xfe00707f, "FLE.S", false, true },
						{ 0xa0001053, 0xfe00707f, "FLT.S", false, true },
						{ 0xa0002053, 0xfe00707f, "FEQ.S", false, true },
						{ 0xc0000053, 0xfff0007f, "FCVT.W.S", false, true },
						{ 0xc0100053, 0xfff0007f, "FCVT.WU.S", false, true },
						{ 0xe0000053, 0xfff0707f, "FMV.X.W", false, true },
						{ 0xe0001053, 0xfff0707f, "FCLASS.S", false, true },
						{ 0xd0000053, 0xfff0007f, "FCVT.S.W", false, true },
						{ 0xd0100053, 0xfff0007f, "FCVT.S.WU", false, true },
						{ 0xf0000053, 0xfff0707f, "FMV.W.X", false, true },
						{ 0xc0200053, 0xfff0007f, "FCVT.L.S", false, true },
						{ 0xc0300053, 0xfff0007f, "FCVT.LU.S", false, true },
						{ 0xd0200053, 0xfff0007f, "FCVT.S.L", false, true },
						{ 0xd0300053, 0xfff0007f, "FCVT.S.LU", false, true },

						//RV32/64D Extension
						{ 0x2000053, 0xfe00007f, "FADD.D", false, true },
						{ 0xa000053, 0xfe00007f, "FSUB.D", false, true },
						{ 0x12000053, 0xfe00007f, "FMUL.D", false, true },
						{ 0x1a000053, 0xfe00007f, "FDIV.D", false, true },
						{ 0x22000053, 0xfe00707f, "FSGNJ.D", false, true },
						{ 0x22001053, 0xfe00707f, "FSGNJN.D", false, true },
						{ 0x22002053, 0xfe00707f, "FSGNJX.D", false, true },
						{ 0x2a000053, 0xfe00707f, "FMIN.D", false, true },
						{ 0x2a001053, 0xfe00707f, "FMAX.D", false, true },
						{ 0x40100053, 0xfff0007f, "FCVT.S.D", false, true },
						{ 0x42000053, 0xfff0007f, "FCVT.D.S", false, true },
						{ 0x5a000053, 0xfff0007f, "FSQRT.D", false, true },
						{ 0xa2000053, 0xfe00707f, "FLE.D", false, true },
						{ 0xa2001053, 0xfe00707f, "FLT.D", false, true },
						{ 0xa2002053, 0xfe00707f, "FEQ.D", false, true },
						{ 0xc2000053, 0xfff0007f, "FCVT.W.D", false, true },
						{ 0xc2100053, 0xfff0007f, "FCVT.WU.D", false, true },
						{ 0xe2001053, 0xfff0707f, "FCLASS.D", false, true },
						{ 0xd2000053, 0xfff0007f, "FCVT.D.W", false, true },
						{ 0xd2100053, 0xfff0007f, "FCVT.D.WU", false, true },
						{ 0xc2200053, 0xfff0007f, "FCVT.L.D", false, true },
						{ 0xc2300053, 0xfff0007f, "FCVT.LU.D", false, true },
						{ 0xe2000053, 0xfff0707f, "FMV.X.D", false, true },
						{ 0xd2200053, 0xfff0007f, "FCVT.D.L", false, true },
						{ 0xd2300053, 0xfff0007f, "FCVT.D.LU", false, true },
						{ 0xf2000053, 0xfff0707f, "FMV.D.X", false, true },

						//RV32/64Q Extension
						{ 0x6000053, 0xfe00007f, "FADD.Q", false, true  },
						{ 0xe000053, 0xfe00007f, "FSUB.Q", false, true  },
						{ 0x16000053, 0xfe00007f, "FMUL.Q", false, true  },
						{ 0x1e000053, 0xfe00007f, "FDIV.Q", false, true  },
						{ 0x26000053, 0xfe00707f, "FSGNJ.Q", false, true  },
						{ 0x26001053, 0xfe00707f, "FSGNJN.Q", false, true  },
						{ 0x26002053, 0xfe00707f, "FSGNJX.Q", false, true  },
						{ 0x2e000053, 0xfe00707f, "FMIN.Q", false, true  },
						{ 0x2e001053, 0xfe00707f, "FMAX.Q", false, true  },
						{ 0x40300053, 0xfff0007f, "FCVT.S.Q", false, true  },
						{ 0x46000053, 0xfff0007f, "FCVT.Q.S", false, true  },
						{ 0x42300053, 0xfff0007f, "FCVT.D.Q", false, true  },
						{ 0x46100053, 0xfff0007f, "FCVT.Q.D", false, true  },
						{ 0x5e000053, 0xfff0007f, "FSQRT.Q", false, true  },
						{ 0xa6000053, 0xfe00707f, "FLE.Q", false, true  },
						{ 0xa6001053, 0xfe00707f, "FLT.Q", false, true  },
						{ 0xa6002053, 0xfe00707f, "FEQ.Q", false, true  },
						{ 0xc6000053, 0xfff0007f, "FCVT.W.Q", false, true  },
						{ 0xc6100053, 0xfff0007f, "FCVT.WU.Q", false, true  },
						{ 0xe6001053, 0xfff0707f, "FCLASS.Q", false, true  },
						{ 0xd6000053, 0xfff0007f, "FCVT.Q.W", false, true  },
						{ 0xd6100053, 0xfff0007f, "FCVT.Q.WU", false, true  },
						{ 0xc6200053, 0xfff0007f, "FCVT.L.Q", false, true  },
						{ 0xc6300053, 0xfff0007f, "FCVT.LU.Q", false, true  },
						{ 0xd6200053, 0xfff0007f, "FCVT.Q.L", false, true  },
						{ 0xd6300053, 0xfff0007f, "FCVT.Q.LU", false, true  }
					}
			} 
		};
	}
}