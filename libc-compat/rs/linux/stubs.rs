// android (bionic libc) errno

extern "C" {
    #[link_name = "__errno_location"]
    pub fn __errno() -> *mut crate::c_int;
}
