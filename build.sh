#!/bin/sh

g++ src/main.cpp src/sqlite_query.cpp -l sqlite3 -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -o main.exe