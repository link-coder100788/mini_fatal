#define MINI_FATAL_IMPLEMENTATION
#define MF_NO_STACKTRACE
#include "../mini_fatal.h"

#include <stdio.h>

typedef int (*test_fn)(void);

typedef struct test_case {
    const char* name;
    test_fn fn;
} test_case;

#define ASSERT_TRUE(expr)                                                       \
    do {                                                                        \
        if (!(expr)) {                                                          \
            fprintf(stderr, "Assertion failed: %s at %s:%d\n",                  \
                    #expr, __FILE__, __LINE__);                                 \
            return 1;                                                           \
        }                                                                       \
    } while (0)

#define RUN_TEST(fn) { #fn, fn }

static int test_version_macros_exist(void) {
    ASSERT_TRUE(MF_MAJOR >= 0);
    ASSERT_TRUE(MF_MINOR >= 0);
    ASSERT_TRUE(MF_PATCH >= 0);
    return 0;
}

static int test_color_macros_exist(void) {
    ASSERT_TRUE(MF_RED[0] != '\0');
    ASSERT_TRUE(MF_RESET[0] != '\0');
    return 0;
}

static int test_public_api_symbols_compile(void) {
    /*
     * Do not call mf_fatal() or mf_panic() here because they intentionally abort.
     * Referencing the declarations verifies that the public API is visible.
     */
    void (*fatal_fn)(const char*) = mf_fatal;
    void (*panic_fn)(const char*, ...) = mf_panic;

    ASSERT_TRUE(fatal_fn != 0);
    ASSERT_TRUE(panic_fn != 0);

    return 0;
}

static int test_stacktrace_macro_compiles(void) {
    DUMP_STACKTRACE();
    return 0;
}

static int test_context_c(void) {
    MF_Context ctx = mf_create_context(10);
    mf_context_push(&ctx, mf_get_context("testing mini_fatal.h in a c context"));
    mf_context_push(&ctx, mf_get_context("running tests"));
    mf_context_push(&ctx, mf_get_context("test_context_c"));
    mf_context_dump(&ctx);
    mf_context_destroy(&ctx);
    return 0;
}

static const test_case tests[] = {
    RUN_TEST(test_version_macros_exist),
    RUN_TEST(test_color_macros_exist),
    RUN_TEST(test_public_api_symbols_compile),
    RUN_TEST(test_stacktrace_macro_compiles),
    RUN_TEST(test_context_c),
};

int main(void) {
    int failures = 0;
    unsigned int i = 0;
    const unsigned int test_count = (unsigned int)(sizeof(tests) / sizeof(tests[0]));

    for (i = 0; i < test_count; ++i) {
        const int result = tests[i].fn();

        if (result == 0) {
            printf("[PASS] %s\n", tests[i].name);
        } else {
            printf("[FAIL] %s\n", tests[i].name);
            failures += 1;
        }
    }

    printf("%u test(s), %d failure(s)\n", test_count, failures);
    return failures == 0 ? 0 : 1;
}