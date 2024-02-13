// just aliases

extern "C" {
    #[link_name = "lseek"]
    pub fn lseek64(fd: crate::c_int, offset: crate::off_t, whence: crate::c_int) -> crate::off_t;
}
