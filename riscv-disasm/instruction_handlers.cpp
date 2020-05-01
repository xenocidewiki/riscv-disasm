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
	void disassembler::fence_instruction_handler(const std::string& mnemonic, const signed int& immediate)
	{
		if (mnemonic == "FENCE.I") {
			std::cout << mnemonic << "\n";
		} else {

			//FENCE.TSO
			if (immediate >> 8) {
				std::cout << "FENCE.TSO rw, rw\n";
				return;
			}

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

			auto successor = iorw{ immediate & 0x0f };
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
		}
	}

	void disassembler::float_instruction_handler(const instruction::type_r& instruction, instruction_data instr_data)
	{
		auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

		auto& destination	= registers::f_reg_name_table[instruction.rd].second;
		auto& first			= registers::f_reg_name_table[instruction.rs1].second;
		auto& last			= registers::f_reg_name_table[instruction.rs2].second;
		auto& mode			= instruction::float_rounding_name[instruction.funct3].second;

		if (flags.is_special_float) {
			std::cout << mnemonic << "(" << mode << ") " << destination << ", " << first << "\n";
			return;
		}

		std::cout << mnemonic << "(" << mode << ") " << destination << ", " << first << ", " << last << "\n";
	}

	void disassembler::a_ext_instruction_handler(const instruction::type_r& instruction, instruction_data instr_data)
	{
		auto& [proper_opcode, mask, mnemonic, flags] { instr_data };

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

		if (mnemonic == "LR.W" || mnemonic == "LR.D")
			std::cout << mnemonic << aq_rl << " " << destination << ", (" << address << ")\n";
		else 
			std::cout << mnemonic << aq_rl << " " << destination << ", " << middle << ", (" << address << ")\n";
	}
}