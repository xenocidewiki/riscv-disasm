#include "riscv.hpp"

int main(int argc, char* argv[])
{
	uint32_t inst_test = 0xFCE08793;

	riscv::instruction::object yeet(0xFCE08793);
	riscv::instruction::object yeet2(inst_test);

	return 0;
}