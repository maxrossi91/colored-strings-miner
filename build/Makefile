SHELL = /bin/bash
CC = g++
CPPFLAGS = --std=c++11 -O3 -DNDEBUG -funroll-loops -static -Wall -Wextra -pedantic -pthread -msse4.2

# Check for SDSL library path
ifndef SDSL_LIBRARY_PATH
	$(error SDSL_LIBRARY_PATH is not set)
endif
SDSL_PATH = $(SDSL_LIBRARY_PATH)/build

THIRDPARTY_HEADERS = -I $(SDSL_PATH)/include -I $(SDSL_PATH)/external/libdivsufsort/include/
THIRDPARTY_LIBS = -L $(SDSL_PATH)/lib -L $(SDSL_PATH)/external/libdivsufsort/lib/
LINK_LIBRARY = -ldivsufsort -lsdsl -ldivsufsort64 -lsqlite3 -lpthread -ldl

all: all_miner all_baseline
all_miner: test-miner-random-gen test-miner-random-gen-large-alphabet test-real-data-mangrove
all_baseline: test-baseline-random-gen test-baseline-random-gen-large-alphabet test-baseline-real-data-mangrove test-baseline-all-colors-random-gen test-baseline-all-real-data-mangrove
all_random: test-miner-random-gen test-miner-random-gen-large-alphabet test-baseline-random-gen test-baseline-random-gen-large-alphabet test-baseline-all-colors-random-gen
all_real: test-real-data-mangrove test-baseline-real-data-mangrove test-baseline-all-real-data-mangrove

miner.o : ../src/miner/miner.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -c ../src/miner/miner.cc $(LINK_LIBRARY)

baseline.o : ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -c ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-miner-random-gen: ../src/miner/miner.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-miner-random-gen ../src/tests/test-miner-random-gen.cc ../src/miner/miner.cc $(LINK_LIBRARY)

test-baseline-random-gen: ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-baseline-random-gen ../src/tests/test-baseline-random-gen.cc ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-miner-random-gen-large-alphabet: ../src/miner/miner.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-miner-random-gen-large-alphabet ../src/tests/test-miner-random-gen-large-alphabet.cc ../src/miner/miner.cc $(LINK_LIBRARY)

test-baseline-random-gen-large-alphabet: ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-baseline-random-gen-large-alphabet ../src/tests/test-baseline-random-gen-large-alphabet.cc ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-real-data-mangrove: ../src/miner/miner.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-real-data-mangrove ../src/tests/test-real-data-mangrove.cc ../src/miner/miner.cc $(LINK_LIBRARY)

test-baseline-real-data-mangrove: ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-baseline-real-data-mangrove ../src/tests/test-baseline-real-data-mangrove.cc ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-baseline-all-colors-random-gen: ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-baseline-all-colors-random-gen ../src/tests/test-baseline-all-colors-random-gen.cc ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-baseline-all-real-data-mangrove: ../src/baseline/baseline.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-baseline-all-real-data-mangrove ../src/tests/test-baseline-all-real-data-mangrove.cc ../src/baseline/baseline.cc $(LINK_LIBRARY)

test-paper-example: ../src/miner/miner.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(THIRDPARTY_HEADERS) $(THIRDPARTY_LIBS) -o test-paper-example ../src/tests/test-paper-example.cc ../src/miner/miner.cc $(LINK_LIBRARY)


clean:
	/bin/rm -f mine test-miner-random-gen test-baseline-random-gen test-miner-random-gen-large-alphabet test-baseline-random-gen-large-alphabet test-real-data-mangrove test-baseline-real-data-mangrove test-baseline-all-real-data-mangrove test-baseline-all-colors-random-gen test-paper-example *.o
