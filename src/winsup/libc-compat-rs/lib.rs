#![allow(
    non_camel_case_types, unused_macros, unused_imports
)]

cfg_if::cfg_if! {
    if #[cfg(target_os = "windows")] {
        use core::{clone::Clone, marker::Copy};

        #[macro_use]
        mod macros;

        mod defs;
        mod missing_defs;
        mod stubs;

        pub use defs::*;
        pub use missing_defs::*;
        pub use stubs::*;
    } else {
        pub use libc::*;
    }
}
