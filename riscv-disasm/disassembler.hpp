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

		void fence_instruction_handler(const std::string& mnemonic, const signed int& imm);
		void float_instruction_handler(const instruction::type_r& instruction, instruction_data instr_data);
		void sl_instruction_handler(const signed int& imm);
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