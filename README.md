# mini_fatal

`mini_fatal` is a small, single-header C/C++ library for reporting unrecoverable program failures.

It provides a minimal fatal-error API that prints a clear diagnostic message, optionally dumps a stack trace, and then terminates the program. It is intended for cases where continuing execution would be unsafe, misleading, or impossible.

## Features

- **Single-header library**  
  Drop `mini_fatal.h` into your project and include it.

- **C and C++ compatible**  
  The public API uses C linkage when included from C++.

- **Fatal error reporting**  
  Print fatal error messages to standard error and abort execution.

- **Formatted panic messages**  
  Use `printf`-style formatting for fatal diagnostics.

- **Optional stack traces**  
  Emit a best-effort stack trace on supported platforms.

- **Compile-time configuration**  
  Disable stack trace support when it is not needed or not available.

- **Minimal API surface**  
  Designed to be easy to understand, extend, and embed.

## Use Cases

`mini_fatal` is useful for failures such as:

- violated internal invariants
- impossible control-flow paths
- corrupted program state
- failed assumptions that indicate a programming error
- critical initialization failures
- unrecoverable runtime errors

It is **not** intended to replace normal error handling, return codes, exceptions, or logging frameworks. Use it when the only correct response is to stop execution immediately.

## Installation

Copy `mini_fatal.h` into your project and include it where needed.

```c
#include "mini_fatal.h"
```

Because this is a single-header library, define `MINI_FATAL_IMPLEMENTATION` in exactly one source file before including the header:

```c
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"
```

In all other files, include it normally:

```c
#include "mini_fatal.h"
```

## Quick Example
```c
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"

int main(void) {
mf_fatal("failed to initialize application");
return 0;
}
```

Output will look similar to:

```text
Fatal error: failed to initialize application
<stack trace if enabled and supported>
```

The program will then abort.

## Formatted Panic Example

Use `mf_panic` when you want a formatted fatal error message:
```c
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"

int main(void) {
const char *path = "config.json";
int error_code = 2;

    mf_panic("failed to open '%s' with error code %d", path, error_code);

    return 0;
}
```

Example output:

```
text
Fatal error: failed to open 'config.json' with error code 2
<stack trace if enabled and supported>
```

## API

### `mf_fatal`

```c
void mf_fatal(const char *msg);
```

Prints a fatal error message, dumps a stack trace if enabled, and aborts the program.

Use this when you already have a plain string message.

### `mf_panic`

```c
void mf_panic(const char *fmt, ...);
```

Prints a formatted fatal error message using a `printf`-style format string, dumps a stack trace if enabled, and aborts the program.

Use this when you want to include values in the fatal message.

### `mf_dump_stacktrace`

```c
void mf_dump_stacktrace(void);
```

Dumps a best-effort stack trace for the current thread.

This function is only available when stack trace support is enabled. If `MF_NO_STACKTRACE` is defined, stack trace dumping is disabled.

## Configuration

### `MINI_FATAL_IMPLEMENTATION`

Define this macro in exactly one source file before including `mini_fatal.h`.

```c
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"
```

This emits the implementation for the library.

### `MF_NO_STACKTRACE`

Define this macro before including `mini_fatal.h` to disable stack trace support.
This is useful for embedded systems where stack traces are not available or not desired.

```c
#define MF_NO_STACKTRACE
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"
```

When stack traces are disabled, fatal functions still print the error message and abort, but no stack trace is emitted.

## Platform Support

`mini_fatal` currently targets:

- POSIX-like systems, including Linux and macOS
- Windows

Stack traces are implemented using platform-specific APIs where available.

## C++ Usage

`mini_fatal` can be included from C++ code:
```cpp
#define MINI_FATAL_IMPLEMENTATION
#include "mini_fatal.h"

int main() {
mf_panic("unrecoverable failure in module %s", "startup");
return 0;
}
```

The public functions are exposed with C linkage when compiled as C++.

## Notes

- Fatal functions do not return.
- Fatal functions call `abort()` after printing diagnostics.
- Stack traces are best-effort and may vary by platform, compiler, linker flags, and build type.
- Debug builds usually produce more useful stack traces than optimized release builds.
- On some platforms, extra linker flags or debug symbols may improve stack trace quality.

## License

This library is licensed under the MIT license