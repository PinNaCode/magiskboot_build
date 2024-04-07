all:

clean:
	rm -rf build
ifneq ($(wildcard .git),)
	rm -f include/flags.h
ifeq ($(OS),Windows_NT)
	git submodule foreach --recursive git config core.symlinks true
endif  # Windows_NT
	git submodule update -f --recursive
	git submodule foreach --recursive git clean -fd
# HACK: On Windows, symlinks have two different types: file and directory,
#       if the target path doesn't exist when the symlink is created,
#       Cygwin will fallback to create a non-native link, which makes the
#       symlink unusable to pure Win32 apps (e.g. our compilers) later.
#
#       Since the target path may not exist at the time of creation, we can fix
#       this by deleting all the symlinks after the first checkout and
#       letting Git to re-create them for us.
ifeq ($(OS),Windows_NT)
	find -type l -exec rm -f {} +
	git submodule update -f --recursive
endif  # Windows_NT
endif  # wildcard .git
