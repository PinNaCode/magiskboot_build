// at

extern "C" {
    pub fn faccessat(
        dirfd: crate::c_int,
        pathname: *const crate::c_char,
        mode: crate::c_int,
        flags: crate::c_int,
    ) -> crate::c_int;
}
