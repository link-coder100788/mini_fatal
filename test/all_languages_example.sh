#!/bin/zsh

echo "Testing all language examples for mini_fatal!"
echo "------------------------------------\n\n"
echo "Testing go: "
cd go
go run .
cd ..
echo "\n\nTesting rust: "
cd rust
./run_rust_test.sh
cd ..
echo "\n\nTesting swift: "
cd swift
./run_swift_test.sh
cd ..
echo "\n\nTesting v: "
cd v
v run .
cd ..
echo "\n\nTesting zig: "
cd zig
./run_zig_test.sh
cd ..
echo "Finished!"