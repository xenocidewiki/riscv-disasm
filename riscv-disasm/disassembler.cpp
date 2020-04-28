#include "disassembler.hpp"
#include "registers.hpp"

namespace riscv
{
	template<>
	void disassembler::parse_instruction<instruction::type_identifier::B>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::I>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::J>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::R>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::R4>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::U>() {

	}

	template<>
	void disassembler::parse_instruction<instruction::type_identifier::S>() {

	}
}