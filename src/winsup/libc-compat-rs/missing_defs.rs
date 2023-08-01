// stat

pub type mode_t = crate::c_ushort;
pub type gid_t = crate::c_short;
pub type uid_t = crate::c_short;

// dirent
pub enum DIR {}
impl crate::Copy for DIR {}
impl crate::Clone for DIR {
    fn clone(&self) -> DIR {
        *self
    }
}

extern "C" {
    pub fn opendir(dirname: *const crate::c_char) -> *mut DIR;
    pub fn rewinddir(dirp: *mut DIR);
    pub fn closedir(dirp: *mut DIR) -> crate::c_int;
}
