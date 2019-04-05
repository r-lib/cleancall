#define R_NO_REMAP
#include <Rinternals.h>

#include "cleancall.h"


// Initialised at load time with the `.Call` primitive
SEXP fns_dot_call;

struct eval_args {
  SEXP call;
  SEXP env;
};

static SEXP eval_wrap(void* data) {
  struct eval_args* args = (struct eval_args*) data;
  return Rf_eval(args->call, args->env);
}

SEXP cleancall_call(SEXP args, SEXP env) {
  SEXP call = PROTECT(Rf_lcons(fns_dot_call, args));
  struct eval_args data = { call, env };

  SEXP out = r_with_cleanup_context(&eval_wrap, &data);

  UNPROTECT(1);
  return out;
}
