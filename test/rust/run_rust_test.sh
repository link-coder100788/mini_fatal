#!/bin/zsh

clang -c rust_test_helper.c -o rust_test_helper.o
rustc main.rs -C link-arg=rust_test_helper.o -o rust_test
./rust_test
rm rust_test_helper.o
rm rust_test