#!/usr/bin/env ruby

require "fileutils"

ROOT = File.expand_path(__dir__)

TESTS = [
  {
    name: "C",
    compiler_env: "CC",
    default_compiler: "clang",
    source: File.join(ROOT, "test", "test.c"),
    binary: File.join(ROOT, "test", "test_c"),
    flags: ["-std=c99", "-Wall", "-Wextra", "-pedantic"]
  },
  {
    name: "C++",
    compiler_env: "CXX",
    default_compiler: "clang++",
    source: File.join(ROOT, "test", "test.cpp"),
    binary: File.join(ROOT, "test", "test_cpp"),
    flags: ["-std=c++11", "-Wall", "-Wextra", "-pedantic"]
  }
].freeze

def exe_path(path)
  Gem.win_platform? ? "#{path}.exe" : path
end

def run_command(command)
  puts command.join(" ")
  system(*command)
end

failures = 0
binaries = TESTS.map { |test| exe_path(test[:binary]) }

begin
  TESTS.each do |test|
    compiler = ENV.fetch(test[:compiler_env], test[:default_compiler])
    binary = exe_path(test[:binary])

    compile_command = [
      compiler,
      *test[:flags],
      test[:source],
      "-o",
      binary
    ]

    puts "\n== Compiling #{test[:name]} test =="
    unless run_command(compile_command)
      warn "Failed to compile #{test[:name]} test"
      failures += 1
      next
    end

    puts "\n== Running #{test[:name]} test =="
    unless run_command([binary])
      warn "#{test[:name]} test failed"
      failures += 1
    end
  end
ensure
  binaries.each do |binary|
    FileUtils.rm_f(binary)
  end
end

puts "\n#{failures.zero? ? 'All tests passed.' : "#{failures} test step(s) failed."}"
exit(failures.zero? ? 0 : 1)