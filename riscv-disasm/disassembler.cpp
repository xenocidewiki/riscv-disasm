#include "disassembler.hpp"
#include "registers.hpp"
#include <iostream>

namespace riscv
{
	std::vector<instruction::object> disassembler::get_instructions(const std::vector<uint32_t>& code)
	{
		std::vector<instruction::object> instruction_objects;
		for (auto instruction : code)
		{
			instruction_objects.emplace_back(instruction::object{ instruction });
		}

		return instruction_objects;
	}

	std::vector<instruction::object> disassembler::get_instructions(std::vector<uint32_t>&& code)
	{
		return get_instructions(code);
	}

	void disassembler::parse_instructions()
	{
		for (const instruction::object& instruction : m_instructions)
		{
			switch(instruction.get_type()) 
			{
			case instruction::type_identifier::R:
				parse_instruction(std::get<instruction::type_r>(instruction.get_data()));
				break;

			case instruction::type_identifier::R4:
				parse_instruction(std::get<instruction::type_r4>(instruction.get_data()));
				break;

			case instruction::type_identifier::I:
				parse_instruction(std::get<instruction::type_i>(instruction.get_data()));
				break;

			case instruction::type_identifier::J:
				parse_instruction(std::get<instruction::type_j>(instruction.get_data()));
				break;

			case instruction::type_identifier::U:
				parse_instruction(std::get<instruction::type_u>(instruction.get_data()));
				break;

			case instruction::type_identifier::S:
				parse_instruction(std::get<instruction::type_s>(instruction.get_data()));
				break;

			case instruction::type_identifier::B:
				parse_instruction(std::get<instruction::type_b>(instruction.get_data()));
				break;

			default:
				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_i& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination		= registers::x_reg_name_table[instruction.rd].second;
				auto& source			= registers::x_reg_name_table[instruction.rs1].second;
				signed int immediate	= instruction.imm;

				//Make better l8r
				if (mnemonic == "FLW" || mnemonic == "FLD" || mnemonic == "FLQ") {
					destination = registers::f_reg_name_table[instruction.rd].second;
					source		= registers::f_reg_name_table[instruction.rs1].second;
				}

				//check for shamt instructions, can be done better but w/e, can refactor later
				if (mnemonic == "SLLI" || mnemonic == "SLLIW" || mnemonic == "SRLI" || mnemonic == "SRLIW" || mnemonic == "SRAI" || mnemonic == "SRAIW")
						immediate = (immediate & 0x3F); //This handles both the RV64I and RV32I case, note that this is 000000111111, this will pull out the shamt correctly for both, look in manual

				if (mnemonic == "FENCE.I") {
					std::cout << mnemonic << "\n";
					return;
				}

				if (mnemonic == "FENCE") {
					//FENCE.TSO
					if (immediate >> 8) {
						std::cout << "FENCE.TSO rw, rw\n";
						return;
					}

					//For the love of god move all of this to another function
					union iorw
					{
						int encoding;
						struct {
							int8_t w : 1;
							int8_t r : 1;
							int8_t o : 1;
							int8_t i : 1;
							int8_t pad : 4;
						};
					};

					auto successor   = iorw{ immediate & 0x0f };
					auto predecessor = iorw{ (immediate & 0b000011110000) >> 4 };

					std::string succ_str;
					std::string pre_str;

					if (successor.i != 0)
						succ_str.append("i");
					if (successor.o != 0)
						succ_str.append("o");
					if (successor.r != 0)
						succ_str.append("r");
					if (successor.w != 0)
						succ_str.append("w");

					if (predecessor.i != 0)
						pre_str.append("i");
					if (predecessor.o != 0)
						pre_str.append("o");
					if (predecessor.r != 0)
						pre_str.append("r");
					if (predecessor.w != 0)
						pre_str.append("w");

					
					std::cout << mnemonic << " " << pre_str << ", " << succ_str << "\n";
					return;
				}

				if (mnemonic == "ECALL" || mnemonic == "EBREAK") {
					std::cout << mnemonic << "\n";
					return;
				}

				//double check this, these are CSR setting instructions that use rs1 as an immediate rather than register
				if (mnemonic == "CSRRWI" || mnemonic == "CSRRSI" || mnemonic == "CSRRCI") {
					uint8_t csr_source = instruction.rs1;

					std::cout << mnemonic << " " << destination << ", 0x" << std::hex << csr_source << ", 0x" << immediate << "\n";
					return;
				}

				std::cout << mnemonic << " " << destination << ", " << source << ", 0x" << std::hex << immediate << "\n";
				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_r& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {

				//A extension checks, make this a function l8r 
				if (instruction.opcode == 0x2F) {
					auto& destination	= registers::x_reg_name_table[instruction.rd].second;
					auto& address		= registers::x_reg_name_table[instruction.rs1].second;
					auto& middle		= registers::x_reg_name_table[instruction.rs2].second;

					std::string aq_rl{};

					uint8_t aq = instruction.funct7 & 0b0000010;
					uint8_t rl = instruction.funct7 & 0b0000001;

					if (aq)
						aq_rl.append(".AQ");
					if (rl)
						aq_rl.append(".RL");

					if (mnemonic == "LR.W" || mnemonic == "LR.D") {
						std::cout << mnemonic << aq_rl << " " << destination << ", (" << address << ")\n";
						return;
					}

					std::cout << mnemonic << aq_rl << " " << destination << ", " << middle << ", (" << address << ")\n";
					return;
				}

				auto& destination	= registers::x_reg_name_table[instruction.rd].second;
				auto& middle		= registers::x_reg_name_table[instruction.rs1].second;
				auto& last		= registers::x_reg_name_table[instruction.rs2].second;

				std::cout << mnemonic << " " << destination << ", " << middle << ", " << last << "\n";

				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_r4& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_b& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination		= registers::x_reg_name_table[instruction.rs1].second;
				auto& source			= registers::x_reg_name_table[instruction.rs2].second;
				signed int immediate	= 0;

				//Might be wrong, make sure to double check....
				immediate |= (instruction.imm_a << 11);
				immediate |= (instruction.imm_b << 1);
				immediate |= (instruction.imm_c << 5);
				immediate |= (instruction.imm_d << 12);

				//Implement actual pc relative addressing later
				std::cout << mnemonic << " " << destination << ", " << source << ", 0x" << std::hex << immediate << "(pc)\n";

				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_u& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination		= registers::x_reg_name_table[instruction.rd].second;
				signed int immediate	= instruction.imm;

				std::cout << mnemonic << " " << destination << ", 0x" << std::hex << immediate << "\n";
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_s& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination		= registers::x_reg_name_table[instruction.rs2].second;
				auto& source			= registers::x_reg_name_table[instruction.rs1].second;
				signed int immediate	= 0;

				immediate |= instruction.imm_a;
				immediate |= (instruction.imm_b << 5);

				std::cout << mnemonic << " " << destination << ", 0x" << std::hex << immediate << "(" << source << ")\n";
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_j& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rd].second;
				signed int immediate = 0;

				if (mnemonic == "FSW" || mnemonic == "FSD" || mnemonic == "FSQ")
					destination = registers::f_reg_name_table[instruction.rd].second;
				
				//double check
				immediate |= (instruction.imm_a << 12);
				immediate |= (instruction.imm_b << 11);
				immediate |= (instruction.imm_c << 1);
				immediate |= (instruction.imm_d << 20);

				std::cout << mnemonic << " " << destination << ", 0x" << std::hex << immediate << "(pc)\n";
			}
		}
	}
}