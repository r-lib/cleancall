#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


extern SEXP pushexit_invoke(SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"pushexit_invoke", (DL_FUNC) &pushexit_invoke, 2},
  {NULL, NULL, 0}
};


SEXP fns_dot_call = NULL;

void R_init_pushexit(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);

  fns_dot_call = Rf_findVar(Rf_install(".Call"), R_BaseEnv);
}
