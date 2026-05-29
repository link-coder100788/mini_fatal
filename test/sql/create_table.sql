-- Create table for errors from mini_fatal
-- This is a test file and is not used in the library
-- The SQL below is found on line 1223 of mini_fatal.h

CREATE TABLE IF NOT EXISTS mf_errors (
    id INTEGER PRIMARY KEY,
    usr_message TEXT,
    file TEXT,
    line INT,
    func TEXT,
    time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);