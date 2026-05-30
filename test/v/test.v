/*
 * test.v
 * ------
 *
 * This file is a test program that shows how to use the mini_fatal single-header
 * C library from the V programming language.
 *
 * mini_fatal is written as a single-header C library. That means the public
 * declarations and the implementation both live in the same header file:
 *
 *     mini_fatal.h
 *
 * The declarations tell V which C functions exist, what their names are, and what
 * arguments they take. This allows the V code to call functions such as mf_fatal,
 * mf_fatal_at_impl, and mf_warning_at_impl.
 *
 * However, declarations are not the same thing as implementations.
 *
 * A declaration only says:
 *
 *     "this function exists somewhere"
 *
 * The implementation is the actual C code that defines what the function does.
 *
 * For a single-header C library, the implementation is usually enabled by defining
 * a special macro before including the header in exactly one C source file. For
 * mini_fatal, that macro is:
 *
 *     MINI_FATAL_IMPLEMENTATION
 *
 * This is why this V test also needs a small .c file. That .c file defines
 * MINI_FATAL_IMPLEMENTATION and then includes mini_fatal.h. This causes the real
 * function bodies to be compiled into the test program.
 *
 * Without that .c file, V would know the names and signatures of the C functions,
 * but the linker would not be able to find their compiled implementations. In other
 * words, the V code could compile far enough to understand the function calls, but
 * the final build would fail when trying to link the program.
 *
 * The v.mod file in this directory is also important. It marks this folder as a V
 * module root. Because of that, test.v can use @VMODROOT in #flag and #include
 * paths to refer to files relative to this V test module. This makes the paths more
 * stable and easier to understand than relying on the current working directory.
 *
 * So this test has three important parts:
 *
 *   1. The V file declares and calls the C functions.
 *   2. The helper .c file provides the actual mini_fatal implementation.
 *   3. The v.mod file gives V a module root, which allows @VMODROOT to work.
 *
 * Together, these files verify that mini_fatal can be used from V through C
 * interop.
 */

import os

#flag @VMODROOT/v_test_helper.c
#include "@VMODROOT/../../mini_fatal.h"

fn C.mf_fatal(msg &char)
fn C.mf_version()
fn C.mf_fatal_at_impl(msg &char, file &char, line int)
fn C.mf_warning_at_impl(msg &char, file &char, line int, func &char)

fn mf_fatal(msg string) {
	C.mf_fatal(msg.str)
}

fn mf_version() {
	C.mf_version()
}

fn mf_fatal_at(msg string, file string, line int) {
	C.mf_fatal_at_impl(msg.str, file.str, line)
}

fn mf_warning_at(msg string, file string, line int, func string) {
	C.mf_warning_at_impl(msg.str, file.str, line, func.str)
}

struct C.mf_context_item {
	msg &char
	file &char
	line int
	col int
	func &char
	thread_id int
	pid int
}

struct C.mf_context {
	data &C.mf_context_item
	size usize
	capacity usize
}

fn C.mf_create_context(cap usize) C.mf_context
fn C.mf_context_push(ctx &C.mf_context, item C.mf_context_item)
fn C.mf_context_dump(ctx &C.mf_context)
fn C.mf_context_destroy(ctx &C.mf_context)

struct C.mf_event {
	session_id usize
	msg &char
	file &char
	line int
	func &char
	pid int
	threadid usize
	usr_json &char
	timestamp usize
	char full[1024]
}

fn main() {
	println("Testing mini_fatal from v!")
	ctx := C.mf_create_context(10)
	C.mf_context_push(&ctx, C.mf_context_item{"Testing context!".str, @FILE.str, @LINE.int(), @COLUMN.int(), @FN.str, 0, os.getpid()})
	C.mf_context_dump(&ctx)
	C.mf_context_destroy(&ctx)
	mf_warning_at("About to fatal abort!", @FILE, @LINE.int(), @FN)
	mf_fatal_at("Fatal error HERE!", @FILE, @LINE.int())
}

