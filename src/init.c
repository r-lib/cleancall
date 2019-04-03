#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


extern SEXP pushexit_invoke(SEXP, SEXP);

extern SEXP test_callback_return(SEXP);
extern SEXP test_callback_jump(SEXP);
extern SEXP test_jumpy_callback(SEXP);
extern SEXP test_no_callbacks();

static const R_CallMethodDef CallEntries[] = {
  {"pushexit_invoke", (DL_FUNC) &pushexit_invoke, 2},

  {"ptr_jump",        (DL_FUNC) &test_callback_jump, 1},
  {"ptr_return",      (DL_FUNC) &test_callback_return, 1},
  {"ptr_jumpy_cb",    (DL_FUNC) &test_jumpy_callback, 1},
  {"ptr_no_cb",       (DL_FUNC) &test_no_callbacks, 0},

  {NULL, NULL, 0}
};


SEXP fns_dot_call = NULL;

void R_init_pushexit(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);

  fns_dot_call = Rf_findVar(Rf_install(".Call"), R_BaseEnv);
}


// Compatiblity - R API does not have a setter for function pointers

typedef union { void *p; DL_FUNC fn; } fn_ptr;

SEXP pushexit_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot)
{
    fn_ptr tmp;
    tmp.fn = p;
    return R_MakeExternalPtr(tmp.p, tag, prot);
}

DL_FUNC pushexit_ExternalPtrAddrFn(SEXP s)
{
    fn_ptr tmp;
    tmp.p =  EXTPTR_PTR(s);
    return tmp.fn;
}

void pushexit_SetExternalPtrAddrFn(SEXP s, DL_FUNC p)
{
    fn_ptr tmp;
    tmp.fn = p;
    R_SetExternalPtrAddr(s, p);
}
