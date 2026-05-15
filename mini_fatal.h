/*
 * mini_fatal.h
 * ------------
 *
 * A small, single-header C/C++ utility for reporting unrecoverable program
 * failures in a consistent and developer-friendly way.
 *
 * mini_fatal is intended for situations where continuing execution would be
 * unsafe, misleading, or impossible. It provides a minimal fatal-error API that
 * prints a clear diagnostic message, optionally emits a stack trace, and then
 * terminates the process.
 *
 * This library is designed to be:
 *
 *   - Simple:
 *       Include one header and call the fatal-reporting function when an
 *       unrecoverable error is encountered.
 *
 *   - Lightweight:
 *       The public interface is intentionally small, with minimal dependencies
 *       and no required runtime setup for normal use.
 *
 *   - C and C++ compatible:
 *       The public functions are exposed with C linkage when included from C++,
 *       making the header usable from both C and C++ projects.
 *
 *   - Single-header friendly:
 *       The declaration and implementation live in the same file. Define
 *       MINI_FATAL_IMPLEMENTATION in exactly one translation unit before
 *       including this header to emit the implementation, if using the
 *       single-header implementation pattern.
 *
 *   - Configurable:
 *       Optional functionality, such as stack trace dumping, can be disabled
 *       at compile time with feature macros.
 *
 *
 * Core functionality
 * ------------------
 *
 *   mf_fatal(const char *msg)
 *
 *       Reports a fatal error message to standard error, attempts to provide
 *       additional debugging information when enabled, and aborts the program.
 *
 *       This function should be used for unrecoverable failures such as:
 *
 *         - violated internal invariants
 *         - impossible control-flow paths
 *         - failed assumptions that indicate a programming error
 *         - critical initialization failures
 *         - corrupted program state
 *
 *
 * Optional stack traces
 * ---------------------
 *
 *   mf_dump_stacktrace()
 *
 *       Emits a best-effort stack trace for the current thread when supported
 *       by the target platform.
 *
 *       Stack trace support can be disabled by defining:
 *
 *           MF_NO_STACKTRACE
 *
 *       before including this header. When disabled, stack trace dumping becomes
 *       a no-op through the DUMP_STACKTRACE() macro.
 *
 *
 * Configuration macros
 * --------------------
 *
 *   MINI_FATAL_IMPLEMENTATION
 *
 *       Define this macro in one source file before including mini_fatal.h to
 *       include the implementation in that translation unit.
 *
 *       Example:
 *
 *           #define MINI_FATAL_IMPLEMENTATION
 *           #include "mini_fatal.h"
 *
 *   MF_NO_STACKTRACE
 *
 *       Define this macro to remove stack trace support. This is useful for
 *       platforms where stack tracing is unavailable, undesirable, or too
 *       expensive to include.
 *
 *
 * Extension guidelines
 * --------------------
 *
 * This header is intended to grow incrementally as additional fatal-error and
 * diagnostics utilities are added. When adding new functionality, prefer the
 * following conventions:
 *
 *   - Prefix public symbols with `mf_` to avoid name collisions.
 *   - Keep optional features guarded by feature macros.
 *   - Preserve C compatibility unless a feature is explicitly C++-only.
 *   - Keep the public API near the top of the file.
 *   - Keep platform-specific implementation details isolated behind platform
 *     detection macros.
 *   - Document new public functions in this comment block under a dedicated
 *     section.
 *
 *
 * Example usage
 * -------------
 *
 *   #define MINI_FATAL_IMPLEMENTATION
 *   #include "mini_fatal.h"
 *
 *   int main(void) {
 *       mf_fatal("something went wrong");
 *       return 0;
 *   }
 *
 *
 * Notes
 * -----
 *
 * mini_fatal is not intended to replace structured error handling, return-code
 * checking, exceptions, logging frameworks, or recovery paths. It is intended
 * specifically for cases where the only correct response is to stop execution
 * immediately and provide useful diagnostic information to the developer.
 *
 * MIT License
 * -----------
 *
 * Copyright 2026 link-coder100788
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MINI_FATAL_H
#define MINI_FATAL_H

#include <stdlib.h>
#include <pthread.h>

void mf_fatal_at_impl(const char* msg, const char* file, int line);

typedef struct mf_context_item {
    const char* msg;
    const char* file;
    int line;
    int col;
    const char* func;
    pthread_t thread_id;
    pid_t pid;
} mf_context_item;

mf_context_item mf_get_context_impl(const char* msg, const char* file, int line, int col, const char* func, pthread_t thread_id, pid_t pid);

typedef struct mf_context {
    mf_context_item* data;
    size_t size;
    size_t capacity;
} mf_context;

typedef void (*mf_callback_t)(void);

typedef struct mf_callback {
    mf_callback_t cb;
    mf_context_item context;
} mf_callback;

typedef struct mf_callback_stack {
    mf_callback* callback;
    size_t size;
    size_t capacity;
} mf_callback_stack;

typedef void* mf_assertable_t;

#ifdef __cplusplus

#include <vector>
#include <iostream>
#include <ostream>

namespace mf {
    void mf_version_cpp();

    void mf_fatal_cpp(std::string msg);

    void mf_fatal_if_cpp(int condition, std::string msg);

    void mf_fatal_if_null_cpp(const void* ptr, std::string msg);

    void mf_unreachable_cpp(std::string msg);

    void mf_todo_cpp(std::string msg);

    class Context {
    public:
        std::vector<mf_context_item> stack;

        void push(mf_context_item context);
        mf_context_item pop();
        void clear();
        void dump();

        static Context from_c_context(mf_context* ctx);
        mf_context to_c_context(size_t cap);
    };

    class Callbacks {
    public:
        std::vector<mf_callback> stack;

        void push(mf_callback callback);
        mf_callback pop();
        void clear();
        void fatal();

        static Callbacks from_c_callbacks(mf_callback_stack* stack);
        mf_callback_stack to_c_callbacks(size_t cap);
    };
}

extern "C" {

#endif

/**
 * Prints the version information of the Mini_Fatal library to the standard output.
 *
 * This function outputs the major, minor, and patch version numbers of the library
 * in a formatted string. It provides a quick way to check the current version
 * being used in the program.
 */
void mf_version();

/**
 * Logs a fatal error message to the standard error stream, dumps the current stack trace,
 * and terminates the program immediately.
 *
 * This function is intended to be used in critical error scenarios where the program
 * cannot gracefully continue execution. It outputs the error message in red text
 * (ANSI red color code) followed by the stack trace, providing useful debugging information.
 * After logging the issue, the program is aborted.
 *
 * @param msg The fatal error message to log. This should describe the reason for the failure
 *            in a way that aids debugging.
 */
void mf_fatal(const char* msg);

/**
 * Logs a critical error message, outputs a formatted stack trace, and terminates
 * the program execution immediately.
 *
 * This function is designed for handling unrecoverable errors where the application cannot
 * continue. It writes the error message in red text, followed by a detailed stack trace
 * to assist in debugging. The program exits after calling this function.
 *
 * @param fmt A printf-style format string that specifies the error message. This string can
 *            include format specifiers, which will be replaced with the variable arguments
 *            passed to the function.
 * @param ... Variable arguments that correspond to the format specifiers in the `fmt` string.
 */
void mf_panic(const char* fmt, ...);

/**
 * Logs a fatal error message to the standard error stream, dumps the current stack trace,
 * and terminates the program immediately if the specified condition is met.
 *
 * This function is intended to be used in critical error scenarios where the program
 * cannot continue execution due to a specific condition. It outputs the error message
 * in red text (ANSI red color code) followed by the stack trace, providing helpful
 * debugging details. If the condition is true, the program is aborted after logging the issue.
 *
 * @param condition The condition to evaluate. If this evaluates to a non-zero value,
 *                  the function logs the error message and terminates the program.
 * @param msg The fatal error message to log. This should describe the reason for the
 *            failure in a way that aids debugging.
 */
void mf_fatal_if(int condition, const char* msg);

/**
 * Checks if the provided pointer is null, and if so, logs a fatal error message,
 * dumps the current stack trace, and terminates the program immediately.
 *
 * This function is designed to catch and handle critical null-pointer errors that
 * indicate unrecoverable issues in the program's execution. If the pointer is null,
 * it outputs a detailed error message in red (ANSI red color code), followed by the
 * stack trace to aid debugging. After logging, the program is aborted.
 *
 * @param ptr The pointer to check. If this pointer is null, a fatal error is logged.
 * @param msg A descriptive error message explaining the context or reason for the
 *            null-pointer check. This helps provide additional debugging information.
 */
void mf_fatal_if_null(const void* ptr, const char* msg);

/**
 * Logs an error message indicating that unreachable code has been executed,
 * dumps the current stack trace for debugging purposes, and immediately terminates the program.
 *
 * This function should only be invoked in situations where the program has entered a
 * state that was assumed to be impossible. It provides a mechanism to alert developers
 * to such occurrences and facilitates debugging by outputting the stack trace.
 *
 * @param msg A message describing the context or reason for reaching the unreachable code.
 */
void mf_unreachable(const char* msg);

/**
 * Logs a "not yet implemented" message to the standard error stream, dumps the current stack trace,
 * and terminates the program immediately.
 *
 * This function is used as a placeholder for functionality that has not yet been implemented.
 * It outputs the provided message in red text (ANSI red color code) to help identify the issue,
 * followed by the current stack trace for debugging. After logging, the program is aborted.
 *
 * @param msg A description of the functionality that is not yet implemented. This message
 *            should explain the missing feature clearly to aid debugging and development.
 */
void mf_todo(const char* msg);

/**
 * Logs a fatal error message along with the file name and line number where the function
 * was called, dumps the current stack trace, and terminates the program immediately.
 *
 * This function is intended to provide detailed debugging information in critical error
 * scenarios. It outputs the error message prefixed with the file and line information
 * in color-coded text (ANSI red and yellow color codes) for better readability, followed
 * by a stack trace to help in diagnosing the issue. After logging, the program execution
 * is forcibly terminated.
 *
 * @param msg The fatal error message to log. This should provide a clear description
 *            of the error context for debugging purposes.
 */
#define mf_fatal_at(msg) mf_fatal_at_impl(msg, __FILE__, __LINE__)

/**
 * Logs a custom fatal error message to the standard error stream, outputs the current stack trace,
 * and terminates the program execution immediately.
 *
 * @param fullmsg The complete fatal error message to display. This should clearly explain the
 *                problem to assist in diagnosing the issue.
 */
void mf_fatal_msg(const char* fullmsg);

/**
 * Creates and initializes a new MF_Context structure with a specified initial capacity.
 *
 * This function allocates memory for the context's data storage and initializes
 * its size to 0. The capacity is set to the provided value, which defines how
 * many MF_Context_Item elements can initially be stored in the context without
 * requiring reallocation.
 *
 * @param cap The initial capacity for the MF_Context, representing the number
 *            of MF_Context_Item elements that the allocated data array can hold.
 *            This value should be greater than zero.
 * @return A newly created MF_Context structure with allocated storage and specified
 *         capacity. If memory allocation fails, mf_fatal is called through
 *         mf_fatal_if_null.
 */
mf_context mf_create_context(size_t cap);

/**
 * Pushes a new context item to the given context stack. If the stack is full (i.e.,
 * its size exceeds its capacity), this function logs a fatal error, dumps the stack
 * trace, and terminates the program.
 *
 * This function updates the context's internal array and increments its size to
 * include the new item.
 *
 * @param ctx The pointer to the context stack where the new item will be pushed.
 *            Must not be null, and its `data` array must be properly allocated
 *            with sufficient capacity.
 * @param context The context item to push onto the stack. Represents the
 *                information to be added to the context structure.
 */
void mf_context_push(mf_context* ctx, mf_context_item context);

/**
 * Removes and returns the last item from the given context's data stack.
 *
 * This function pops an item from the stack stored within the specified
 * context (`MF_Context`). If the context is null or the stack is empty,
 * appropriate handling is performed. A fatal error is triggered if the
 * context is null, ensuring that invalid operations do not proceed. In
 * the case of an empty stack, an empty `MF_Context_Item` with default
 * values is returned.
 *
 * @param ctx The context from which to pop the last item. This must not
 *            be null; otherwise, a fatal error is raised.
 * @return The last item in the context's stack. If the stack is empty,
 *         an `MF_Context_Item` with default-initialized members is returned.
 */
mf_context_item mf_context_pop(mf_context* ctx);

/**
 * Releases all resources associated with the specified context and resets its fields
 * to their default state. If the context object is null, the function reports a fatal
 * error and terminates the program.
 *
 * This function is typically used to correctly deallocate and reset a context structure
 * when it is no longer necessary, preventing memory leaks and leaving the object
 * in a safe, reusable state.
 *
 * @param ctx Pointer to the context to be destroyed. It must be a valid, non-null pointer.
 *            If the pointer is null, a fatal error is reported, and the program is aborted.
 */
void mf_context_destroy(mf_context* ctx);

/**
 * Dumps the current context information for debugging purposes.
 *
 * This method logs or outputs the current state of the context to help diagnose issues or analyze
 * the current configuration set within the framework.
 *
 * @param ctx A pointer to the context object containing the state information to be dumped.
 *                This must be a valid, initialized context.
 */
void mf_context_dump(mf_context* ctx);

#define mf_get_context(msg) mf_get_context_impl(msg, __FILE__, __LINE__, __COLUMN__, __PRETTY_FUNCTION__, pthread_self(), getpid())

/**
 * Dumps the provided context information and terminates the program with a fatal error message.
 *
 * This function combines the functionality of dumping the details of the given
 * context and reporting a fatal error. It first calls `mf_context_dump` to output
 * all context details, ensuring that all relevant diagnostic information is available.
 * Then, it invokes `mf_fatal` to display the fatal error message and terminate
 * the program execution.
 *
 * @param ctx A pointer to the mf_context structure containing context information to be dumped.
 *            Must not be null; otherwise, an error is reported.
 * @param msg A constant character pointer representing the fatal error message to be displayed.
 *            This provides additional details about the cause of the termination.
 */
void mf_fatal_dump(mf_context* ctx, const char* msg);

/**
 * Validates the success of a memory allocation operation.
 *
 * This function checks whether a memory allocation pointer is null, indicating
 * a failure in the allocation process. If the allocation fails, the program
 * will terminate with an error message.
 *
 * @param ptr The pointer to the newly allocated memory to be validated.
 */
void mf_check_alloc(const void* ptr);

/**
 * Creates and initializes a callback stack with the specified capacity.
 *
 * This function allocates memory for a stack of callback functions and initializes
 * its size and capacity. The capacity defines how many callback functions the stack
 * can hold. If memory allocation fails, the function terminates the program with
 * an error message and a stack trace.
 *
 * @param cap The desired capacity of the callback stack.
 * @return An initialized mf_callback_stack structure with the specified capacity.
 */
mf_callback_stack mf_create_callback_stack(size_t cap);

/**
 * Pushes a callback onto the callback stack.
 *
 * This function adds a new callback to the specified stack. If the stack is null,
 * or if the stack's capacity is exceeded, a fatal error is triggered.
 *
 * @param stack Pointer to the callback stack where the callback should be pushed.
 * @param callback The callback to be added to the stack.
 */
void mf_callback_push(mf_callback_stack* stack, mf_callback callback);

/**
 * Removes and returns the top callback from the specified callback stack.
 *
 * This function decreases the size of the callback stack and retrieves the
 * callback at the previous top position. If the stack is empty, it returns
 * an empty callback structure.
 *
 * @param stack A pointer to the mf_callback_stack from which the callback will be popped.
 *              The function will terminate with a fatal error if this pointer is null.
 * @return The top mf_callback from the stack. If the stack is empty, an empty
 *         mf_callback structure is returned.
 */
mf_callback mf_callback_pop(mf_callback_stack* stack);

/**
 * Destroys the callback stack and frees associated resources.
 *
 * This function clears the specified callback stack by releasing the memory
 * allocated for the callbacks, resetting the stack properties, and ensuring
 * that the stack is in a safe, empty state. If the provided stack pointer is null,
 * the program will terminate with an appropriate error message.
 *
 * @param stack A pointer to the mf_callback_stack to be destroyed. The pointer must not be null.
 */
void mf_callback_destroy(mf_callback_stack* stack);

/**
 * Executes all the registered callbacks in a callback stack, then generates a stack trace, and aborts the program.
 *
 * This function iterates through each callback present in the provided callback stack
 * and executes them. After all callbacks are executed, it generates and dumps
 * the current stack trace for diagnostic purposes and then forcibly terminates
 * the program using an abort operation.
 *
 * @param stack A pointer to an `mf_callback_stack` structure containing the callbacks to be executed.
 */
void mf_fatal_callback(mf_callback_stack* stack);

/**
 * Raises the specified signal, causing the program to handle it immediately.
 *
 * This function invokes the `raise` function with the given signal number,
 * which triggers the corresponding signal handler or default behavior
 * associated with that signal.
 *
 * @param sig The signal number to be raised. It must be a valid signal
 *            recognized by the system.
 */
void mf_fatal_signal(int sig);

/**
 * Compares two memory regions for equality and triggers a fatal error if they are not equal.
 *
 * This function verifies whether two memory regions, represented by `a` and `b`, are identical
 * by comparing their contents up to the specified `size`. If the regions are not equal, it
 * invokes a fatal error handling routine with the specified error message.
 *
 * @warning Compares raw memory. Use on types with a set and distinct size and format
 *
 * @param a Pointer to the first memory region to compare.
 * @param b Pointer to the second memory region to compare.
 * @param size The number of bytes to compare between the two memory regions.
 * @param msg A custom error message to display if the memory regions are not equal.
 */
void mf_assert_eq(mf_assertable_t a, mf_assertable_t b, size_t size, const char* msg);

/**
 * Asserts that two memory blocks are not equal in content.
 *
 * This function compares two memory blocks of the specified size. If the memory
 * blocks are equal, it triggers a fatal error and outputs the provided error message.
 * This is useful for ensuring that certain regions of memory are distinct during
 * debugging or runtime validation.
 *
 * @warning Compares raw memory. Use on types with a set and distinct size and format
 *
 * @param a Pointer to the first memory block.
 * @param b Pointer to the second memory block.
 * @param size The size of the memory blocks to compare, in bytes.
 * @param msg The error message to display if the assertion fails.
 */
void mf_assert_ne(mf_assertable_t a, mf_assertable_t b, size_t size, const char* msg);

#ifndef MF_NO_STACKTRACE

void mf_dump_stacktrace();

#endif

#ifdef MF_ENABLE_STANDALONE

[[noreturn]] static void mf_abort_standalone() {
#if defined(__clang__) || defined(__GNUC__)
    __builtin_trap();
#elif defined(_MSC_VER)
    __debugbreak();
#elif defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("int3");
    __asm__ volatile("ud2");
#elif defined (__aarch64__)
    __asm__ volatile("brk #0");
#elif defined(__arm__)
    __asm__ volatile(".inst 0xe7f001f0");
#elif defined(__riscv)
    __asm__ volatile("ebreak");
#endif
    for (;;) {
    }
}

static long mf_print_stderr__apple_arm64(const char* data, size_t len) {
#if defined(__APPLE__) && defined(__aarch64__)
    register long x0 __asm__("x0") = 2;
    register const char* x1 __asm__("x1") = data;
    register size_t x2 __asm__("x2") = len;
    register long x16 __asm__("x16") = 0x2000004;

    __asm__ volatile(
        "svc #0x80"
        : "+r"(x0)
        : "r"(x1), "r"(x2), "r"(x16)
        : "memory"
    );

    return x0;
#else
    (void)data;
    (void)len;
    return -1;
#endif
}

#endif

#ifdef __cplusplus
}
#endif

#ifdef MF_NO_STACKTRACE

#define DUMP_STACKTRACE() ((void) 0)

#else

#define DUMP_STACKTRACE() mf_dump_stacktrace()

#endif

#ifndef MF_ABRT
#define MF_ABRT() abort()
#endif

#ifndef MF_DISABLE_COLOR
#define MF_RED "\033[31m"
#define MF_YELLOW "\033[33m"
#define MF_RESET "\033[0m"
#else
#define MF_RED ""
#define MF_YELLOW ""
#define MF_RESET ""
#endif

#define MF_MAJOR 0
#define MF_MINOR 1
#define MF_PATCH 0

#ifndef __COLUMN__
#define __COLUMN__ -1
#endif

#ifdef MINI_FATAL_IMPLEMENTATION

#define MF_TOSTRING(x) MF_TOSTRING_IMPL(x)
#define MF_TOSTRING_IMPL(x) #x

#define MF_AT_HELPER __FILE__ ":" MF_TOSTRING(__LINE__)

#ifndef MF_MAX_CONTEXT
#define MF_MAX_CONTEXT 64
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>

#if defined(__unix__) || defined(__APPLE__)

#include <execinfo.h>
#include <unistd.h>
// ReSharper disable CppUnusedIncludeDirective
#include <signal.h>
// ReSharper restore CppUnusedIncludeDirective
#define MF_POSIX 1

#elif

#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#define MF_WINDOWS 1

#else

#error "Unsupported platform"

#endif

#ifndef MF_NO_STACKTRACE

inline void mf_dump_stacktrace() {
#if MF_POSIX

    void* stack[64];
    int size = backtrace(stack, 64);
    backtrace_symbols_fd(stack, size, STDERR_FILENO);

#elif MF_WINDOWS

    void* stack[62];
    USHORT frames = CaptureStackBackTrace(0, 62, stack, NULL);
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    for (USHORT i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        fprintf(stderr, "%u: %s\n", i, symbol->Name);
    }
    free(symbol);

#endif
}

#endif

inline void mf_version() {
    printf("Mini_Fatal %d.%d.%d\n", MF_MAJOR, MF_MINOR, MF_PATCH);
}

inline void mf_fatal(const char* msg) {
    fprintf(stderr, MF_RED "Fatal error: %s\n" MF_RESET, msg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_panic(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, MF_RED "Fatal error: ");
    vprintf(fmt, args);
    fprintf(stderr, "\n" MF_RESET);
    va_end(args);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_fatal_if(int condition, const char* msg) {
    if (condition) {
        fprintf(stderr, MF_RED "Fatal error: %s\n" MF_RESET, msg);
        DUMP_STACKTRACE();
        MF_ABRT();
    }
}

inline void mf_fatal_if_null(const void* ptr, const char* msg) {
    if (!ptr) {
        fprintf(stderr, MF_RED "Fatal error: Pointer %p was null: %s\n" MF_RESET, ptr, msg);
        DUMP_STACKTRACE();
        MF_ABRT();
    }
}

inline void mf_unreachable(const char* msg) {
    if (!msg) fprintf(stderr, MF_RED "Unreachable code reached\n" MF_RESET);
    else fprintf(stderr, MF_RED "Unreachable code reached: %s\n" MF_RESET, msg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_todo(const char* msg) {
    if (!msg) fprintf(stderr, MF_RED "Not yet implemented" MF_RESET);
    else fprintf(stderr, MF_RED "Not yet implemented: %s\n" MF_RESET, msg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_fatal_at_impl(const char* msg, const char* file, int line) {
    fprintf(stderr, MF_RED "Fatal error at " MF_RESET MF_YELLOW "%s:%d" MF_RESET MF_RED ": %s\n" MF_RESET, file, line, msg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_fatal_msg(const char* fullmsg) {
    fprintf(stderr, MF_RED "%s\n" MF_RESET, fullmsg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline mf_context mf_create_context(size_t cap) {
    mf_context ctx;
    ctx.data = (mf_context_item*)malloc(cap * sizeof(mf_context_item));
    mf_fatal_if_null(ctx.data, "Failed to allocate context data");
    ctx.size = 0;
    ctx.capacity = cap;
    return ctx;
}

inline void mf_context_push(mf_context* ctx, mf_context_item context) {
    mf_fatal_if_null(ctx, "Context is null");
    if (ctx->size >= ctx->capacity) {
        mf_fatal_at("Context overflow");
    }
    ctx->data[ctx->size++] = context;
}

inline mf_context_item mf_context_pop(mf_context* ctx) {
    mf_fatal_if_null(ctx, "Context is null");
    if (ctx->size == 0) {
        mf_context_item empty = {0};
        return empty;
    }
    return ctx->data[--ctx->size];
}

inline void mf_context_destroy(mf_context* ctx) {
    mf_fatal_if_null(ctx, "Context is null");
    free(ctx->data);
    ctx->data = NULL;
    ctx->size = 0;
    ctx->capacity = 0;
}

inline void mf_context_dump(mf_context* ctx) {
    mf_fatal_if_null(ctx, "Context is null");
    for (size_t i = 0; i < ctx->size; i++) {
        mf_context_item* item = &ctx->data[i];
        printf("%s at %s:%d:%d in %s threadid: %p pid: %d\n", item->msg, item->file, item->line, item->col, item->func, item->thread_id, item->pid);
    }
}

inline mf_context_item mf_get_context_impl(const char* msg, const char* file, int line, int col, const char* func, pthread_t threadid, pid_t pid) {
    mf_context_item context;
    context.msg = msg;
    context.file = file;
    context.line = line;
    context.col = col;
    context.func = func;
    context.thread_id = threadid;
    context.pid = pid;
    return context;
}

inline void mf_fatal_dump(mf_context* ctx, const char* msg) {
    mf_context_dump(ctx);
    mf_fatal(msg);
}

inline void mf_check_alloc(const void* ptr) {
    mf_fatal_if_null(ptr, "Memory allocation failed");
}

inline mf_callback_stack mf_create_callback_stack(size_t cap) {
    mf_callback_stack stack;
    stack.callback = (mf_callback*)malloc(cap * sizeof(mf_callback));
    mf_fatal_if_null(stack.callback, "Failed to allocate callback stack");
    stack.size = 0;
    stack.capacity = cap;
    return stack;
}

inline void mf_callback_push(mf_callback_stack* stack, mf_callback callback) {
    mf_fatal_if_null(stack, "Callback stack is null");
    if (stack->size >= stack->capacity) {
        mf_fatal_at("Callback stack overflow");
    }
    stack->callback[stack->size++] = callback;
}

inline mf_callback mf_callback_pop(mf_callback_stack* stack) {
    mf_fatal_if_null(stack, "Callback stack is null");
    if (stack->size == 0) {
        mf_callback empty = {0};
        return empty;
    }
    return stack->callback[--stack->size];
}

inline void mf_callback_destroy(mf_callback_stack* stack) {
    mf_fatal_if_null(stack, "Callback stack is null");
    free(stack->callback);
    stack->callback = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

inline void mf_fatal_callback(mf_callback_stack* stack) {
    for (size_t i = 0; i < stack->size; i++) {
        mf_callback* callback = &stack->callback[i];
        callback->cb();
    }
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_fatal_signal(int sig) {
    DUMP_STACKTRACE();
    raise(sig);
}

inline void mf_assert_eq(mf_assertable_t a, mf_assertable_t b, size_t size, const char* msg) {
    if (memcmp(a, b, size) == 0) return;
    mf_fatal(msg);
}

inline void mf_assert_ne(mf_assertable_t a, mf_assertable_t b, size_t size, const char* msg) {
    if (memcmp(a, b, size) != 0) return;
    mf_fatal(msg);
}

#ifdef __cplusplus

inline void mf::mf_version_cpp() {
    std::cout << "Mini_Fatal " << MF_MAJOR << "." << MF_MINOR << "." << MF_PATCH << std::endl;
}

inline void mf::mf_fatal_cpp(std::string msg) {
    mf_fatal(msg.c_str());
}

inline void mf::mf_fatal_if_cpp(int condition, std::string msg) {
    if (condition) mf_fatal(msg.c_str());
}

inline void mf::mf_fatal_if_null_cpp(const void* ptr, std::string msg) {
    if (!ptr) mf_fatal(msg.c_str());
}

inline void mf::mf_unreachable_cpp(std::string msg) {
    mf_unreachable(msg.c_str());
}

inline void mf::mf_todo_cpp(std::string msg) {
    mf_todo(msg.c_str());
}

inline void mf::Context::push(mf_context_item context) {
    stack.push_back(context);
}

inline mf_context_item mf::Context::pop() {
    auto back = stack.back();
    stack.pop_back();
    return back;
}

inline void mf::Context::clear() {
    stack.clear();
}

inline void mf::Context::dump() {
    for (auto& context : stack) {
        std::cout << context.msg
        << " at "
        << context.file
        << ":"
        << context.line
        << ":"
        << context.col
        << " in "
        << context.func
        << " threadid:  "
        << context.thread_id
        << " pid: "
        << context.pid
        << std::endl;
    }
}

inline mf::Context mf::Context::from_c_context(mf_context* ctx) {
    mf_fatal_if_null(ctx, "Context is null");
    Context context;
    for (size_t i = 0; i < ctx->size; i++) {
        mf_context_item* item = &ctx->data[i];
        context.push(mf_context_item { item->msg, item->file, item->line, item->col, item->func, item->thread_id, item->pid });
    }
    return context;
}

inline mf_context mf::Context::to_c_context(size_t cap) {
    if (cap < stack.size()) mf_panic("Capacity of %d is too small for a size of %d!", cap, stack.size());
    mf_context ctx;
    ctx.data = (mf_context_item*)malloc(cap * sizeof(mf_context_item));
    ctx.capacity = cap;
    ctx.size = 0;
    for (auto& item : stack) {
        mf_context_push(&ctx, mf_context_item { item.msg, item.file, item.line, item.col, item.func, item.thread_id, item.pid });
    }
    return ctx;
}

inline void mf::Callbacks::push(mf_callback callback) {
    stack.push_back(callback);
}

inline mf_callback mf::Callbacks::pop() {
    auto back = stack.back();
    stack.pop_back();
    return back;
}

inline void mf::Callbacks::clear() {
    stack.clear();
}

inline void mf::Callbacks::fatal() {
    for (auto& c : stack) {
        c.cb();
    }
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline mf::Callbacks mf::Callbacks::from_c_callbacks(mf_callback_stack* stack) {
    mf_fatal_if_null(stack, "Callbacks stack is null");
    Callbacks callbacks;
    for (size_t i = 0; i < stack->size; i++) {
        mf_callback* callback = &stack->callback[i];
        callbacks.push(mf_callback { callback->cb });
    }
    return callbacks;
}

inline mf_callback_stack mf::Callbacks::to_c_callbacks(size_t cap) {
    if (cap < stack.size()) mf_panic("Capacity of %d is too small for a size of %d!", cap, stack.size());
    mf_callback_stack callbacks;
    callbacks.callback = (mf_callback*)malloc(cap * sizeof(mf_callback));
    callbacks.capacity = cap;
    callbacks.size = 0;
    for (auto& c : stack) {
        mf_callback_push(&callbacks, mf_callback { c.cb });
    }
    return callbacks;
}

#endif

#endif

#endif //MINI_FATAL_H
