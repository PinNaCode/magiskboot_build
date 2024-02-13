// android (bionic libc) errno

#[cfg(any(target_os = "linux", target_os = "cygwin",
          target_os = "emscripten"))]
extern "C" {
    #[link_name = "__errno_location"]
    pub fn __errno() -> *mut crate::c_int;
}

#[cfg(target_vendor = "apple")]
extern "C" {
    #[link_name = "__error"]
    pub fn __errno() -> *mut crate::c_int;
}

#[cfg(target_os = "windows")]
extern "cdecl" {
    #[link_name = "_errno"]
    pub fn __errno() -> *mut crate::c_int;
}
