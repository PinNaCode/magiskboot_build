#![allow(
    non_camel_case_types, unused_macros, unused_imports, non_upper_case_globals
)]

#[macro_use]
mod macros;

cfg_if::cfg_if! {
    if #[cfg(target_os = "windows")] {
        // windows has some missing defs,
        // we also need to shadow add some extra stuffs

        mod windows;
        pub use windows::*;
    } else if #[cfg(target_os = "emscripten")] {
        // emscripten has some missing defs
        // and we want to shadow some mman funcs

        mod emscripten;
        pub use emscripten::*;
    } else {
        pub use libc::*;
    }
}

#[macro_use]
mod common;

post_defs_overrides!();

pub use common::*;
