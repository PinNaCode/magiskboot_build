pub use libc::*;

// shadowing sendfile to use our stub

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
