use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    fn mf_fatal(msg: *const c_char);
}

fn main() {
    println!("Hello, world!");
    let msg = CString::new("fatal error!").unwrap();
    unsafe { mf_fatal(msg.as_ptr()) };
}