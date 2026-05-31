extern fn mf_fatal(msg: [*:0]const u8) void;

fn f1() void { mf_fatal("Aborting Zig!"); }
fn f2() void { f1(); }
fn f3() void { f2(); }
fn f4() void { f3(); }
fn f5() void { f4(); }
fn f6() void { f5(); }
fn f7() void { f6(); }
fn f8() void { f7(); }
fn f9() void { f8(); }
fn f10() void { f9(); }

pub fn main() void {
    f10();
}