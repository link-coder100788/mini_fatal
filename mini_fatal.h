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

void mf_fatal_at_impl(const char* msg, const char* file, int line);

typedef struct mf_context_item {
    const char* msg;
    const char* file;
    int line;
} mf_context_item;

mf_context_item mf_get_context_impl(const char* msg, const char* file, int line);

typedef struct mf_context {
    mf_context_item* data;
    size_t size;
    size_t capacity;
} mf_context;

#ifdef __cplusplus

#include <vector>
#include <iostream>
#include <ostream>

namespace mf {
    class Context {
    public:
        std::vector<mf_context_item> stack;

        void push(mf_context_item context);
        mf_context_item pop();
        void clear();
        void dump();
    };
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
        std::cout << context.msg << " at " << context.file << ":" << context.line << std::endl;
    }
}

#endif

#ifdef __cplusplus
extern "C" {
#endif

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
 * when it is no longer needed, preventing memory leaks and leaving the object
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
 * @param context A pointer to the context object containing the state information to be dumped.
 *                This must be a valid, initialized context.
 */
void mf_context_dump(mf_context* ctx);

#define mf_get_context(msg) mf_get_context_impl(msg, __FILE__, __LINE__)

#ifndef MF_NO_STACKTRACE

void mf_dump_stacktrace();

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

#define MF_RED "\033[31m"
#define MF_YELLOW "\033[33m"
#define MF_RESET "\033[0m"

#define MF_MAJOR 0
#define MF_MINOR 1
#define MF_PATCH 0

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
#include <signal.h>
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
    fprintf(stderr, MF_RED "Unreachable code reached: %s\n" MF_RESET, msg);
    DUMP_STACKTRACE();
    MF_ABRT();
}

inline void mf_todo(const char* msg) {
    fprintf(stderr, MF_RED "Not yet implemented: %s\n" MF_RESET, msg);
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
        printf("%s at %s:%d\n", item->msg, item->file, item->line);
    }
}

inline mf_context_item mf_get_context_impl(const char* msg, const char* file, int line) {
    mf_context_item context = {msg, file, line};
    return context;
}

#endif

#endif //MINI_FATAL_H
