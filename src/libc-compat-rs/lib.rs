#![allow(
    non_camel_case_types, unused_macros, unused_imports, non_upper_case_globals
)]

#[macro_use]
mod macros;

cfg_if::cfg_if! {
    if #[cfg(target_os = "linux")] {
        mod linux;
        pub use linux::*;
    } else if #[cfg(target_vendor = "apple")] {
        mod darwin;
        pub use darwin::*;
    } else if #[cfg(target_os = "cygwin")] {
        mod cygwin;
        pub use cygwin::*;
    } else if #[cfg(target_os = "windows")] {
        mod windows;
        pub use windows::*;
    } else {
        pub use libc::*;
    }
}
