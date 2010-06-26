#!/bin/bash

if [ -e tests/sampledb.sqlitedb ]
then
	rm tests/sampledb.sqlitedb;
fi

sqlite3 tests/sampledb.sqlitedb < ../tests/sampledb.sqlite3.sql

