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

				//check for shamt instructions, can be done better but w/e, can refactor later
				if (mnemonic == "SLLI" || mnemonic == "SLLIW" || mnemonic == "SRLI" || mnemonic == "SRLIW" || mnemonic == "SRAI" || mnemonic == "SRAIW") {
					immediate = (immediate & 0x3F);
				}

				//Check for fence instructions, add handler l8r
				if (mnemonic == "FENCE") {

				}

				if (mnemonic == "ECALL" || mnemonic == "EBREAK") {
					std::cout << mnemonic << "\n";
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
				auto& destination	= registers::x_reg_name_table[instruction.rd].second;
				auto& var_add_1		= registers::x_reg_name_table[instruction.rs1].second;
				auto& var_add_2		= registers::x_reg_name_table[instruction.rs2].second;

				std::cout << mnemonic << " " << destination << ", " << var_add_1 << ", " << var_add_2 << "\n";

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