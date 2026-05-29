-- Create table for errors from mini_fatal
-- This is a test file and is not used in the library
-- The SQL below is found on line 1235 of mini_fatal.h

INSERT INTO mf_errors (usr_message, file, line, func) VALUES (?, ?, ?, ?);