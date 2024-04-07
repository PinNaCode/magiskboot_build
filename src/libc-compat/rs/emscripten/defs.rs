pub use libc::*;

// shadowing these functions to use our stubs
// check src/libc-compat/emsup/mmap_hack.c for details

extern "C" {
    #[link_name = "_mmap_stub_impl"]
    pub fn mmap(
        addr: *mut c_void,
        len: size_t,
        prot: c_int,
        flags: c_int,
        fd: c_int,
        offset: off_t,
    ) -> *mut c_void;

    #[link_name = "_munmap_stub_impl"]
    pub fn munmap(addr: *mut c_void, len: size_t) -> c_int;

    // close() is shadowed using a --wrap ldflag
}
