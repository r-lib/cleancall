#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "cleancall.h"


extern SEXP cleancall_call(SEXP, SEXP);
extern SEXP test_callback_return(SEXP);
extern SEXP test_callback_jump(SEXP);
extern SEXP test_jumpy_callback(SEXP);
extern SEXP test_no_callbacks();
extern SEXP test_early_ok(SEXP);
extern SEXP test_early_jump(SEXP);
extern SEXP test_mixed(SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"ptr_call",               (DL_FUNC) &cleancall_call, 2},
  {"ptr_test_jump",          (DL_FUNC) &test_callback_jump, 1},
  {"ptr_test_return",        (DL_FUNC) &test_callback_return, 1},
  {"ptr_test_jumpy_cb",      (DL_FUNC) &test_jumpy_callback, 1},
  {"ptr_test_no_cb",         (DL_FUNC) &test_no_callbacks, 0},
  {"ptr_test_early_ok",      (DL_FUNC) &test_early_ok, 1},
  {"ptr_test_early_jump",    (DL_FUNC) &test_early_jump, 1},
  {"ptr_test_mixed",         (DL_FUNC) &test_mixed, 1},
  {NULL, NULL, 0}
};


SEXP fns_dot_call = NULL;

void R_init_cleancall(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);

  fns_dot_call = Rf_findVar(Rf_install(".Call"), R_BaseEnv);

  R_RegisterCCallable("cleancall", "r_with_cleanup_context",
                      (DL_FUNC) &r_with_cleanup_context);
  R_RegisterCCallable("cleancall", "r_call_on_exit",
                      (DL_FUNC) &r_call_on_exit);
  R_RegisterCCallable("cleancall", "r_call_on_early_exit",
                      (DL_FUNC) &r_call_on_early_exit);
}
