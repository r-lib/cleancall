#define R_NO_REMAP
#include <Rinternals.h>

#include "cleancall.h"

struct cb_data {
  SEXP node;
  SEXP value;
};

static void cb(void* data_) {
  struct cb_data* data = (struct cb_data*) data_;
  SETCDR(data->node, Rf_cons(data->value, CDR(data->node)));
}

struct cb_data *make_data1(SEXP node) {
  struct cb_data *data1 =
    (struct cb_data*) R_alloc(1, sizeof(struct cb_data));
  data1->node = node;
  data1->value = Rf_install("first");
  return data1;
}

struct cb_data *make_data2(SEXP node) {
  struct cb_data *data2 =
    (struct cb_data*) R_alloc(1, sizeof(struct cb_data));
  data2->node = node;
  data2->value = Rf_install("second");
  return data2;
}

SEXP test_callback_return(SEXP node) {
  struct cb_data *data1 = make_data1(node);
  r_call_on_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_exit(cb, (void*) data2);

  return R_NilValue;
}

SEXP test_callback_jump(SEXP node) {
  struct cb_data *data1 = make_data1(node);
  r_call_on_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_exit(cb, (void*) data2);

  Rf_error("tilt");
} /* # nocov (we never get here) */

static void jumpy_cb(void* data_) {
  Rf_error("jump");
} /* # nocov (we never get here) */

SEXP test_jumpy_callback(SEXP node) {
  r_call_on_exit(jumpy_cb, NULL);

  struct cb_data *data1 = make_data1(node);
  r_call_on_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_exit(cb, (void*) data2);

  Rf_error("tilt");
} /* # nocov (we never get here)*/

SEXP test_no_callbacks(void) {
  return R_NilValue;
}

SEXP test_early_ok(SEXP node) {
  struct cb_data *data1 = make_data1(node);
  r_call_on_early_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_early_exit(cb, (void*) data2);

  return R_NilValue;
}

SEXP test_early_jump(SEXP node) {
  struct cb_data *data1 = make_data1(node);
  r_call_on_early_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_early_exit(cb, (void*) data2);

  Rf_error("jump");
} /* # nocov (we never get here) */

SEXP test_mixed(SEXP node) {
  struct cb_data *data1 = make_data1(node);
  r_call_on_exit(cb, (void*) data1);

  struct cb_data *data2 = make_data2(node);
  r_call_on_early_exit(cb, (void*) data2);

  return R_NilValue;
}
