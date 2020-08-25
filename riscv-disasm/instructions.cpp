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
#include "instructions.hpp"

namespace riscv {
	namespace instruction {
		const uint8_t object::get_opcode(const uint32_t instruction) const
		{
			/*
			Clears out all bits except last 7 (opcode)
			*/

			return instruction & 0x0000007f;
		}

		//add errorchecks here omegalul
		const type_identifier object::set_instruction_format(const uint32_t instruction) const
		{
			const uint8_t opcode = get_opcode(instruction);
			
			return opcode_instruction_type.at(opcode);
		}

		const object::instruction_format object::set_instruction_data(const type_identifier type, const uint32_t instruction)
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

			case type_identifier::CEXT:
				return cext_handler(instruction); //TODO: Pass instruction as uint16_t make sure that works properly
				break;

			default:
				return type_r{ 0 };
			}
		}

		const object::instruction_format object::get_data() const
		{
			return m_data;
		}

		const type_identifier object::get_type() const
		{
			return m_type;
		}

		const object::instruction_format object::cext_handler(const uint16_t instruction) const
		{

		}
	}
}