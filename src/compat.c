#define R_NO_REMAP
#include <Rinternals.h>

#include "compat.h"


#if (defined(R_VERSION) && R_VERSION < R_Version(3, 4, 0))
 SEXP R_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot) {
   fn_ptr ptr;
   ptr.fn = p;
   return R_MakeExternalPtr(ptr.p, tag, prot);
 }
 DL_FUNC R_ExternalPtrAddrFn(SEXP s) {
   fn_ptr ptr;
   ptr.p = EXTPTR_PTR(s);
   return ptr.fn;
 }
#endif


// The R API does not have a setter for function pointers

SEXP cleancall_MakeExternalPtrFn(DL_FUNC p, SEXP tag, SEXP prot)
{
    fn_ptr tmp;
    tmp.fn = p;
    return R_MakeExternalPtr(tmp.p, tag, prot);
}

void cleancall_SetExternalPtrAddrFn(SEXP s, DL_FUNC p)
{
    fn_ptr tmp;
    tmp.fn = p;
    R_SetExternalPtrAddr(s, p);
}
