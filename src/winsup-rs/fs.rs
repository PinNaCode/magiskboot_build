use std::ffi::CString;
use std::{path::Path, io};
use crate::cvt::cvt;
use libc::mode_t;

// DirBuilder using POSIX mkdir with mode support

#[derive(Debug)]
pub struct DirBuilder {
    mode: mode_t,
    recursive: bool,
}

impl DirBuilder {
    pub fn new() -> DirBuilder {
        DirBuilder { mode: 0o777, recursive: false }
    }

    fn mkdir(&self, p: &Path) -> io::Result<()> {
        let s = match p.to_str() {
            Some(x) => x,
            None => return Err(io::Error::last_os_error())
        };
        let cs = match CString::new(s) {
            Ok(c) => c,
            Err(_) => return Err(io::Error::last_os_error())
        };

        cvt(unsafe { libc::mkdir(cs.as_ptr(), self.mode) })?;
        Ok(())
    }

    pub fn recursive(&mut self, recursive: bool) -> &mut Self {
        self.recursive = recursive;
        self
    }

    pub fn set_mode(&mut self, mode: u32) {
        self.mode = mode as mode_t;
    }

    pub fn create<P: AsRef<Path>>(&self, path: P) -> io::Result<()> {
        self._create(path.as_ref())
    }

    fn _create(&self, path: &Path) -> io::Result<()> {
        if self.recursive { self.create_dir_all(path) } else { self.mkdir(path) }
    }

    fn create_dir_all(&self, path: &Path) -> io::Result<()> {
        if path == Path::new("") {
            return Ok(());
        }

        match self.mkdir(path) {
            Ok(()) => return Ok(()),
            Err(ref e) if e.kind() == io::ErrorKind::NotFound => {}
            Err(_) if path.is_dir() => return Ok(()),
            Err(e) => return Err(e)
        }
        match path.parent() {
            Some(p) => self.create_dir_all(p)?,
            None => {
                return Err(io::Error::new(
                    io::ErrorKind::Uncategorized,
                    "failed to create whole tree"
                ));
            }
        }
        match self.mkdir(path) {
            Ok(()) => Ok(()),
            Err(_) if path.is_dir() => Ok(()),
            Err(e) => Err(e)
        }
    }
}

pub trait DirBuilderExt {
    fn mode(&mut self, mode: u32) -> &mut Self;
}

impl DirBuilderExt for DirBuilder {
    fn mode(&mut self, mode: u32) -> &mut DirBuilder {
        self.set_mode(mode);
        self
    }
}

// the missing symlink function

pub fn symlink<P: AsRef<Path>, Q: AsRef<Path>>(original: P, link: Q) -> io::Result<()> {
    let p = match original.as_ref().to_str() {
        Some(x) => x,
        None => return Err(io::Error::last_os_error())
    };
    let q = match link.as_ref().to_str() {
        Some(x) => x,
        None => return Err(io::Error::last_os_error())
    };
    let ps = match CString::new(p) {
        Ok(c) => c,
        Err(_) => return Err(io::Error::last_os_error())
    };
    let qs = match CString::new(q) {
        Ok(c) => c,
        Err(_) => return Err(io::Error::last_os_error())
    };
    cvt(unsafe { libc::symlink(ps.as_ptr(), qs.as_ptr()) })?;
    Ok(())
}
