#!/bin/zsh

clang helper.c -c -o mini_fatal.o
ar rcs libmini_fatal.a mini_fatal.o
rm mini_fatal.o