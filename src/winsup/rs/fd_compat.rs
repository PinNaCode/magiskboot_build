pub use std::os::windows::io::{OwnedHandle as OwnedFd, BorrowedHandle as BorrowedFd};
pub use libc::c_int as RawFd;

use std::fs::File;
use std::os::windows::io::{RawHandle, FromRawHandle, AsHandle, AsRawHandle, IntoRawHandle};
use libc::{get_osfhandle, c_void, open_osfhandle, c_int, O_BINARY};

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

impl FromRawFd for File {
    #[inline]
    unsafe fn from_raw_fd(fd: RawFd) -> File {
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

impl AsFd for File {
    #[inline]
    fn as_fd(&self) -> BorrowedFd<'_> {
        self.as_handle()
    }
}

extern "C" {
    #[link_name = "__get_osfhandle_oflag"]
    fn get_oflag(h: RawHandle) -> c_int;
}

impl AsRawFd for File {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        let h = self.as_raw_handle();
        unsafe { open_osfhandle(h as isize, get_oflag(h) | O_BINARY) }
    }
}

impl AsRawFd for BorrowedFd<'_> {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        let h = self.as_raw_handle();
        unsafe { open_osfhandle(h as isize, get_oflag(h) | O_BINARY) }
    }
}

impl AsRawFd for OwnedFd {
    #[inline]
    fn as_raw_fd(&self) -> RawFd {
        let h = self.as_raw_handle();
        unsafe { open_osfhandle(h as isize, get_oflag(h) | O_BINARY) }
    }
}

impl IntoRawFd for File {
    #[inline]
    fn into_raw_fd(self) -> RawFd {
        let h = self.into_raw_handle();
        unsafe { open_osfhandle(h as isize, get_oflag(h) | O_BINARY) }
    }
}

impl IntoRawFd for OwnedFd {
    #[inline]
    fn into_raw_fd(self) -> RawFd {
        let h = self.into_raw_handle();
        unsafe { open_osfhandle(h as isize, get_oflag(h) | O_BINARY) }
    }
}
