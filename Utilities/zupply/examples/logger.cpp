/* Zupply example: logger */
#include "zupply.hpp"
using namespace zz;

void thread_func()
{
	int n = 0;
	auto mylogger = log::get_logger("myownlogger");
	for (int i = 0; i < 10; ++i)
	{
		n += i;
		mylogger->info("The summation: ") << n;
	}
	mylogger->debug("Thread {} finished", os::thread_id());
}

void logger_examples()
{
	// load config from file
	log::config_from_file("../../examples/logger_config_example.txt");
	// check the loggers status
	log::dump_loggers();

	auto defaultLogger = log::get_logger("default");
	defaultLogger->info("Info will not show in default logger");
	defaultLogger->error("Error will show in default logger");
	defaultLogger->fatal("Fatal will show in default logger");

	std::vector<std::thread> vt;
	for (int i = 0; i < 4; ++i)
	{
		vt.push_back(std::thread(thread_func));
	}

	for (int i = 0; i < 4; ++i)
	{
		vt[i].join();
	}
}

void config_examples()
{
	auto defaultLogger = log::get_logger("default");
	// set level mask
	defaultLogger->set_level_mask(log::level_mask_from_string("debug info warn error fatal"));
	// attach console
	defaultLogger->attach_console();
	// config stdout format
	log::get_sink("stdout")->set_format("[%datetime]{custom stuff..} %msg [end.]");
	defaultLogger->info("Now info message will be shown.");
	defaultLogger->info("And debug message.");
}

int main(int argc, char** argv)
{
	logger_examples();
	config_examples();
	return 0;
}

