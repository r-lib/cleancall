#ifndef CLEANCALL_COMPAT_H
#define CLEANCALL_COMPAT_H

#include <Rversion.h>
#include <R_ext/Rdynload.h>


#if (defined(R_VERSION) && R_VERSION < R_Version(3, 4, 0))
 typedef union {void* p; DL_FUNC fn;} fn_ptr;
 SEXP R_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot);
 DL_FUNC R_ExternalPtrAddrFn(SEXP s);
#endif

// The R API does not have a setter for external function pointers
SEXP cleancall_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot);
void cleancall_SetExternalPtrAddrFn(SEXP s, DL_FUNC p);


#endif
