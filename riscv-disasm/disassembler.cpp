//	Copyright(C) 2020 xenocidewiki
//	This file is part of riscv-disasm.
//
//	riscv-disasm is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	riscv-disasm is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with riscv-disasm. If not, see <https://www.gnu.org/licenses/>.
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
			switch (instruction.get_type())
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
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rd].second;
				auto& source = registers::x_reg_name_table[instruction.rs1].second;
				signed int immediate = instruction.imm;

				if (flags.is_fence) {
					fence_instruction_handler(mnemonic, immediate);
					return;
				}

				if (flags.is_e_sys) {
					std::cout << mnemonic << "\n";
					return;
				}

				//double check this, these are CSR setting instructions that use rs1 as an immediate rather than register
				//do we need to make a function for this or eh? (seems wasteful)
				if (flags.is_imm_csr) {
					uint8_t csr_source = instruction.rs1;

					std::cout << mnemonic << " " << destination << ", 0x" << std::hex << csr_source << ", 0x" << immediate << "\n";
					return;
				}

				//Make better l8r
				if (flags.is_float) {
					destination = registers::f_reg_name_table[instruction.rd].second;
					source = registers::f_reg_name_table[instruction.rs1].second;
				}

				//check for shamt instructions
				if (flags.is_shamt)
					immediate = (immediate & 0x3F); //This handles both the RV64I and RV32I case, note that this is 000000111111, this will pull out the shamt correctly for both, look in manual

				if (flags.is_sl) {
					std::cout << mnemonic << " " << destination << ", 0x" << std::hex << immediate << "(" << source << ")\n";
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
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {

				//A extension checks
				if (flags.is_a_ext) {
					a_ext_instruction_handler(instruction, instr_data);
					return;
				}

				if (flags.is_float) {
					float_instruction_handler(instruction, instr_data);
					return;
				}

				auto& destination = registers::x_reg_name_table[instruction.rd].second;
				auto& middle = registers::x_reg_name_table[instruction.rs1].second;
				auto& last = registers::x_reg_name_table[instruction.rs2].second;

				std::cout << mnemonic << " " << destination << ", " << middle << ", " << last << "\n";

				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_r4& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::f_reg_name_table[instruction.rd].second;
				auto& first = registers::f_reg_name_table[instruction.rs1].second;
				auto& middle = registers::f_reg_name_table[instruction.rs2].second;
				auto& last = registers::f_reg_name_table[instruction.rs3].second;
				auto& mode = instruction::float_rounding_name[instruction.funct3].second;

				std::cout << mnemonic << "(" << mode << ") " << destination << ", " << first << ", " << middle << ", " << last << "\n";

				return;
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_b& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rs1].second;
				auto& source = registers::x_reg_name_table[instruction.rs2].second;
				signed int immediate = 0;

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
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rd].second;
				signed int immediate = instruction.imm;

				std::cout << mnemonic << " " << destination << ", 0x" << std::hex << immediate << "\n";
			}
		}
	}

	void disassembler::parse_instruction(const instruction::type_s& instruction)
	{
		auto potential_instructions = instruction::instruction_table.at(instruction.opcode);

		for (auto& instr_data : potential_instructions)
		{
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rs2].second;
				auto& source = registers::x_reg_name_table[instruction.rs1].second;
				signed int immediate = 0;

				if (flags.is_float) {
					destination = registers::f_reg_name_table[instruction.rs2].second;
					source = registers::f_reg_name_table[instruction.rs1].second;
				}

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
			auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

			if ((instruction.instruction & mask) == proper_opcode) {
				auto& destination = registers::x_reg_name_table[instruction.rd].second;
				signed int immediate = 0;

				if (flags.is_float)
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

	void parse_instruction(const instruction::type_cr& instruction)
	{}

	void parse_instruction(const instruction::type_ci& instruction)
	{}

	void parse_instruction(const instruction::type_css& instruction)
	{}

	void parse_instruction(const instruction::type_ciw& instruction)
	{}

	void parse_instruction(const instruction::type_cl& instruction)
	{}

	void parse_instruction(const instruction::type_cs& instruction)
	{}

	void parse_instruction(const instruction::type_ca& instruction)
	{}

	void parse_instruction(const instruction::type_cb& instruction)
	{}

	void parse_instruction(const instruction::type_cj& instruction)
	{}
}