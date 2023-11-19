macro_rules! __item {
    ($i:item) => {
        $i
    };
}

macro_rules! s {
    ($($(#[$attr:meta])* pub $t:ident $i:ident { $($field:tt)* })*) => ($(
        s!(it: $(#[$attr])* pub $t $i { $($field)* });
    )*);
    (it: $(#[$attr:meta])* pub union $i:ident { $($field:tt)* }) => (
        compile_error!("unions cannot derive extra traits, use s_no_extra_traits instead");
    );
    (it: $(#[$attr:meta])* pub struct $i:ident { $($field:tt)* }) => (
        __item! {
            #[repr(C)]
            #[cfg_attr(feature = "extra_traits", derive(Debug, Eq, Hash, PartialEq))]
            #[allow(deprecated)]
            $(#[$attr])*
            pub struct $i { $($field)* }
        }
        #[allow(deprecated)]
        impl crate::Copy for $i {}
        #[allow(deprecated)]
        impl crate::Clone for $i {
            fn clone(&self) -> $i { *self }
        }
    );
}

macro_rules! s_no_extra_traits {
    ($($(#[$attr:meta])* pub $t:ident $i:ident { $($field:tt)* })*) => ($(
        s_no_extra_traits!(it: $(#[$attr])* pub $t $i { $($field)* });
    )*);
    (it: $(#[$attr:meta])* pub struct $i:ident { $($field:tt)* }) => (
        __item! {
            #[repr(C)]
            $(#[$attr])*
            pub struct $i { $($field)* }
        }
        #[allow(deprecated)]
        impl crate::Copy for $i {}
        #[allow(deprecated)]
        impl crate::Clone for $i {
            fn clone(&self) -> $i { *self }
        }
    );
}

macro_rules! f {
    ($($(#[$attr:meta])* pub $({$constness:ident})* fn $i:ident(
                $($arg:ident: $argty:ty),*
    ) -> $ret:ty {
        $($body:stmt);*
    })*) => ($(
        #[inline]
        $(#[$attr])*
        pub unsafe extern fn $i($($arg: $argty),*
        ) -> $ret {
            $($body);*
        }
    )*)
}

macro_rules! safe_f {
    ($($(#[$attr:meta])* pub $({$constness:ident})* fn $i:ident(
                $($arg:ident: $argty:ty),*
    ) -> $ret:ty {
        $($body:stmt);*
    })*) => ($(
        #[inline]
        $(#[$attr])*
        pub extern fn $i($($arg: $argty),*
        ) -> $ret {
            $($body);*
        }
    )*)
}
