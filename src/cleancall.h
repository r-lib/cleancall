#ifndef CLEANCALL_H
#define CLEANCALL_H


SEXP r_with_cleanup_context(SEXP (*fn)(void* data), void* data);
void r_call_on_exit(void (*fn)(void* data), void* data);
void r_call_on_early_exit(void (*fn)(void* data), void* data);

#endif
