#!/bin/bash

g++ src/main.cpp -l sqlite3 -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -o studentScoreSystem