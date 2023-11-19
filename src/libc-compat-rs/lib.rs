#![allow(
    non_camel_case_types, unused_macros, unused_imports
)]

cfg_if::cfg_if! {
    if #[cfg(target_os = "windows")] {
        mod windows;
        pub use windows::*;
    } else {
        pub use libc::*;
    }
}
