#ifndef R_CLEANCALL_H
#define R_CLEANCALL_H

#include <R_ext/Rdynload.h>

static R_INLINE SEXP r_with_cleanup_context(SEXP (*fn)(void* data),
                                            void* data) {
  static SEXP (*cleancall_ptr)(SEXP (*fn)(void*), void *data) = NULL;
  if (cleancall_ptr == NULL) {
    cleancall_ptr = (SEXP (*)(SEXP (*)(void *), void *))
      R_GetCCallable("cleancall", "r_with_cleanup_context");
  }
  return cleancall_ptr(fn, data);
}

static R_INLINE void r_call_on_exit(void (*fn)(void* data), void* data) {
  static void (*cleancall_ptr)(void (*fn)(void*), void *data) = NULL;
  if (cleancall_ptr == NULL) {
    cleancall_ptr = (void (*)(void (*)(void *), void *))
      R_GetCCallable("cleancall", "r_call_on_exit");
  }
  cleancall_ptr(fn, data);
}

static R_INLINE void r_call_on_early_exit(void (*fn)(void* data),
                                          void* data) {
  static void (*cleancall_ptr)(void (*fn)(void*), void *data) = NULL;
  if (cleancall_ptr == NULL) {
    cleancall_ptr = (void (*)(void (*)(void *), void *))
      R_GetCCallable("cleancall", "r_call_on_early_exit");
  }
  cleancall_ptr(fn, data);
}

#endif
