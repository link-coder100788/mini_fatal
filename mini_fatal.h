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

#ifndef MF_NO_STACKTRACE

void mf_dump_stacktrace();

#endif

#ifdef __cplusplus
}
#endif

#ifdef MF_NO_STACKTRACE
#define DUMP_STACKTRACE() ((void)0)
#else
#define DUMP_STACKTRACE() mf_dump_stacktrace()
#endif

#define MF_RED "\033[31m"
#define MF_RESET "\033[0m"

#ifdef MINI_FATAL_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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
    abort();
}

inline void mf_panic(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, MF_RED "Fatal error: ");
    vprintf(fmt, args);
    fprintf(stderr, "\n" MF_RESET);
    va_end(args);
    DUMP_STACKTRACE();
    abort();
}

#endif

#endif //MINI_FATAL_H
