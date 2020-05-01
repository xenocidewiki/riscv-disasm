#	Copyright(C) 2020 xenocidewiki
#	This file is part of riscv-disasm.
#
#	riscv-disasm is free software : you can redistribute it and /or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	riscv-disasm is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with riscv-disasm. If not, see <https://www.gnu.org/licenses/>.
file = open("riscvopcodes.txt", "r")

lines = file.readlines()

mnemonic = []
match = []
mask = []
i = 0
for line in lines:
    split = line.split(" ")
    mnemonic_name = split[1].split("_")
     
    if i % 2 == 0:
        if len(mnemonic_name) == 4:
            mnemonic.append(mnemonic_name[1] + "." + mnemonic_name[2] + "." + mnemonic_name[3])
        elif len(mnemonic_name) == 3:
            mnemonic.append(mnemonic_name[1] + "." + mnemonic_name[2])
        else:
            mnemonic.append(mnemonic_name[1])

        match.append(split[2].strip())
    else:
        mask.append(split[3].strip())

    i += 1

old_opcode = 0
for i in range(len(mnemonic)):
    opcode = hex(int(match[i], 0) & 0x7F)

    if opcode == old_opcode:
        print(", { " + match[i] + ", " + mask[i] + ", \"" + mnemonic[i] + "\" }", end='')
    else:
        print(" } },")
        old_opcode = opcode
        print("{ " + opcode + ", { {" + match[i] + ", " + mask[i] + ", \"" + mnemonic[i] + "\" }", end='')

