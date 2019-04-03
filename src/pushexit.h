#ifndef PUSHEXIT_H
#define PUSHEXIT_H


SEXP r_with_exit_context(SEXP (*fn)(void* data), void* data);
void r_push_exit(void (*fn)(void* data), void* data);


#endif
