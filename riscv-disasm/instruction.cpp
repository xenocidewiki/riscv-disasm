#include "instructions.hpp"

namespace riscv {
	namespace instruction {
		const uint8_t object::get_opcode(const uint32_t& instruction) const
		{
			/*
			Clears out all bits except last 7 (opcode)
			*/

			return instruction & 0x0000007f;
		}

		const type_identifier object::set_instruction_format(const uint32_t& instruction) const
		{
			const uint8_t opcode = get_opcode(instruction);
			
			return opcode_instruction_type.at(opcode);
		}

		const object::instruction_format object::set_instruction_data(const type_identifier& type, const uint32_t& instruction)
		{	
			switch(type)
			{
			case type_identifier::B:
				return type_b{ instruction };

			case type_identifier::I:
				return type_i{ instruction };

			case type_identifier::J:
				return type_j{ instruction };

			case type_identifier::R:
				return type_r{ instruction };

			case type_identifier::R4:
				return type_r4{ instruction };

			case type_identifier::S:
				return type_s{ instruction };

			case type_identifier::U:
				return type_u{ instruction };

			default:
				return type_r{ 0 };
			}
		}
	}
}