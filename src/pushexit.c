#define R_NO_REMAP
#include <Rinternals.h>


static SEXP callbacks = NULL;

static void call_exits(void* data) {
  // Remove protecting node
  SEXP top = CDR(callbacks);

  // Restore old stack
  callbacks = (SEXP) data;

  // Handlers should not jump
  while (top != R_NilValue) {
    SEXP cb = CAR(top);
    top = CDR(top);

    void (*fn)(void*) = (void (*)(void*)) R_ExternalPtrAddrFn(CAR(cb));
    void *data = (void*) EXTPTR_PTR(CDR(cb));
    fn(data);
  }
}

SEXP r_with_exit_context(SEXP (*fn)(void* data), void* data) {
  SEXP old = callbacks;
  callbacks = PROTECT(Rf_cons(R_NilValue, R_NilValue));

  SEXP out = R_ExecWithCleanup(fn, data, &call_exits, (void*) old);

  UNPROTECT(1);
  return out;
}

void r_push_exit(void (*fn)(void* data), void* data) {
  SEXP top = CDR(callbacks);

  // FIXME: Preallocate the callback in case the allocator jumps
  SEXP fn_extptr = PROTECT(R_MakeExternalPtrFn((DL_FUNC) fn, R_NilValue, R_NilValue));
  SEXP data_extptr = PROTECT(R_MakeExternalPtr(data, R_NilValue, R_NilValue));
  SEXP cb = Rf_cons(Rf_cons(fn_extptr, data_extptr), top);

  SETCDR(callbacks, cb);
  UNPROTECT(2);
}
