#ifndef _am_model_dirac_deltaxB_h
#define _am_model_dirac_deltaxB_h

#include <sundials/sundials_types.h>
#include <sundials/sundials_nvector.h>
#include <sundials/sundials_sparse.h>
#include <sundials/sundials_direct.h>

class UserData;
class ReturnData;
class TempData;
class ExpData;

int deltaxB_model_dirac(realtype t, int ie, N_Vector x, N_Vector xB, N_Vector xdot, N_Vector xdot_old, void *user_data, TempData *tdata);


#endif /* _am_model_dirac_deltaxB_h */
