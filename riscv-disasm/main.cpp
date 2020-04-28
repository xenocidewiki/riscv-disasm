#include "riscv.hpp"

int main(int argc, char* argv[])
{
	std::vector<uint32_t> inst_test = { 0xFCE08793 };
	riscv::instruction::object gay{ 0xFCE08793 };

	riscv::disassembler disasm {inst_test};

	disasm.parse_instructions();
	return 0;
}