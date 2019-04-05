#ifndef CLEANCALL_H
#define CLEANCALL_H


SEXP r_with_cleanup_context(SEXP (*fn)(void* data), void* data);
void r_push_cleanup(void (*fn)(void* data), void* data);


#endif
