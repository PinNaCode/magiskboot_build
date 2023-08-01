use std::fs::{FileType as HostFileType, Metadata as HostMetadata};
use std::os::windows::fs::MetadataExt as HostMetadataExt;
use std::{path::Path, io, mem};
use cvt::cvt;
use libc::mode_t;

// stubs for the MinGW stuffs

pub trait IsBlockDevice {
    fn is_block_device(&self) -> bool;
}

impl IsBlockDevice for HostFileType {
    fn is_block_device(&self) -> bool {
        false
    }
}

pub trait StatSize {
    fn st_size(&self) -> u64;
}

impl StatSize for HostMetadata {
    fn st_size(&self) -> u64 {
        self.file_size()
    }
}

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
            Some(x) => x.as_ptr() as *mut i8,
            None => return Err(io::Error::last_os_error())
        };
        cvt(unsafe { libc::mkdir(s, self.mode) })?;
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
        Some(x) => x.as_ptr() as *mut i8,
        None => return Err(io::Error::last_os_error())
    };
    let q = match link.as_ref().to_str() {
        Some(x) => x.as_ptr() as *mut i8,
        None => return Err(io::Error::last_os_error())
    };
    cvt(unsafe { libc::symlink(p, q) })?;
    Ok(())
}

// stat

#[derive(Copy, Clone, Eq, Debug)]
pub struct FileType {
    mode: mode_t,
}

impl PartialEq for FileType {
    fn eq(&self, other: &Self) -> bool {
        self.masked() == other.masked()
    }
}

impl core::hash::Hash for FileType {
    fn hash<H: core::hash::Hasher>(&self, state: &mut H) {
        self.masked().hash(state);
    }
}

impl FileType {
    pub fn is_dir(&self) -> bool {
        self.is(libc::S_IFDIR)
    }
    pub fn is_file(&self) -> bool {
        self.is(libc::S_IFREG)
    }
    pub fn is_symlink(&self) -> bool {
        self.is(libc::S_IFLNK)
    }

    pub fn is(&self, mode: mode_t) -> bool {
        self.masked() == mode
    }

    fn masked(&self) -> mode_t {
        self.mode & libc::S_IFMT
    }
}

pub trait FileTypeExt {
    fn is_block_device(&self) -> bool;
    fn is_char_device(&self) -> bool;
}

impl FileTypeExt for FileType {
    fn is_block_device(&self) -> bool {
        self.is(libc::S_IFBLK)
    }
    fn is_char_device(&self) -> bool {
        self.is(libc::S_IFCHR)
    }
}

#[derive(Clone)]
pub struct FileAttr {
    stat: libc::stat
}

impl FileAttr {
    fn from_stat(stat: libc::stat) -> Self {
        Self { stat }
    }

    pub fn file_type(&self) -> FileType {
        FileType { mode: self.stat.st_mode as mode_t }
    }
}

#[derive(Clone)]
pub struct Metadata(FileAttr);

impl Metadata {
    pub fn file_type(&self) -> FileType {
        self.0.file_type()
    }

    pub fn is_file(&self) -> bool {
        self.file_type().is_file()
    }
}

pub trait MetadataExt {
    fn st_rdev(&self) -> u64;
    fn rdev(&self) -> u64;
}

impl MetadataExt for Metadata {
    fn st_rdev(&self) -> u64 {
        self.0.stat.st_rdev as u64
    }

    fn rdev(&self) -> u64 {
        self.st_rdev()
    }
}

pub fn stat(p: &Path) -> io::Result<FileAttr> {
    let s = match p.to_str() {
        Some(x) => x.as_ptr() as *mut i8,
        None => return Err(io::Error::last_os_error())
    };

    let mut stat: libc::stat = unsafe { mem::zeroed() };
    cvt(unsafe { libc::stat(s, &mut stat) })?;
    Ok(FileAttr::from_stat(stat))
}

pub fn metadata<P: AsRef<Path>>(path: P) -> io::Result<Metadata> {
    stat(path.as_ref()).map(Metadata)
}
