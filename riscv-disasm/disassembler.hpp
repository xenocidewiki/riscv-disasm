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
#pragma once

#include "instructions.hpp"

namespace riscv {
	enum class isa
	{
		RV32,
		RV64,
		RV128
	};

	class disassembler
	{
		using instruction_data = std::tuple<uint32_t, uint32_t, std::string, instruction::instruction_flags>;

		std::vector<instruction::object> m_instructions;
		isa m_architecture;

		//Use std::span when msvc decides to get off their lazy ass and implement it
		std::vector<instruction::object> get_instructions(const std::vector<uint32_t>& code);
		std::vector<instruction::object> get_instructions(std::vector<uint32_t>&& code);

		void parse_instruction(const instruction::type_i& instruction);
		void parse_instruction(const instruction::type_r& instruction);
		void parse_instruction(const instruction::type_r4& instruction);
		void parse_instruction(const instruction::type_b& instruction);
		void parse_instruction(const instruction::type_u& instruction);
		void parse_instruction(const instruction::type_s& instruction);
		void parse_instruction(const instruction::type_j& instruction);
		void parse_instruction(const instruction::type_cr& instruction);
		void parse_instruction(const instruction::type_ci& instruction);
		void parse_instruction(const instruction::type_css& instruction);
		void parse_instruction(const instruction::type_ciw& instruction);
		void parse_instruction(const instruction::type_cl& instruction);
		void parse_instruction(const instruction::type_cs& instruction);
		void parse_instruction(const instruction::type_ca& instruction);
		void parse_instruction(const instruction::type_cb& instruction);
		void parse_instruction(const instruction::type_cj& instruction);

		void fence_instruction_handler(const std::string& mnemonic, const signed int& imm);
		void float_instruction_handler(const instruction::type_r& instruction, instruction_data instr_data);
		void a_ext_instruction_handler(const instruction::type_r& instruction, instruction_data instr_data);

	public:
		disassembler() = delete;
		disassembler(const disassembler& disasm) = delete;
		disassembler(disassembler&& disasm) = delete;

		disassembler(const std::vector<uint32_t>& code, const isa arch) : m_instructions{ get_instructions(code) }, m_architecture{ arch }
		{}

		disassembler(std::vector<uint32_t>&& code, const isa arch) : m_instructions{ get_instructions(std::move(code)) }, m_architecture{ arch }
		{}

		void parse_instructions();
	};
}