#include "riscv.hpp"

int main(int argc, char* argv[])
{
	std::vector<uint32_t> inst_test = { 0x00850463, 0x00002e17, 0xee1ff0ef, 0x00E12423, 0x4027d79b, 0x40f707bb, 0x0CF2030F };

	riscv::disassembler disasm {inst_test}; 

	disasm.parse_instructions();
	return 0;
}