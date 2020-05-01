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

#include <array>

namespace riscv
{
	namespace registers
	{
		enum x_reg : uint8_t
		{
			x0, x1, x2, x3, x4, x5, x6, x7,
			x8, x9, x10, x11, x12, x13, x14, x15,
			x16, x17, x18, x19, x20, x21, x22, x23,
			x24, x25, x26, x27, x28, x29, x30, x31
		};

		enum f_reg : uint8_t
		{
			f0, f1, f2, f3, f4, f5, f6, f7,
			f8, f9, f10, f11, f12, f13, f14, f15,
			f16, f17, f18, f19, f20, f21, f22, f23,
			f24, f25, f26, f27, f28, f29, f30, f31
		};

		inline std::array x_reg_name_table = {
			std::pair{x_reg::x0, "zero"},
			std::pair{x_reg::x1, "ra"},
			std::pair{x_reg::x2, "sp"},
			std::pair{x_reg::x3, "gp"},
			std::pair{x_reg::x4, "tp"},
			std::pair{x_reg::x5, "t0"},
			std::pair{x_reg::x6, "t1"},
			std::pair{x_reg::x7, "t2"},
			std::pair{x_reg::x8, "s0"}, 
			std::pair{x_reg::x9, "s1"},
			std::pair{x_reg::x10, "a0"},
			std::pair{x_reg::x11, "a1"},
			std::pair{x_reg::x12, "a2"},
			std::pair{x_reg::x13, "a3"},
			std::pair{x_reg::x14, "a4"},
			std::pair{x_reg::x15, "a5"},
			std::pair{x_reg::x16, "a6"},
			std::pair{x_reg::x17, "a7"},
			std::pair{x_reg::x18, "s2"},
			std::pair{x_reg::x19, "s3"},
			std::pair{x_reg::x20, "s4"},
			std::pair{x_reg::x21, "s5"},
			std::pair{x_reg::x22, "s6"},
			std::pair{x_reg::x23, "s7"},
			std::pair{x_reg::x24, "s8"},
			std::pair{x_reg::x25, "s9"},
			std::pair{x_reg::x26, "s10"},
			std::pair{x_reg::x27, "s11"},
			std::pair{x_reg::x28, "t3"},
			std::pair{x_reg::x29, "t4"},
			std::pair{x_reg::x30, "t5"},
			std::pair{x_reg::x31, "t6"}
		};

		inline std::array f_reg_name_table = {
			std::pair{f_reg::f0, "ft0"},
			std::pair{f_reg::f1, "ft1"},
			std::pair{f_reg::f2, "ft2"},
			std::pair{f_reg::f3, "ft3"},
			std::pair{f_reg::f4, "ft4"},
			std::pair{f_reg::f5, "ft5"},
			std::pair{f_reg::f6, "ft6"},
			std::pair{f_reg::f7, "ft7"},
			std::pair{f_reg::f8, "fs0"},
			std::pair{f_reg::f9, "fs1"},
			std::pair{f_reg::f10, "fa0"},
			std::pair{f_reg::f11, "fa1"},
			std::pair{f_reg::f12, "fa2"},
			std::pair{f_reg::f13, "fa3"},
			std::pair{f_reg::f14, "fa4"},
			std::pair{f_reg::f15, "fa5"},
			std::pair{f_reg::f16, "fa6"},
			std::pair{f_reg::f17, "fa7"},
			std::pair{f_reg::f18, "fs2"},
			std::pair{f_reg::f19, "fs3"},
			std::pair{f_reg::f20, "fs4"},
			std::pair{f_reg::f21, "fs5"},
			std::pair{f_reg::f22, "fs6"},
			std::pair{f_reg::f23, "fs7"},
			std::pair{f_reg::f24, "fs8"},
			std::pair{f_reg::f25, "fs9"},
			std::pair{f_reg::f26, "fs10"},
			std::pair{f_reg::f27, "fs11"},
			std::pair{f_reg::f28, "ft8"},
			std::pair{f_reg::f29, "ft9"},
			std::pair{f_reg::f30, "ft10"},
			std::pair{f_reg::f31, "ft11"}
		};
	}
}