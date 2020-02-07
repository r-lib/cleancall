# cleancall

> C Resource Cleanup via Exit Handlers

<!-- badges: start -->

![lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)
[![Travis build status](https://travis-ci.org/r-lib/cleancall.svg?branch=master)](https://travis-ci.org/r-lib/cleancall)
[![Windows Build
status](https://ci.appveyor.com/api/projects/status/p2jjoufya2e66oa5/branch/master?svg=true)](https://ci.appveyor.com/project/gaborcsardi/cleancall)
[![](https://www.r-pkg.org/badges/version/cleancall)](https://cran.r-project.org/package=cleancall)
[![CRAN RStudio mirror
downloads](https://cranlogs.r-pkg.org/badges/cleancall)](https://www.r-pkg.org/pkg/cleancall)
[![Coverage status](https://codecov.io/gh/r-lib/cleancall/branch/master/graph/badge.svg)](https://codecov.io/github/r-lib/cleancall?branch=master)
<!-- badges: end -->

## Features

* Add exit handlers to a `.Call()` to C, via a `call_with_cleanup()`
  wrapper.
* Restrict an exit handler to run only on early exit (error, interrupt,
  debugger exit, restart invokation, condition caught, etc.).
  I.e. anything that prevented your function from running its normal course.
* Exit handlers are executed in reverse order. Last added runs first.
* Exit handlers can be added from any downstream function, they don't need
  to be called directly from the function called by `call_with_cleanup()`.

## Limitations

We suggest that exit handlers are kept as simple and fast as possible.
In particular, errors (and other early exits) triggered from exit handlers
are not caught currently. If an exit handler exits early the others do not
run. If this is an issue, you can wrap the exit handler in
`R_tryCatch()` (available for R 3.4.0 and later).

## Installation

You can install the released version of cleancall from
[CRAN](https://CRAN.R-project.org) with:

``` r
install.packages("cleancall")
```

## Example

This example is from the processx package. Its `processx_wait()` function
waits for an external process to end, and this wait is interruptible.
`processx_wait()` opens two temporary file descriptors for the wait, and
these need to be closed at the end of the function, even on an interrupt,
otherwise we have a resource leak.

See
[this link](https://github.com/r-lib/processx/blob/a8f09d147fead78347a87fcf4e0fbd1c07de1c21/src/unix/processx.c#L507-L589)
for the complete function, before fixing.

Here we only include the relevant parts:

```c
SEXP processx_wait(SEXP status, SEXP timeout) {
  processx_handle_t *handle = R_ExternalPtrAddr(status);
  int ctimeout = INTEGER(timeout)[0], timeleft = ctimeout;
  struct pollfd fd;
  int ret = 0;
  pid_t pid;

  [...]

  /* Setup the self-pipe that we can poll */
  if (pipe(handle->waitpipe)) {
    processx__unblock_sigchld();
    error("processx error: %s", strerror(errno));
  }

  [...]

  while (ctimeout < 0 || timeleft > PROCESSX_INTERRUPT_INTERVAL) {
    do {
      ret = poll(&fd, 1, PROCESSX_INTERRUPT_INTERVAL);
    } while (ret == -1 && errno == EINTR);

    /* If not a timeout, then we are done */
    if (ret != 0) break;

    R_CheckUserInterrupt();

    [...]
  }

  [...]

cleanup:
  if (handle->waitpipe[0] >= 0) close(handle->waitpipe[0]);
  if (handle->waitpipe[1] >= 0) close(handle->waitpipe[1]);
  handle->waitpipe[0] = -1;
  handle->waitpipe[1] = -1;

  return ScalarLogical(ret != 0);
}
```

`pipe()` allocates two file descriptors, they are saved in
`handle->waitpipe[0]` and `handle->waitpipe[1]`. The wait is interruptible,
so the function calls `R_CheckUserInterrupt()`. This checks for a
`CTRL+C` or `ESC` interrupt, and if there was one, it returns directly to
the caller of `.Call()`. This is of course problematic, because
`processx_wait()` has no chance of closing the pipe file descriptors.

Fixing this with cleancall is as follows. First your package needs to
depend on cleancall, update `DESCRIPTION`:

```
[...]
Imports:
    cleancall,
LinkingTo:
    cleancall
[...]
```

In the R code calling `processx_wait(),` replace `.Call()` with
`cleancall::call_with_cleanup()`:

```r
cleancall::call_with_cleanup(c_processx_wait, private$status,
                             as.integer(timeout))
```

Then include the `cleancall.h` header in the C code,  and use
`r_call_on_exit()` to push a cleanup handler to the stack of the
foreign call:

```
#include <cleancall.h>

[...]

static void processx__close_fd(void *ptr) {
  int *fd = ptr;
  if (*fd >= 0) close(*fd);
}

SEXP processx_wait(SEXP status, SEXP timeout) {
  processx_handle_t *handle = R_ExternalPtrAddr(status);

  [...]

  if (pipe(handle->waitpipe)) {
    processx__unblock_sigchld();
    error("processx error: %s", strerror(errno));
  }
  r_call_on_exit(processx__close_fd, handle->waitpipe);
  r_call_on_exit(processx__close_fd, handle->waitpipe + 1);

  [...]
}
```

You can see the [whole fix as a commit message on GitHub](https://github.com/r-lib/processx/commit/d05aadd4b0975a391d35a05958421f242bf96d23).

See also our blog post at
https://www.tidyverse.org/articles/2019/05/resource-cleanup-in-c-and-the-r-api/

Note that the cleanup functions cannot generally assume that
stack-allocated data are still around at the time they are called. This is
usually not a problem since cleanup is mostly about objects allocated on
the heap with non-automatic storage. If needed, you can protect
stack-allocated data from being unwound by using
`r_with_cleanup_context()`. This becomes the point at which cleanup
functions are called, which ensures any object allocated on the stack
before that point are still around.

## Usage

### `void r_call_on_exit(void (*fn)(void* data), void *data)`

Push an exit handler to the stack. This exit handler is always executed,
i.e. both on normal and early exits.

Exit handlers are executed right after the function called from
`call_with_cleanup()` exits. (Or the function used in
`r_with_cleanup_context()`, if the cleanup context was established from C.)

Exit handlers are executed in reverse order (last in is first out, LIFO).
Exit handlers pushed with `r_call_on_exit()` and `r_call_on_early_exit()`
share the same stack.

Best practice is to use this function immediately after acquiring a
resource, with the appropriate cleanup function for that resource.

### `void r_call_on_early_exit(void (*fn)(void* data), void *data)`

Push an exit handler to the stack. This exit handler is only executed
on early exists, _not_ on normal termination.

Exit handlers are executed right after the function called from
`call_with_cleanup()` exits. (Or the function used in
`r_with_cleanup_context()`, if the cleanup context was established from C.)

Exit handlers are executed in reverse order (last in is first out, LIFO).
Exit handlers pushed with `r_call_on_exit()` and `r_call_on_early_exit()`
share the same stack.

Best practice is to use this function immediately after acquiring a
resource, with the appropriate cleanup function for that resource.

### `SEXP r_with_cleanup_context(SEXP (*fn)(void* data), void* data)`

Establish a cleanup stack and call `fn` with `data`. This function can
be used to establish a cleanup stack from C code.

## Embedding cleancall

If you don't want to depend on the cleancall package, you can also easily
embed the cleancall code into your package. These are the steps that you
need to do:

1. Copy the `cleancall.R` file into your package, into the `R/` directory.
1. Copy the `cleancall.h` and `cleancall.c` files into your package,
   into `src/`.
1. If you have a `Makevars` and/or `Makevars.win` file, and you
   define `OBJECTS` there, add `cleancall.o` to `OBJECTS`.
1. Use the `CLEANCALL_METHOD_RECORD` macro in your registration of C
   functions. E.g.
   ```c
   #include "cleancall.h"
   [...]
   static const R_CallMethodDef callMethods[]  = {
     CLEANCALL_METHOD_RECORD,
     [...]
     { NULL, NULL, 0 }
   };
   ```
1. Add this call to your package init function:
   ```c
   cleancall_fns_dot_call = Rf_findVar(Rf_install(".Call"), R_BaseEnv);
   ```
1. Use `call_with_cleanup()` instead of `.Call()` for the C functions
   that you want to add cleanup code to.
1. Add the `r_call_on_exit()` etc. calls to your C function(s).

This is an example pull request that embeds cleancall into processx:
https://github.com/r-lib/processx/pull/238
(This pull request is slightly more complicated than a minimal example,
because it uses a wrapper to `.Call` already.)

## License

MIT @ [RStudio](https://github.com/rstudio)

Please note that the 'cleancall' project is released with a
[Contributor Code of Conduct](https://github.com/r-lib/cleancall/blob/master/.github/CODE_OF_CONDUCT.md). By
contributing to this project, you agree to abide by its terms.
