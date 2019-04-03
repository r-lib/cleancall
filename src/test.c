#define R_NO_REMAP
#include <Rinternals.h>

#include "pushexit.h"

struct cb_data {
  SEXP node;
  SEXP value;
};

static void cb(void* data_) {
  struct cb_data* data = (struct cb_data*) data_;
  SETCDR(data->node, Rf_cons(data->value, CDR(data->node)));
}

SEXP test_callback_return(SEXP node) {
  struct cb_data data1 = { node, Rf_install("first") };
  r_push_exit(cb, (void*) &data1);

  struct cb_data data2 = { node, Rf_install("second") };
  r_push_exit(cb, (void*) &data2);

  return R_NilValue;
}

SEXP test_callback_jump(SEXP node) {
  struct cb_data data1 = { node, Rf_install("first") };
  r_push_exit(cb, (void*) &data1);

  struct cb_data data2 = { node, Rf_install("second") };
  r_push_exit(cb, (void*) &data2);

  Rf_error("tilt");
}

static void jumpy_cb(void* data_) {
  Rf_error("jump");
}

SEXP test_jumpy_callback(SEXP node) {
  r_push_exit(jumpy_cb, NULL);

  struct cb_data data1 = { node, Rf_install("first") };
  r_push_exit(cb, (void*) &data1);

  struct cb_data data2 = { node, Rf_install("second") };
  r_push_exit(cb, (void*) &data2);

  Rf_error("tilt");
}

SEXP test_no_callbacks() {
  return R_NilValue;
}
