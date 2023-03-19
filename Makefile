all:

clean:
	rm -rf build
	rm -f flags.h

checkout:
	git submodule update -f

clear: clean checkout
