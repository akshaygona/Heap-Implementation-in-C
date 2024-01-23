C_FILES := $(wildcard *.c)
TARGETS := ${C_FILES:.c=}

all: ${TARGETS}

# Build each TARGET from its corresponding C source file
%: %.c
	gcc -I.. -g -m32 -Xlinker -rpath=.. -o $@ $< -L.. -lheap -std=gnu99

# Remove all generated target files (executables)
# Use before running make to get a clean re-build of all targets.
clean:
	rm -f ${TARGETS} *.o *.output

