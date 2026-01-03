
gcc -c sqlite3.c -O2 -DSQLITE_THREADSAFE=1 -DSQLITE_OMIT_LOAD_EXTENSION
# ar rcs libsqlite3.a sqlite3.o

