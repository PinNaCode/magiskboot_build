all:

clean:
	rm -rf build
	rm -f flags.h
ifeq ($(OS),Windows_NT)
	git submodule foreach --recursive git config core.symlinks true
endif
	git submodule update -f --recursive
	git submodule foreach --recursive git clean -fd
ifeq ($(OS),Windows_NT)
	find -type l -exec rm -f {} +
	git submodule update -f --recursive
endif
