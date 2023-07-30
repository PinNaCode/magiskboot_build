all:

clean:
	rm -rf build
	rm -f flags.h
ifeq ($(OS),Windows_NT)
	git submodule foreach --recursive git config core.symlinks true
endif
	git submodule update -f --recursive
	git submodule foreach --recursive git clean -fd

clear: clean checkout
