#!/bin/zsh

clang -c swift_test_helper.m -o swift_test_helper.o
swiftc test.swift swift_test_helper.o -import-objc-header MFFatalSwift-Bridging-Header.h -o swift_test
./swift_test
rm swift_test
rm swift_test_helper.o