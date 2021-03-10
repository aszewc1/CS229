# Alexandra Szewc	aszewc1
# Victor Wen		vwen2

#
# CSF Assignment 3 Makefile
#

# Executables to build for the "all" target
EXES = csim

# Source file
C_SRCS = cache.c

# Compiler flags
CFLAGS = -g -Og -Wall -Wextra -pedantic -std=gnu11

# Rule for compiling and assembling a .c file to produce a .o file
%.o : %.c
	gcc $(CFLAGS) -c $*.c -o $*.o

# Default target: build all executables
all : $(EXES)

# C version of postfix calculator
csim : cache.o main.o
	gcc -o $@ $^

# Clean up generated files (such as object files and executables)
clean :
	rm -f *~ *.o $(EXES) depend.mak solution.zip

depend.mak :
	touch $@

# This target generates C header file dependencies (so that C modules
# get recompiled whenever their source or included header files get
# modified).
depend :
	gcc -M $(CFLAGS) $(C_SRCS) >> depend.mak

include depend.mak
