#define R_NO_REMAP
#include <Rinternals.h>

// Compats defined in init.c because the R API does not have a setter
// for external function pointers
SEXP pushexit_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot);
void pushexit_SetExternalPtrAddrFn(SEXP s, DL_FUNC p);


static SEXP callbacks = NULL;

// Preallocate a callback and push it on the stack
static void push_callback() {
  SEXP top = CDR(callbacks);

  SEXP fn_extptr = PROTECT(pushexit_MakeExternalPtrFn(NULL, R_NilValue, R_NilValue));
  SEXP data_extptr = PROTECT(R_MakeExternalPtr(NULL, R_NilValue, R_NilValue));
  SEXP cb = Rf_cons(Rf_cons(fn_extptr, data_extptr), top);

  SETCDR(callbacks, cb);

  UNPROTECT(2);
}


static void call_exits(void* data) {
  // Remove protecting node and preallocated callback
  SEXP top = CDDR(callbacks);

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
  push_callback();

  SEXP out = R_ExecWithCleanup(fn, data, &call_exits, (void*) old);

  UNPROTECT(1);
  return out;
}

void r_push_exit(void (*fn)(void* data), void* data) {
  if (!callbacks) {
    Rf_error("Internal error: Exit handler pushed outside of an exit context");
  }

  SEXP cb = CADR(callbacks);

  // Update pointers
  pushexit_SetExternalPtrAddrFn(CAR(cb), (DL_FUNC) fn);
  R_SetExternalPtrAddr(CDR(cb), data);

  // Preallocate the next callback in case the allocator jumps
  push_callback();
}
