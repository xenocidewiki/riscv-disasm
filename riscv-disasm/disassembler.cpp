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
				auto& destination	= registers::x_reg_name_table[instruction.rd].second;
				auto& source		= registers::x_reg_name_table[instruction.rs1].second;
				auto& immediate		= instruction.imm;

				std::cout << mnemonic << " " << destination << ", " << source << ", " << immediate;
				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_r& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_r4& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_b& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_u& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_s& instruction)
	{

	}

	void disassembler::parse_instruction(const instruction::type_j& instruction)
	{

	}
}