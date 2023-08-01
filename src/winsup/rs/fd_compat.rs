pub use std::os::windows::io::{RawHandle, OwnedHandle as OwnedFd, BorrowedHandle as BorrowedFd};
pub use libc::c_int as RawFd;
pub use std::os::windows::io::{FromRawHandle, AsHandle, AsRawHandle, IntoRawHandle};

use std::fs;
use libc::{get_osfhandle, c_void, open_osfhandle};

pub trait FromRawFd {
    unsafe fn from_raw_fd(fd: RawFd) -> Self;
}

pub trait AsFd {
    fn as_fd(&self) -> BorrowedFd<'_>;
}

pub trait AsRawFd {
    fn as_raw_fd(&self) -> RawFd;
}

pub trait IntoRawFd {
    fn into_raw_fd(self) -> RawFd;
}

impl FromRawFd for fs::File {
    #[inline]
    unsafe fn from_raw_fd(fd: RawFd) -> fs::File {
        Self::from_raw_handle(get_osfhandle(fd) as *mut c_void)
    }
}

impl FromRawFd for OwnedFd {
    #[inline]
    unsafe fn from_raw_fd(fd: RawFd) -> Self {
        Self::from_raw_handle(get_osfhandle(fd) as *mut c_void)
    }
}

impl<T: AsFd + AsHandle> AsFd for &T {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl<T: AsFd + AsHandle> AsFd for &mut T {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl<T: AsFd + AsHandle> AsFd for Box<T> {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl AsFd for BorrowedFd<'_> {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl AsFd for OwnedFd {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl AsFd for fs::File {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

impl AsRawFd for fs::File {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        unsafe { open_osfhandle(self.as_raw_handle() as isize, 0) }
    }
}

impl AsRawFd for BorrowedFd<'_> {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        unsafe { open_osfhandle(self.as_raw_handle() as isize, 0) }
    }
}

impl AsRawFd for OwnedFd {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        unsafe { open_osfhandle(self.as_raw_handle() as isize, 0) }
    }
}

impl IntoRawFd for fs::File {
    #[inline]
    fn into_raw_fd(self) -> RawFd {
        unsafe { open_osfhandle(self.into_raw_handle() as isize, 0) }
    }
}

impl IntoRawFd for OwnedFd {
    #[inline]
    fn into_raw_fd(self) -> RawFd {
        unsafe { open_osfhandle(self.into_raw_handle() as isize, 0) }
    }
}
