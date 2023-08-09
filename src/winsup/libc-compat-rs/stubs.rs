// dirent

// there is an additional d_type field
// compared to the original MinGW dirent

s_no_extra_traits! {
    pub struct dirent {
        pub d_ino: crate::c_long,
        pub d_type: crate::c_uchar,
        pub d_reclen: crate::c_ushort,
        pub d_namlen: crate::c_ushort,
        pub d_name: [crate::c_char; 260],
    }
}

pub const DT_REG: u8 = 1;
pub const DT_LNK: u8 = 2;
pub const DT_DIR: u8 = 3;

// replace the MinGW impls bcs we've modified dirent struct and added dir fd support

pub enum DIR {}
impl crate::Copy for DIR {}
impl crate::Clone for DIR {
    fn clone(&self) -> DIR {
        *self
    }
}

extern "C" {
    #[link_name = "_opendir_stub"]
    pub fn opendir(dirname: *const crate::c_char) -> *mut DIR;

    #[link_name = "_readdir_stub"]
    pub fn readdir(dirp: *mut DIR) -> *mut crate::dirent;

    #[link_name = "_rewinddir_stub"]
    pub fn rewinddir(dirp: *mut DIR);

    #[link_name = "_closedir_stub"]
    pub fn closedir(dirp: *mut DIR) -> crate::c_int;
}

extern "C" {
    pub fn fdopendir(fd: crate::c_int) -> *mut DIR;
    pub fn dirfd(dirp: *mut DIR) -> crate::c_int;
}

// no-op

pub const O_CLOEXEC: crate::c_int = 0;

// link

extern "C" {
    pub fn readlink(path: *const crate::c_char, buf: *mut crate::c_char, bufsz: crate::size_t) -> crate::ssize_t;
    pub fn readlinkat(dirfd: crate::c_int,
        pathname: *const crate::c_char,
        buf: *mut crate::c_char,
        bufsiz: crate::size_t) -> crate::ssize_t;
    pub fn symlink(path1: *const crate::c_char, path2: *const crate::c_char) -> crate::c_int;
    pub fn symlinkat(
        target: *const crate::c_char,
        newdirfd: crate::c_int,
        linkpath: *const crate::c_char,
    ) -> crate::c_int;
    pub fn linkat(
        olddirfd: crate::c_int,
        oldpath: *const crate::c_char,
        newdirfd: crate::c_int,
        newpath: *const crate::c_char,
        flags: crate::c_int,
    ) -> crate::c_int;
}

// at

pub const AT_REMOVEDIR: crate::c_int = 1;

extern "C" {
    pub fn unlinkat(dirfd: crate::c_int, pathname: *const crate::c_char, flags: crate::c_int) -> crate::c_int;
    pub fn openat(dirfd: crate::c_int, pathname: *const crate::c_char,
                          flags: crate::c_int, ...) -> crate::c_int;
    pub fn faccessat(
        dirfd: crate::c_int,
        pathname: *const crate::c_char,
        mode: crate::c_int,
        flags: crate::c_int,
    ) -> crate::c_int;
    pub fn fstatat(
        dirfd: crate::c_int,
        pathname: *const crate::c_char,
        buf: *mut crate::stat,
        flags: crate::c_int,
    ) -> crate::c_int;
    pub fn mkdirat(dirfd: crate::c_int, pathname: *const crate::c_char,
                          mode: crate::mode_t) -> crate::c_int;
}

// mman

extern "C" {
    #[link_name = "mmap"]
    pub fn __mmap_impl(
        addr: *mut crate::c_void,
        len: crate::size_t,
        prot: crate::c_int,
        flags: crate::c_int,
        fd: crate::c_int,
        offset: crate::c_longlong,
    ) -> *mut crate::c_void;
    pub fn munmap(addr: *mut crate::c_void, len: crate::size_t) -> crate::c_int;
}

pub fn mmap(
    addr: *mut crate::c_void,
    len: crate::size_t,
    prot: crate::c_int,
    flags: crate::c_int,
    fd: crate::c_int,
    offset: crate::off_t,
) -> *mut crate::c_void {
    unsafe { __mmap_impl(addr, len, prot, flags, fd, offset as crate::c_longlong) }
}

pub const MAP_SHARED: crate::c_int = 1;
pub const MAP_PRIVATE: crate::c_int = 2;
pub const MAP_FAILED: *mut crate::c_void = !0 as *mut crate::c_void;
pub const PROT_READ: crate::c_int = 1;
pub const PROT_WRITE: crate::c_int = 2;

// mknod

extern "C" {
    pub fn mknod(pathname: *const crate::c_char, mode: crate::mode_t, dev: crate::dev_t) -> crate::c_int;
}

f! {
    pub fn major(dev: crate::dev_t) -> crate::c_int {
        ((dev >> 16) & 0xffff) as crate::c_int
    }

    pub fn minor(dev: crate::dev_t) -> crate::c_int {
        (dev & 0xffff) as crate::c_int
    }
}

safe_f! {
    pub {const} fn makedev(ma: crate::c_int, mi: crate::c_int) -> crate::dev_t {
        let ma = ma as crate::dev_t;
        let mi = mi as crate::dev_t;
        (ma << 16) | (mi & 0xffff)
    }
}

// stat

pub const S_IFBLK: crate::mode_t = 24576;
pub const S_IFLNK: crate::mode_t = 40960;

extern "C" {
    pub fn lstat(path: *const crate::c_char, buf: *mut crate::stat) -> crate::c_int;
}

pub const S_IRGRP: crate::mode_t = 32;
pub const S_IROTH: crate::mode_t = 4;
pub const S_IRUSR: crate::mode_t = 256;
pub const S_IWOTH: crate::mode_t = 2;
pub const S_IWGRP: crate::mode_t = 16;
pub const S_IWUSR: crate::mode_t = 128;
pub const S_IXGRP: crate::mode_t = 8;
pub const S_IXOTH: crate::mode_t = 1;
pub const S_IXUSR: crate::mode_t = 64;
