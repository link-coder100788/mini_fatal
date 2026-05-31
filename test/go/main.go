package main

// #define MINI_FATAL_IMPLEMENTATION
// #define MF_ABRT() exit(1)
// #include "../../mini_fatal.h"
import "C"

func main() {
    C.mf_fatal(C.CString("Fatal error in go!"))
}