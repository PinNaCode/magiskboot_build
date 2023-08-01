pub use libc::*;

// mkdir

// this shadows the original one provided by MinGW
// we add the missing mode arg here :)

extern "C" {
    #[link_name = "_mkdir_stub"]
    pub fn mkdir(path: *const crate::c_char, mode: crate::mode_t) -> crate::c_int;
}

// io

// wtf

extern "C" {
    #[link_name = "_read_stub"]
    pub fn read(fd: crate::c_int, buf: *mut crate::c_void, count: crate::size_t) -> crate::ssize_t;

    #[link_name = "_write_stub"]
    pub fn write(fd: crate::c_int, buf: *const crate::c_void, count: crate::size_t) -> crate::ssize_t;
}

// type mismatch

// changed from c_int to mode_t (c_short)
// values are not chanegd :)

pub const S_IFCHR: crate::mode_t = 8192;
pub const S_IFDIR: crate::mode_t = 16384;
pub const S_IFREG: crate::mode_t = 32768;
pub const S_IFMT: crate::mode_t = 61440;
pub const S_IEXEC: crate::mode_t = 64;
pub const S_IWRITE: crate::mode_t = 128;
pub const S_IREAD: crate::mode_t = 256;

// stat

// shadowing the MinGW one since we reimplement
// some missing features like device nodes and links

extern "C" {
    #[link_name = "_fstat_stub"]
    pub fn fstat(fildes: c_int, buf: *mut stat) -> c_int;
    #[link_name = "_stat_stub"]
    pub fn stat(path: *const c_char, buf: *mut stat) -> c_int;
}
