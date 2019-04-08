#ifndef R_CLEANCALL_H
#define R_CLEANCALL_H


static R_INLINE SEXP r_with_cleanup_context(SEXP (*fn)(void* data),
                                            void* data) {
  static SEXP (*fun)(SEXP (*func)(void*), void *data) = NULL;
  if (fun == NULL) {
    fun = (SEXP (*)(SEXP (*)(void *), void *))
      R_GetCCallable("cleancall", "r_with_cleanup_context");
  }
  return fun(fn, data);
}

static R_INLINE void r_call_on_exit(void (*fn)(void* data), void* data) {
  static void (*fun)(void (*func)(void*), void *data) = NULL;
  if (fun == NULL) {
    fun = (void (*)(void (*)(void *), void *))
      R_GetCCallable("cleancall", "r_call_on_exit");
  }
  fun(fn, data);
}

static R_INLINE void r_call_on_early_exit(void (*fn)(void* data),
                                          void* data) {
  static void (*fun)(void (*func)(void*), void *data) = NULL;
  if (fun == NULL) {
    fun = (void (*)(void (*)(void *), void *))
      R_GetCCallable("cleancall", "r_call_on_early_exit");
  }
  fun(fn, data);
}

#endif
