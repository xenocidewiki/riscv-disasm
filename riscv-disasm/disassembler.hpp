#pragma once
#include "instructions.hpp"

namespace riscv {
	class disassembler
	{
		std::vector<instruction::object> m_instructions;

		template <instruction::type_identifier instruction_type>
		void parse_instruction();

	public:
		disassembler() = default;
		disassembler(const disassembler& disasm) = delete;
		disassembler(disassembler&& disasm) = delete;

		void parse(const std::vector<uint32_t>& code);
		void parse(std::vector<uint32_t>&& code);

		void print_instructions();
	};
}