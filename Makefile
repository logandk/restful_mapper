.PHONY: all test vendor

all:
	cd build; cmake -DCMAKE_INSTALL_PREFIX=$(CURDIR) ..; make all install

test: all
	cd tests/build; cmake ..; make; ./tests

vendor:
	cd vendor; cmake .; make
