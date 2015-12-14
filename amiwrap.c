/**
 * @file   amiwrap.c
 * @brief  core routines for mex interface
 *
 * This file defines the fuction mexFunction which is executed upon calling the mex file from matlab
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES /* MS definition of PI and other constants */
#include <math.h>
#ifndef M_PI /* define PI if we still have no definition */
#define M_PI 3.14159265358979323846
#endif
#include <mex.h>
#include "wrapfunctions.h" /* user functions */
#include <include/amici.h> /* amici functions */

/*!
 * mexFunction is the main function of the mex simulation file this function carries out all numerical integration and writes results into the sol struct.
 *
 * @param[in] nlhs number of output arguments of the matlab call @type int
 * @param[out] plhs pointer to the array of output arguments @type mxArray
 * @param[in] nrhs number of input arguments of the matlab call @type int
 * @param[in] prhs pointer to the array of input arguments @type mxArray
 * @return void
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    int ip, ix, jx, iy, it, ir; /* integers for indexing in loops */
    int jp; /* integers for indexing in loops */
    int ncheck; /* the number of (internal) checkpoints stored so far */
    
    void *ami_mem; /* pointer to cvodes memory block */
    UserData udata; /* user data */
    ReturnData rdata; /* return data */
    ExpData edata; /* experimental data */
    TempData tdata; /* temporary data */
    int status; /* general status flag */
    double *pstatus; /* return status flag */
    int cv_status; /* status flag returned by integration method */
    
    realtype tlastroot; /* storage for last found root */
    int nroots;
    int iroot;
    double tnext;
    
    bool rootflag, discflag;
    
    udata = setupUserData(prhs);
    if (udata == NULL) goto freturn;
    
    /* solution struct */
    
    if (!prhs[0]) {
        mexErrMsgTxt("No solution struct provided!");
    }
    
    /* options */
    if (!prhs[4]) {
        mexErrMsgTxt("No options provided!");
    }
    
    tdata = (TempData) mxMalloc(sizeof *tdata);
    if (tdata == NULL) goto freturn;
    
    ami_mem = setupAMI(&status, udata, tdata);
    if (ami_mem == NULL) goto freturn;
    
    rdata = setupReturnData(prhs, udata);
    if (rdata == NULL) goto freturn;
    
    edata = setupExpData(prhs, udata);
    if (edata == NULL) goto freturn;
    
    cv_status = 0;
    
    /* FORWARD PROBLEM */
    
    ncheck = 0; /* the number of (internal) checkpoints stored so far */
    
    t = tstart;
    
    if(ne>0) nroots = 0;
    if(ne>0) iroot = 0;
    
    tlastroot = 0;
    /* loop over timepoints */
    for (it=0; it < nt; it++) {
        if (cv_status == 0) {
            /* only integrate if no errors occured */
            if(ts[it] > tstart) {
                while (t<ts[it]) {
                    if(sensi_meth == AMI_ASA && sensi >= 1) {
                        cv_status = AMISolveF(ami_mem, RCONST(ts[it]), x, dx, &t, AMI_NORMAL, &ncheck);
                    } else {
                        cv_status = AMISolve(ami_mem, RCONST(ts[it]), x, dx, &t, AMI_NORMAL);
                    }
                    if (cv_status==AMI_ROOT_RETURN) {
                        cv_status = getEventOutput(&status, &tlastroot, &nroots, &iroot, ami_mem, udata, rdata, edata, tdata);
                        if (t==ts[it]) {
                            cv_status = 0;
                        }
                    }
                }
            }
            
            tsdata[it] = ts[it];
            x_tmp = NV_DATA_S(x);
            for (ix=0; ix<nx; ix++) {
                xdata[it+nt*ix] = x_tmp[ix];
            }
            
            if (it == nt-1) {
                if( sensi_meth == AMI_SS) {
                    status = fxdot(t,x,dx,xdot,udata);
                    if (status != AMI_SUCCESS) goto freturn;
                    
                    xdot_tmp = NV_DATA_S(xdot);
                    
                    status = fJ(nx,ts[it],0,x,dx,xdot,Jtmp,udata,NULL,NULL,NULL);
                    if (status != AMI_SUCCESS) goto freturn;
                    
                    memcpy(xdotdata,xdot_tmp,nx*sizeof(realtype));
                    memcpy(Jdata,Jtmp->data,nx*nx*sizeof(realtype));
                    
                    status = fdxdotdp(t,dxdotdpdata,x,udata);
                    if (status != AMI_SUCCESS) goto freturn;
                    status = fdydp(ts[it],it,dydpdata,x,udata);
                    if (status != AMI_SUCCESS) goto freturn;
                    status = fdydx(ts[it],it,dydxdata,x,udata);
                    if (status != AMI_SUCCESS) goto freturn;
                }
            }
            
            if(ts[it] > tstart) {
                getDiagnosis(&status, it, ami_mem, udata, rdata);
            }
            
        } else {
            for(ix=0; ix < nx; ix++) xdata[ix*nt+it] = mxGetNaN();
        }
    }
    
    if(cv_status == 0) {
        getDataOutput(&status, it, ami_mem, udata, rdata, edata, tdata);
    }
    
    /* fill events */
    if (ne>0) {
        fillEventOutput(&status, &nroots, &iroot, ami_mem, udata, rdata, edata, tdata);
    }
    
    if (sensi >= 1) {
        /* only set output sensitivities if no errors occured */
        if(sensi_meth == AMI_ASA) {
            if(cv_status == 0) {
                setupAMIB(&status, ami_mem, udata, tdata);
                
                it = nt-2;
                iroot = nroots-1;
                while (it>0 || iroot>0) {
                    /* enter while */
                    
                    
                    if (discs[iroot]>ts[it]) {
                        tnext = discs[iroot];
                    } else {
                        tnext = ts[it];
                    }
                    
                    cv_status = AMISolveB(ami_mem, tnext, AMI_NORMAL);
                    
                    status = AMIGetB(ami_mem, which, &t, xB, dxB);
                    if (status != AMI_SUCCESS) goto freturn;
                    status = AMIGetQuadB(ami_mem, which, &t, xQB);
                    if (status != AMI_SUCCESS) goto freturn;
                    
                    if (tnext == discs[iroot]) {
                        status = fdeltaqB(t,deltaqB,x,xB,xQB,udata);
                        if (status != AMI_SUCCESS) goto freturn;
                        status = fdeltaxB(t,deltaxB,x,xB,udata);
                        if (status != AMI_SUCCESS) goto freturn;
                        
                        xB_tmp = NV_DATA_S(xB);
                        for (ix=0; ix<nx; ix++) {
                            xB_tmp[ix] += deltaxB[ix];
                        }
                        
                        xQB_tmp = NV_DATA_S(xQB);
                        for (ip=0; ip<np; ip++) {
                            xQB_tmp[ip] += deltaqB[ip];
                        }
                        iroot--;
                    }
                    
                    if (tnext == ts[it]) {
                        xB_tmp = NV_DATA_S(xB);
                        for (ix=0; ix<nx; ix++) {
                            xB_tmp[ix] += dgdx[it+ix*nt];
                        }
                        getDiagnosisB(&status,it,ami_mem,udata,rdata,tdata);
                    }
                    
                    status = AMIReInitB(ami_mem, which, t, xB, dxB);
                    if (status != AMI_SUCCESS) goto freturn;
                    status = AMIQuadReInitB(ami_mem, which, xQB);
                    if (status != AMI_SUCCESS) goto freturn;
                    
                    status = AMICalcICB(ami_mem, which, tstart, xB, dxB);
                    if (status != AMI_SUCCESS) goto freturn;
                }
                if (t>tstart) {
                    if(cv_status == 0) {
                        if (nx>0) {
                            /* solve for backward problems */
                            cv_status = AMISolveB(ami_mem, tstart, AMI_NORMAL);
                            
                            status = AMIGetQuadB(ami_mem, which, &t, xQB);
                            if (status != AMI_SUCCESS) goto freturn;
                            status = AMIGetB(ami_mem, which, &t, xB, dxB);
                            if (status != AMI_SUCCESS) goto freturn;
                        }
                    }
                }
                
                /* evaluate initial values */
                sx = N_VCloneVectorArray_Serial(np,x);
                if (sx == NULL) goto freturn;
                
                status = fx0(x,udata);
                if (status != AMI_SUCCESS) goto freturn;
                status = fdx0(x,dx,udata);
                if (status != AMI_SUCCESS) goto freturn;
                status = fsx0(sx, x, dx, udata);
                if (status != AMI_SUCCESS) goto freturn;
                
                if(cv_status == 0) {
                    
                    xB_tmp = NV_DATA_S(xB);
                    
                    for (ip=0; ip<np; ip++) {
                        llhS0[ip] = 0.0;
                        sx_tmp = NV_DATA_S(sx[ip]);
                        for (ix = 0; ix < nx; ix++) {
                            llhS0[ip] = llhS0[ip] + xB_tmp[ix] * sx_tmp[ix];
                        }
                    }
                    
                    xQB_tmp = NV_DATA_S(xQB);
                    
                    for(ip=0; ip < np; ip++) {
                        llhSdata[ip] = - llhS0[ip] - dgdp[ip] - drdp[ip] - xQB_tmp[ip];
                    }
                    
                } else {
                    for(ip=0; ip < np; ip++) {
                        llhSdata[ip] = mxGetNaN();
                    }
                }
            } else {
                for(ip=0; ip < np; ip++) {
                    llhSdata[ip] = mxGetNaN();
                }
            }
        }
    }
    
    /* evaluate likelihood */
    
    *llhdata = - g - r;
    
    status = cv_status;
    
    goto freturn;
    
freturn:
    /* Free memory */
    if(nx>0) {
        N_VDestroy_Serial(x);
        N_VDestroy_Serial(dx);
        N_VDestroy_Serial(xdot);
        AMIFree(&ami_mem);
        DestroyMat(Jtmp);
        if (ne>0) {
            if(rootsfound) mxFree(rootsfound);
        }
        if (ne>0) {
            if(rootidx) mxFree(rootidx);
            if(sigma_z)    mxFree(sigma_z);
        }
        
        if(sigma_y)    mxFree(sigma_y);
        if (sensi >= 1) {
            N_VDestroyVectorArray_Serial(sx,np);
            if (sensi_meth == AMI_FSA) {
                N_VDestroyVectorArray_Serial(sdx, np);
            }
            if (sensi_meth == AMI_ASA) {
                if(dydx)    mxFree(dydx);
                if(dydp)    mxFree(dydp);
                if(dgdp)    mxFree(dgdp);
                if(dgdx)    mxFree(dgdx);
                if(drdp)    mxFree(drdp);
                if(drdx)    mxFree(drdx);
                if (ne>0) {
                    if(dzdp)    mxFree(dzdp);
                    if(dzdx)    mxFree(dzdx);
                }
                if(llhS0)     mxFree(llhS0);
                if(dsigma_ydp)    mxFree(dsigma_ydp);
                if (ne>0) {
                    if(dsigma_zdp)    mxFree(dsigma_zdp);
                }
                if(dxB)      N_VDestroy_Serial(dxB);
                if(xB)      N_VDestroy_Serial(xB);
                if(xQB)     N_VDestroy_Serial(xQB);
            }
            
        }
    }
    
    N_VDestroy_Serial(id);
    
    if(udata)    mxFree(udata);
    if(tdata)    mxFree(tdata);
    
    if(mxGetField(prhs[0], 0 ,"status")) { pstatus = mxGetPr(mxGetField(prhs[0], 0 ,"status")); } else { mexErrMsgTxt("Parameter status not specified as field in solution struct!"); }
    *pstatus = (double) status;
    
    return;
}