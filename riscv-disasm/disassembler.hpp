#pragma once
#include "instructions.hpp"

namespace riscv {
	class disassembler
	{
		std::vector<instruction::object> m_instructions;

		void parse_instruction(const instruction::type_i& instruction);
		void parse_instruction(const instruction::type_r& instruction);
		void parse_instruction(const instruction::type_r4& instruction);
		void parse_instruction(const instruction::type_b& instruction);
		void parse_instruction(const instruction::type_u& instruction);
		void parse_instruction(const instruction::type_s& instruction);
		void parse_instruction(const instruction::type_j& instruction);

		//Use std::span when msvc decides to get off their lazy ass and implement it
		std::vector<instruction::object> get_instructions(const std::vector<uint32_t>& code);
		std::vector<instruction::object> get_instructions(std::vector<uint32_t>&& code);

	public:
		disassembler() = delete;
		disassembler(const disassembler& disasm) = delete;
		disassembler(disassembler&& disasm) = delete;

		disassembler(const std::vector<uint32_t>& code) : m_instructions{ get_instructions(code) }
		{}

		disassembler(std::vector<uint32_t>&& code) : m_instructions{ get_instructions(std::move(code)) }
		{}

		void parse_instructions();
	};
}