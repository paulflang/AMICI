
#include <include/symbolic_functions.h>
#include <include/amici.h>
#include <include/amici_model.h>
#include <string.h>
#include <include/tdata.h>
#include <include/udata.h>
#include "model_neuron_o2_w.h"

int sigma_z_model_neuron_o2(realtype t, int ie, TempData *tdata) {
int status = 0;
Model *model = (Model*) tdata->model;
UserData *udata = (UserData*) tdata->udata;
memset(tdata->sigmaz,0,sizeof(realtype)*5);
  tdata->sigmaz[0] = 1.0;
return(status);

}

