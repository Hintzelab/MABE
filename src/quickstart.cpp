#include "zupply.hpp"
using namespace zz;

int main(int argc, char** argv)
{
	auto logger = log::get_logger("default");
	logger->info("Welcome to zupply!");
	
	return 0;
}