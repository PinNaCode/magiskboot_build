// android (bionic libc) errno

#[cfg(mbb_stubs___errno)]
extern "C" {
    pub fn __errno() -> *mut crate::c_int;
}
