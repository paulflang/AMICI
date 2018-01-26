
#include <include/symbolic_functions.h>
#include <include/amici_defines.h> //realtype definition
typedef amici::realtype realtype;
#include <cmath> 

void deltasx_model_events(double *deltasx, const realtype t, const realtype *x, const realtype *p, const realtype *k, const realtype *h, const realtype *w, const int ip, const int ie, const realtype *xdot, const realtype *xdot_old, const realtype *sx, const realtype *stau) {
switch (ip) {
  case 0: {
              switch(ie) { 
              case 2: {
  deltasx[2] = -stau[0]*(xdot[2]-xdot_old[2]);

              } break;

              case 3: {
  deltasx[0] = -stau[0]*(xdot[0]-xdot_old[0]);

              } break;

              } 

  } break;

  case 1: {
              switch(ie) { 
              case 2: {
  deltasx[2] = -stau[0]*(xdot[2]-xdot_old[2]);

              } break;

              case 3: {
  deltasx[0] = -stau[0]*(xdot[0]-xdot_old[0]);

              } break;

              } 

  } break;

  case 2: {
              switch(ie) { 
              case 2: {
  deltasx[2] = -stau[0]*(xdot[2]-xdot_old[2]);

              } break;

              case 3: {
  deltasx[0] = -stau[0]*(xdot[0]-xdot_old[0]);

              } break;

              } 

  } break;

  case 3: {
              switch(ie) { 
              case 2: {
  deltasx[2] = -stau[0]*(xdot[2]-xdot_old[2]);

              } break;

              case 3: {
  deltasx[0] = -stau[0]*(xdot[0]-xdot_old[0]);

              } break;

              } 

  } break;

}
}
