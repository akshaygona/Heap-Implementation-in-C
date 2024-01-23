README: p3 cs354heap Dynamic Memory Allocator

### F23 Assignment page:
Part A:
https://canvas.wisc.edu/courses/360002/assignments/2039988

Part B:
https://canvas.wisc.edu/courses/360002/assignments/2057583

### Provided Files
README   link to project page and info about other files
Makefile used by make to build a shared object file needed for testing
cs354heap.h header file with the signatures for public "shared" functions
cs354heap.c source code with functions that must be completed
tests/   a bunch of tests, and you can write your own!

### How to use the Makefile

To build just the heap implementation, run the following from the top-level
directory in your project directory:

    make

This builds your c file cs354heap.c and then links it into a library (shared
object) file -- libheap.so. It is libheap.so which then gets dynamically loaded
by the test programs when you run them.

To build the test programs, run the following from the top-level directory in
your project directory:

    make buildtests

To run a single test program, any of the following work (after building):

    ./tests/$TEST_NAME

    OR

    make $TEST_NAME.run

The latter will save the output of the test in tests/$TEST_NAME.output.

To run all tests:

    make runtests

This will also save all the tests output in .output files.

### Write your own tests to help your incremental development
You may edit the tests given, but it is probably best to copy
each test to a new file and edit your own tests so that you 
can keep all tests as they have been provided.

We will grade based on these tests and others we do not provide.

