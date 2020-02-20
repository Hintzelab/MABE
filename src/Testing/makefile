SHELL := /bin/bash
GTESTFLAGS := -I googletest/googletest/include -L googletest/build/googlemock/gtest -lgtest 
all: test_all

help:
	$(info valid targets are:)
	$(info ~    make: builds all test cases)
	$(info ~   clean: removes objects and exes)
	$(info ~     run: runs the test_all exe)
	$(info ~    runi: runs the test_all exe into less (w colors))

run:
	@./test_all

runi:
	@unbuffer ./test_all | less -r

clean:
	rm -rf test_all *.o

gtest:
ifeq (,$(wildcard googletest))
	git clone https://github.com/google/googletest googletest
	mkdir -p googletest/build
	cd googletest/build && cmake .. -Dgtest_disable_pthreads=ON && make -j4 gtest
endif

## Add test categories here, so we can call them separately if needed "make test_genome"
test_all: tests.o
	g++ -o test_all tests.o $(GTESTFLAGS)

## Each code file requires the " | gtest ..." prerequisite to ensure parallel (-j) builds are correct
tests.o: | gtest tests.cpp
	c++ -Wno-c++98-compat -w -Wall -std=c++11 -O3 -o tests.o -c tests.cpp $(GTESTFLAGS)
