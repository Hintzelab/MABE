#include "zupply.hpp"
using namespace zz;

int main()
{
	std::cout << "Testing progbar..." << std::endl;
	unsigned task = 1000;
	log::ProgBar pb(task, "Test");
	for (unsigned i = 0; i < task; )
	{
		//std::cout << i << " ";
		int step = 1;
		pb.step(step);
		i += step;
		time::sleep(1);
	}
}