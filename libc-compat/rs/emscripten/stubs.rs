// android (bionic libc) errno

extern "C" {
    #[link_name = "__errno_location"]
    pub fn __errno() -> *mut crate::c_int;
}

// syscall

pub const SYS_dup3: crate::c_int = 0;

// sendfile

extern "C" {
    #[link_name = "__sendfile_stub"]
    fn real_sendfile(
        out_fd: crate::c_int,
        in_fd: crate::c_int,
        count: crate::size_t
    );
}

f! {
    pub fn sendfile(
        out_fd: crate::c_int,
        in_fd: crate::c_int,
        offset: *mut crate::off_t,
        count: crate::size_t
    ) -> crate::ssize_t {
        if offset.is_null() {
            real_sendfile(out_fd, in_fd, count);
            0
        } else {
            panic!("unreachable code")
        }
    }
}
