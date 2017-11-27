/* Zupply example: date and timer */
#include "zupply.hpp"
using namespace zz;

void datetime_examples()
{
	std::cout << "Testing DateTime class..." << std::endl;
	// show current date in local time zone
	time::DateTime date;
	std::cout << "Local time(Pacific): " << date.to_string() << std::endl;
	// or UTC time
	date.to_utc_time();
	std::cout << "UTC time: " << date.to_string() << std::endl;
	// another way is using static function
	std::cout << "Call UTC directly: " << zz::time::DateTime::utc_time().to_string() << std::endl;

	
	// customize to_string format
	std::string str = date.to_string("%m/%d %a Hour:%H, Min:%M, Second:%S");
	std::cout << "With format '%m/%d %a Hour:%H, Min:%M, Second:%S': " << str << std::endl;
	str = date.to_string("%c");
	std::cout << "With format %c(standard, locale dependent): " << str << std::endl;
	
}

int costy_function()
{
	long long ret = 0;
	for (int i = 0; i < 9999999; ++i)
	{
		ret += i % 100;
	}
	return ret % 1000000;
}

int costy_function2()
{
	long long ret = 0;
	for (int i = 0; i < 9999999; ++i)
	{
		ret += i % 200;
	}
	return ret % 1000000;
}

void timer_examples()
{
	std::cout << "Testing Timer class..." << std::endl;

	// create a timer before time consuming function
	time::Timer t;
	costy_function();
	std::cout << "func1 elapsed time: " << t.to_string() << std::endl;
	
	t.reset(); // reset timer, start new timer
	costy_function2(); // another function
	std::cout << t.to_string("func2 elapsed time: [%us us]") << std::endl; // use formatter

	// pause timer
	t.pause();	
	// different quantizations
	std::cout << "sec: " << t.elapsed_sec() << std::endl;
	std::cout << "msec: " << t.elapsed_ms() << std::endl;
	std::cout << "usec: " << t.elapsed_us() << std::endl;
	std::cout << "nsec: " << t.elapsed_ns() << std::endl;
	// use double, no quantize
	std::cout << "sec in double: " << t.elapsed_sec_double() << std::endl;

	// sleep for 200 ms
	t.resume();	// resume timer recording
	zz::time::sleep(500);
	std::cout << "After sleep for 0.5 sec: " << t.elapsed_sec_double() << std::endl;
}


int main(int argc, char** argv)
{
	datetime_examples();
	timer_examples();
	return 0;
}

