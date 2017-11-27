/* Zupply example: argparser */
#include "zupply.hpp"
using namespace zz;

void argparser_example(int argc, char** argv)
{
	cfg::ArgParser argparser;

	// add info
	argparser.add_info("This is a demo argument parser");
	// add option for help
	argparser.add_opt_help('h', "help");
	// add version info
	//argparser.add_opt_version('v', "version", "0.1");

	struct MyOption
	{
		bool flag;
		int i;
		double d;
	};

	struct MyOption op;

	// add a flag option, when set, the target will be set to true, otherwise false
	argparser.add_opt_flag('f', "flag", "this is a flag option", &op.flag);
	// add an option to set integer i, also set to required
	argparser.add_opt_value('i', "integer", op.i, -1, "set integer value of i", "INT").require();
	// add an option to set double d
	argparser.add_opt_value('d', "double", op.d, 0.0, "set double value of d", "DOUBLE");
	
	// we could also load arguments into vector
	std::vector<double> vd;
	// no short argument,  maximum 4 arguments
	argparser.add_opt_value(-1, "vector", vd, std::vector<double>({ 0.1, 0.2 }), "load a vector").set_max(4).set_type("DVEC");
	// add place holder option
	std::string input;
	std::string output;
	auto inputArg = argparser.add_opt_value(-1, "", input, std::string("input_filename"), "input filename", "file").require();
	auto outputArg = argparser.add_opt_value(-1, "", output, std::string("output_filename"), "output filename", "file");

	// argparser support custom option with lambda function
	argparser.add_opt("custom").set_help("custom option, print msg").call([](){std::cout << "hey, called my custom stuff" << std::endl; });


	// now parse the arguments
	argparser.parse(argc, argv);

	// check errors
	if (argparser.count_error() > 0)
	{
		std::cout << argparser.get_error() << std::endl;
		// print help
		std::cout << argparser.get_help() << std::endl;
		// continue or exit()??
		//exit(-1);
	}

	// op.d, op.flag, op.i, vd stores the arguments
	std::cout << "flag: " << op.flag << std::endl;
	std::cout << "i: " << op.i << std::endl;
	std::cout << "d: " << op.d << std::endl;
	std::cout << "vector: " << std::endl;
	for (auto i = vd.begin(); i != vd.end(); ++i)
	{
		std::cout << *i << std::endl;
	}
	std::cout << "input filename: " << input << std::endl;
	std::cout << "output filename: " << output << std::endl;

}

int main(int argc, char** argv)
{
	argparser_example(argc, argv);
	return 0;
}

