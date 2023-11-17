all:

clean:
	rm -rf build
	rm -f flags.h
ifeq ($(OS),Windows_NT)
	git submodule foreach --recursive git config core.symlinks true
endif
	git submodule update -f --recursive
	git submodule foreach --recursive git clean -fd
# HACK: On Windows, symlinks have two different types: file and directory,
#       if the target path doesn't exist when the symlink is created,
#       Cygwin/MSYS defaults to creating a file symlink, which makes the
#       symlink unusable to pure Win32 apps (e.g. our compilers) if the type
#       doesn't match the target file/directory later, even if it exists.
#
#       Cygwin doesn't use the symlink directly like Win32 apps so it's not an
#       issue there.
#
#       Since the target path may not exist at the time of creation, we can make
#       sure the link type is correct by deleting all the symlinks after the first
#       checkout and letting Git to re-create them for us.
ifeq ($(OS),Windows_NT)
	find -type l -exec rm -f {} +
	git submodule update -f --recursive
endif
