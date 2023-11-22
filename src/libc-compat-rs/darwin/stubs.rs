// android (bionic libc) errno

extern "C" {
    #[link_name = "__error"]
    pub fn __errno() -> *mut crate::c_int;
}

// fcntl

pub const O_PATH: crate::c_int = 0;  // no-op

// long variants

extern "C" {
    #[link_name = "lseek"]
    pub fn lseek64(fd: crate::c_int, offset: crate::off_t, whence: crate::c_int) -> crate::off_t;
}

// syscall

pub const SYS_dup3: crate::c_int = 0;
