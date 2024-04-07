// defined in libc-compat/non-linux/sendfile_fallback.c

macro_rules! add_sendfile_stubs_if_needed {
    () => {
        cfg_if::cfg_if! {
            if #[cfg(mbb_stubs_sendfile)] {
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
            }
        }
    };
}
