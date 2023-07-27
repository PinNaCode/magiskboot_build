all:

clean:
	rm -rf build
	rm -f flags.h
	git submodule update -f --recursive
	git submodule foreach --recursive git clean -fd

clear: clean checkout
