
#include <include/symbolic_functions.h>
#include <string.h>
#include <include/udata.h>
#include "model_neuron_w.h"

int dwdx_model_neuron(realtype t, N_Vector x, N_Vector dx, void *user_data) {
int status = 0;
UserData *udata = (UserData*) user_data;
realtype *x_tmp = N_VGetArrayPointer(x);
memset(udata->dwdx,0,sizeof(realtype)*0);
status = w_model_neuron(t,x,NULL,user_data);
return(status);

}


