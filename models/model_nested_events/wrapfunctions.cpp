#include <include/amici_model.h>
#include "wrapfunctions.h"

std::unique_ptr<amici::Model> getModel(const amici::UserData *udata) {
    return std::unique_ptr<amici::Model>(new Model_model_nested_events(udata));
}

void getModelDims(int *nx, int *nk, int *np) {
    if(nx)
        *nx = 1;
    if(nk)
        *nk = 0;
    if(np)
        *np = 5;
}
