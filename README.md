# freec

A runtime memory error detector for C that catches double-frees, use-after-frees, invalid frees and leaks at runtime. On detection, it prints beautiful, Rust-style error messages that point at the exact source line where things went wrong.

Loaded into any C program with `LD_PRELOAD`, which means no recompilation needed (other than `-g` for source-line info).

## How to run

1. Clone the repository

   ```bash
   git clone https://github.com/byebyelabs/freec.git
   ```

2. Navigate to the project directory

   ```bash
   cd freec
   ```

3. Build the shared library

   ```bash
   make all
   ```

   This produces `build/byebye_freec.so`.

4. Compile your own program with `-g` and run it with `LD_PRELOAD`:

   ```bash
   clang -g -o main main.c
   LD_PRELOAD=./build/byebye_freec.so ./main
   ```

   Or use `make setup`, which builds the library and appends a `freec` alias to your `~/.bashrc`:

   ```bash
   make setup
   freec ./main
   ```

## Running the test suite

The [tests/](tests/) directory has a few C programs that all purposefully throw some memory bug. The main test file ([tests/\_\_main\_\_.py](tests/__main__.py)) compiles each one with `-g` and runs it under the `freec` library; this gives a corresponding error report.

```bash
make tests
make tests/double_free
make tests/use_after_free
make tests/invalid_free
make tests/memory_leak
```

## What it catches

- Use after free: dereferencing memory after it has been freed
- Double free: freeing the same allocation more than once
- Invalid free: calling `free()` on a non-heap pointer or a pointer in the middle of an allocation (instead of the original pointer)
- Dangling pointers: memory that was never freed before the program exited

When `freec` detects a violation, it prints a Rust-style error message that points at where the violation occurred, the original `malloc` location, and (when relevant) the most recent valid event on that allocation:

```
error: cannot free the same memory more than once
-> ./tests/double_free_basic.c:8
    05  int *p = malloc(sizeof(int) * 10);
        ---------------------------------- malloc happened here
    ...
    07  free(p);
        -------- correctly freed here
    ...
    08  free(p); // DOUBLE FREE
        ^^^^^^^^^^^^^^^^^^^^^^^ second free occurred here
```

### Error Formatting

`_dump_error_info` in [src/tracing.c](src/tracing.c) prints the Rust-style error message. For each relevant trace (malloc location, last valid event, violation point) it opens the source file, reads the offending line, and underlines it with `^` (red) for the violation or `-` (blue) for context.

## Notes

- The user's program must be compiled with `-g`
- `printf` and `scanf` allocate internal buffers that `glibc` frees lazily after `_exit`. The dangling-pointer check explicitly skips allocations whose backtrace went through them
- Only one page is kept right now, with allocations packed by offset. Allocations larger than a page (or many small allocations in aggregate) aren't yet supported (see the `TODO` in [src/runtime.c](src/runtime.c))
