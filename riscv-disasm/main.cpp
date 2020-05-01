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
#include "riscv.hpp"

int main(int argc, char* argv[])
{
	std::vector<uint32_t> inst_test = { 
		0xE218103, 0x00850463, 0x00002e17, 0xee1ff0ef, 0x00E12423, 
		0x4027d79b, 0x40f707bb, 0x0CF2030F, 0x940133, 0x1e30a12f, 0x1200a12f,
		0x80660143, 0x257106ef, 0x55533107
	};

	riscv::disassembler disasm { inst_test, riscv::isa::RV64 };

	disasm.parse_instructions();
	return 0;
}