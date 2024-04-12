// at

extern "C" {
    pub fn faccessat(
        dirfd: crate::c_int,
        pathname: *const crate::c_char,
        mode: crate::c_int,
        flags: crate::c_int,
    ) -> crate::c_int;
}

// string

extern "C" {
    pub fn memmem(
        haystack: *const crate::c_void,
        haystacklen: crate::size_t,
        needle: *const crate::c_void,
        needlelen: crate::size_t,
    ) -> *mut crate::c_void;
}
