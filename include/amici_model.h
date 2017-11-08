#ifndef AMICI_MODEL_H
#define AMICI_MODEL_H

#include <include/amici_exception.h>
#include <include/amici_defines.h>
#include <include/amici_vector.h>
#include <include/symbolic_functions.h>
#include <nvector/nvector_serial.h>
#include <sundials/sundials_direct.h>
#include <sundials/sundials_sparse.h>

#include <vector>

namespace amici {
    
    class UserData;
    class ReturnData;
    class ExpData;
    class Solver;
    extern msgIdAndTxtFp warnMsgIdAndTxt;
    
    
    /**
     * @brief The Model class represents an AMICI ODE model.
     * The model can compute various model related quantities based
     * on symbolically generated code.
     */
    class Model {
    public:
        /** default constructor */
        Model()
        : np(0), nk(0), nx(0), nxtrue(0), ny(0), nytrue(0), nz(0), nztrue(0),
        ne(0), nw(0), ndwdx(0), ndwdp(0), nnz(0), nJ(0), ubw(0), lbw(0),
        o2mode(AMICI_O2MODE_NONE) {}
        
        /** constructor with model dimensions
         * @param np number of parameters
         * @param nx number of state variables
         * @param nxtrue number of state variables of the non-augmented model
         * @param nk number of constants
         * @param ny number of observables
         * @param nytrue number of observables of the non-augmented model
         * @param nz number of event observables
         * @param nztrue number of event observables of the non-augmented model
         * @param ne number of events
         * @param nJ number of objective functions
         * @param nw number of repeating elements
         * @param ndwdx number of nonzero elements in the x derivative of the
         * repeating elements
         * @param ndwdp number of nonzero elements in the p derivative of the
         * repeating elements
         * @param nnz number of nonzero elements in Jacobian
         * @param ubw upper matrix bandwidth in the Jacobian
         * @param lbw lower matrix bandwidth in the Jacobian
         * @param o2mode second order sensitivity mode
         */
        Model(const int np, const int nx, const int nxtrue, const int nk,
              const int ny, const int nytrue, const int nz, const int nztrue,
              const int ne, const int nJ, const int nw, const int ndwdx,
              const int ndwdp, const int nnz, const int ubw, const int lbw,
              const AMICI_o2mode o2mode)
        : np(np), nk(nk), nx(nx), nxtrue(nxtrue), ny(ny), nytrue(nytrue),
        nz(nz), nztrue(nztrue), ne(ne), nw(nw), ndwdx(ndwdx), ndwdp(ndwdp),
        nnz(nnz), nJ(nJ), ubw(ubw), lbw(lbw), o2mode(o2mode), nplist(np),
        dJydy(nJ*nytrue*ny, 0.0),
        dJydp(nJ*nplist, 0.0),
        dJydsigma(nJ*nytrue*ny, 0.0),
        dJzdz(nJ*nztrue*nz, 0.0),
        dJzdp(nJ*nplist, 0.0),
        dJzdsigma(nJ*nztrue*nz, 0.0),
        dJrzdz(nJ*nztrue*nz, 0.0),
        dJrzdsigma(nJ*nztrue*nz, 0.0),
        dJydyTmp(nJ * ny, 0.0),
        dJydxTmp(nJ * nx, 0.0),
        dJydsigmaTmp(nJ * ny, 0.0),
        dJzdzTmp(nJ * nz, 0.0),
        dJzdxTmp(nJ * nx, 0.0),
        dJzdsigmaTmp(nJ * nz, 0.0),
        dJrzdsigmaTmp(nJ * nz, 0.0),
        y(ny, 0.0),
        x(nx, 0.0),
        z(nz, 0.0),
        rz(nz, 0.0),
        dzdx(nz*nx, 0.0),
        dzdp(nz*nplist, 0.0),
        drzdx(nz*nx, 0.0),
        drzdp(nz*nplist, 0.0),
        dydp(ny*nplist, 0.0),
        dydx(ny*nx,0.0),
        sigmay(ny, 0.0),
        dsigmaydp(ny*nplist, 0.0),
        sigmaz(nz, 0.0),
        dsigmazdp(nz*nplist, 0.0),
        dxdotdp(nx*nplist, 0.0),
        w(nw, 0.0),
        dwdx(ndwdx, 0.0),
        dwdp(ndwdp, 0.0),
        M(nx*nx, 0.0),
        stau(nplist, 0.0),
        deltax(nx, 0.0),
        deltasx(nx*nplist, 0.0),
        deltaxB(nx, 0.0),
        deltaqB(nJ*nplist, 0.0)
        {
            J = SparseNewMat(nx, nx, nnz, CSC_MAT);
        }
        
        /**
         * @brief Returns a UserData instance with preset model dimensions
         * @return The UserData instance
         */
        UserData getUserData() const;
        
        /**
         * @brief Returns a new UserData instance with preset model dimensions
         * @return The UserData instance
         */
        UserData *getNewUserData() const;
        
        /** Retrieves the solver object
         * @return Solver solver object @type Solver
         */
        virtual Solver *getSolver() { return NULL; }
        
        virtual void frootwrap(realtype t, AmiVector x, AmiVector dx, realtype *root,
                               const UserData *user_data) = 0;
        
        virtual void fxdotwrap(realtype t, AmiVector x, AmiVector dx, AmiVector xdot,
                               const UserData *user_data) = 0;
        
        virtual void fJwrap(realtype t, realtype cj, AmiVector x, AmiVector dx,
                              AmiVector xdot, DlsMat J, const UserData *user_data) = 0;
        
        virtual void fJSparsewrap(realtype t, realtype cj, AmiVector x, AmiVector dx,
                            AmiVector xdot, SlsMat J, const UserData *user_data) = 0;
        
        virtual void fJDiagwrap(realtype t, AmiVector Jdiag, realtype cj, AmiVector x,
                                AmiVector dx, const UserData *user_data) = 0;
        
        virtual void fdxdotdpwrap(realtype t, AmiVector x, AmiVector dx,
                                  const UserData *user_data) = 0;
        
        virtual void fJvwrap(realtype t, AmiVector x, AmiVector dx, AmiVector xdot,
                             AmiVector v, AmiVector nJv, realtype cj, const UserData *user_data) = 0;
        

        void fx0(AmiVector x, const UserData *udata);
        
        /** model specific implementation of fx0
         * param[out] sx0 initial state sensitivities
         * param[in] t initial time
         * param[in] x0 initial state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip parameter index
         **/
        virtual void model_x0(realtype *x0, const realtype t, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        /** Initial value for time derivative of states (only necessary for DAEs)
         * @param[in] x0 Vector with the initial states @type N_Vector
         * @param[out] dx0 Vector to which the initial derivative states will be
         *written (only DAE) @type N_Vector
         * @param[in] user_data object with model specifications @type TempData
         **/
        virtual void fdx0(AmiVector x0, AmiVector dx0, const UserData *udata) = 0;
        
        void fsx0(AmiVectorArray sx, const AmiVector x, const UserData *udata);
        
        /** model specific implementation of fsx0
         * param[out] sx0 initial state sensitivities
         * param[in] t initial time
         * param[in] x0 initial state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip sensitivity index
         **/
        virtual void model_sx0(realtype *sx0, const realtype t,const realtype *x0, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        
        /** Sensitivity of derivative initial states sensitivities sdx0 (only
         *necessary for DAEs)
         * @param[in] udata object with user input
         **/
        virtual void fsdx0(const UserData *udata) = 0;
        
        void fstau(const int ie,const realtype t, const AmiVector x, const AmiVectorArray sx, const UserData *udata);
        
        /** model specific implementation of fstau
         * param[out] stau total derivative of event timepoint
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] sx current state sensitivity
         * param[in] ip sensitivity index
         * param[in] ie event index
         **/
        virtual void model_stau(double *stau, const realtype t, const realtype *x, const realtype *p, const realtype *k, const realtype *sx, const int ip, const int ie) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fy(int it, ReturnData *rdata, const UserData *udata);
        
        /** model specific implementation of fy
         * param[out] y model output at current timepoint
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_y(double *y, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }

        void fdydp(const int it, ReturnData *rdata, const UserData *udata);
        
        /** model specific implementation of fdydp
         * param[out] dydp partial derivative of observables y w.r.t. model parameters p
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip parameter index w.r.t. which the derivative is requested
         **/
        virtual void model_dydp(double *dydp, const realtype t, const realtype *x, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdydx(const int it, ReturnData *rdata, const UserData *udata);
        
        /** model specific implementation of fdydx
         * param[out] dydx partial derivative of observables y w.r.t. model states x
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_dydx(double *dydx, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fz(const int nroots, const realtype t, const AmiVector x, ReturnData *rdata,
                        const UserData *udata);
        
        /** model specific implementation of fz
         * param[out] z value of event output
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_z(double *z, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fsz(const int nroots, const realtype t, const AmiVector x, const AmiVectorArray sx, ReturnData *rdata,
                         const UserData *udata);
        
        /** model specific implementation of fsz
         * param[out] sz Sensitivity of rz, total derivative
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] sx current state sensitivity
         * param[in] ip sensitivity index
         **/
        virtual void model_sz(double *sz, const realtype t, const realtype *x, const realtype *p, const realtype *k, const realtype *sx, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void frz(const int nroots, const realtype t, const AmiVector x, ReturnData *rdata,
                         const UserData *udata);
        
        /** model specific implementation of frz
         * param[out] rz value of root function at current timepoint (non-output events not included)
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_rz(double *rz, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fsrz(const int nroots, const realtype t, const AmiVector x, const AmiVectorArray sx, ReturnData *rdata,
                          const UserData *udata);
        
        /** model specific implementation of fsrz
         * param[out] srz Sensitivity of rz, total derivative
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] sx current state sensitivity
         * param[in] ip sensitivity index
         **/
        virtual void model_srz(double *srz, const realtype t, const realtype *x, const realtype *p, const realtype *k, const realtype *sx, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdzdp(const realtype t, const AmiVector x, const UserData *udata);
        
        /** model specific implementation of fdzdp
         * param[out] dzdp partial derivative of event-resolved output z w.r.t. model parameters p
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip parameter index w.r.t. which the derivative is requested
         **/
        virtual void model_dzdp(double *dzdp, const realtype t, const realtype *x, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdzdx(const realtype t, const AmiVector x, const UserData *udata);
        
        /** model specific implementation of fdzdx
         * param[out] dzdx partial derivative of event-resolved output z w.r.t. model states x
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_dzdx(double *dzdx, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdrzdp(const realtype t, const AmiVector x, const UserData *udata);
        
        /** model specific implementation of fdzdp
         * param[out] drzdp partial derivative of root output rz w.r.t. model parameters p
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip parameter index w.r.t. which the derivative is requested
         **/
        virtual void model_drzdp(double *drzdp, const realtype t, const realtype *x, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdrzdx(const realtype t, const AmiVector x, const UserData *udata);
        
        /** model specific implementation of fdrzdx
         * param[out] drzdx partial derivative of root output rz w.r.t. model states x
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_drzdx(double *drzdx, const realtype t, const realtype *x, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdeltax(const int ie, const realtype t, const AmiVector x,
                             const AmiVector xdot, const AmiVector xdot_old, const UserData *udata);
        
        /** model specific implementation of fdeltax
         * param[out] deltax state update
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ie event index
         * param[in] xdot new model right hand side
         * param[in] xdot_old previous model right hand side
         **/
        virtual void model_deltax(double *deltax, const realtype t, const realtype *x, const realtype *p, const realtype *k,
                                 const int ie, const realtype *xdot, const realtype *xdot_old) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdeltasx(const int ie, const realtype t, const AmiVector x, const AmiVectorArray sx,
                              const AmiVector xdot, const AmiVector xdot_old, const UserData *udata);
        
        /** model specific implementation of fdeltasx
         * param[out] deltasx sensitivity update
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip sensitivity index
         * param[in] ie event index
         * param[in] xdot new model right hand side
         * param[in] xdot_old previous model right hand side
         * param[in] sx state sensitivity
         * param[in] stau event-time sensitivity
         **/
        virtual void model_deltasx(double *deltasx, const realtype t, const realtype *x, const realtype *p, const realtype *k,
                                   const int ip, const int ie, const realtype *xdot, const realtype *xdot_old, const realtype *sx,
                                   const realtype *stau) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdeltaxB(const int ie, const realtype t, const AmiVector x, const AmiVector xB,
                              const AmiVector xdot, const AmiVector xdot_old, const UserData *udata);
        
        /** model specific implementation of fdeltaxB
         * param[out] deltaxB adjoint state update
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ie event index
         * param[in] xdot new model right hand side
         * param[in] xdot_old previous model right hand side
         * param[in] xB current adjoint state
         **/
        virtual void model_deltaxB(double *deltaxB, const realtype t, const realtype *x, const realtype *p, const realtype *k,
                                  const int ie, const realtype *xdot, const realtype *xdot_old, const realtype *xB) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdeltaqB(const int ie, const realtype t, const AmiVector x, const AmiVector xB,
                              const AmiVector xdot, const AmiVector xdot_old, const UserData *udata);
        
        /** model specific implementation of fdeltasx
         * param[out] deltasx sensitivity update
         * param[in] t current time
         * param[in] x current state
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] ip sensitivity index
         * param[in] ie event index
         * param[in] xdot new model right hand side
         * param[in] xdot_old previous model right hand side
         * param[in] xB adjoint state
         * param[in] qBdot right hand side of quadradature
         **/
        virtual void model_deltaqB(double *deltaqB, const realtype t, const realtype *x, const realtype *p, const realtype *k,
                                   const int ip, const int ie, const realtype *xdot, const realtype *xdot_old, const realtype *xB) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }

        void fsigma_y(const int it, const ExpData *edata, ReturnData *rdata,
                      const UserData *udata);
        
        /** model specific implementation of fsigmay
         * param[out] sigmay standard deviation of measurements
         * param[in] t current time
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_sigma_y(double *sigmay, const realtype t, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdsigma_ydp(const int it, const ReturnData *rdata, const UserData *udata);
        
        /** model specific implementation of fsigmay
         * param[out] dsigmaydp partial derivative of standard deviation of measurements
         * param[in] t current time
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_dsigma_ydp(double *dsigmaydp, const realtype t, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fsigma_z(const realtype t, const int ie, const int *nroots,
                      const ExpData *edata, ReturnData *rdata, const UserData *udata);
        
        /** model specific implementation of fsigmaz
         * param[out] sigmaz standard deviation of event measurements
         * param[in] t current time
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_sigma_z(double *sigmaz, const realtype t, const realtype *p, const realtype *k) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdsigma_zdp(const realtype t, const UserData *udata);
        
        /** model specific implementation of fsigmaz
         * param[out] dsigmazdp partial derivative of standard deviation of event measurements
         * param[in] t current time
         * param[in] p parameter vector
         * param[in] k constant vector
         **/
        virtual void model_dsigma_zdp(double *dsigmazdp, const realtype t, const realtype *p, const realtype *k, const int ip) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fJy(const int it, ReturnData *rdata, const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fJy
         * param[out] nllh negative log-likelihood for measurements y
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] y model output at timepoint
         * param[in] sigmay measurement standard deviation at timepoint
         * param[in] my measurements at timepoint
         **/
        virtual void model_Jy(double *nllh, const realtype *p, const realtype *k, const double *y, const double *sigmay, const double *my) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fJz(const int nroots, ReturnData *rdata, const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fJz
         * param[out] nllh negative log-likelihood for event measurements z
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] z model event output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         * param[in] mz event measurements at timepoint
         **/
        virtual void model_Jz(double *nllh, const realtype *p, const realtype *k, const double *z, const double *sigmaz, const double *mz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fJrz(const int nroots, ReturnData *rdata, const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fJrz
         * param[out] nllh regularization for event measurements z
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] z model event output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         **/
        virtual void model_Jrz(double *nllh, const realtype *p, const realtype *k, const double *z, const double *sigmaz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJydy(const int it, const ReturnData *rdata,
                    const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJydy
         * param[out] dJydy partial derivative of time-resolved measurement negative log-likelihood Jy
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] y model output at timepoint
         * param[in] sigmay measurement standard deviation at timepoint
         * param[in] my measurement at timepoint
         **/
        virtual void model_dJydy(double *dJydy, const realtype *p, const realtype *k,
                                 const double *y, const double *sigmay, const double *my) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJydsigma(const int it, const ReturnData *rdata,
                                const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJydsigma
         * param[out] dJydsigma Sensitivity of time-resolved measurement 
         * negative log-likelihood Jy w.r.t. standard deviation sigmay
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] y model output at timepoint
         * param[in] sigmaz measurement standard deviation at timepoint
         * param[in] my measurement at timepoint
         **/
        virtual void model_dJydsigma(double *dJydsigma, const realtype *p, const realtype *k,
                                 const double *y, const double *sigmay, const double *my) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJzdz(const int nroots, const ReturnData *rdata,
                    const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJzdz
         * param[out] dJzdz partial derivative of event measurement negative log-likelihood Jz
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] z model event output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         * param[in] mz event measurement at timepoint
         **/
        virtual void model_dJzdz(double *dJzdz, const realtype *p, const realtype *k,
                                 const double *z, const double *sigmaz, const double *mz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJzdsigma(const int nroots, const ReturnData *rdata,
                        const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJzdsigma
         * param[out] dJzdsigma Sensitivity of event measurement
         * negative log-likelihood Jz w.r.t. standard deviation sigmaz
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] z model event output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         * param[in] mz event measurement at timepoint
         **/
        virtual void model_dJzdsigma(double *dJzdsigma, const realtype *p, const realtype *k,
                                     const double *z, const double *sigmaz, const double *mz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJrzdz(const int nroots, const ReturnData *rdata,
                     const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJrzdz
         * param[out] dJrzdz partial derivative of event penalization Jrz
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] rz model root output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         **/
        virtual void model_dJrzdz(double *dJrzdz, const realtype *p, const realtype *k,
                                 const double *rz, const double *sigmaz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        void fdJrzdsigma(const int nroots,const ReturnData *rdata,
                         const UserData *udata, const ExpData *edata);
        
        /** model specific implementation of fdJrzdsigma
         * param[out] dJzdsigma Sensitivity of event penalization Jz w.r.t.
         * standard deviation sigmaz
         * param[in] p parameter vector
         * param[in] k constant vector
         * param[in] rz model root output at timepoint
         * param[in] sigmaz event measurement standard deviation at timepoint
         **/
        virtual void model_dJrzdsigma(double *dJrzdsigma, const realtype *p, const realtype *k,
                                     const double *rz, const double *sigmaz) {
            throw AmiException("Requested functionality is not supported as (%s) is not implemented for this model!",__func__);
        }
        
        virtual ~Model() {
            SparseDestroyMat(J);
        };
        
        // Generic implementations
        void fsy(const int it, ReturnData *rdata);
        
        void fsz_tf(const int ie, ReturnData *rdata);
        
        void fsJy(const int it, const std::vector<double> dJydx, ReturnData *rdata);
        
        void fdJydp(const int it, const ExpData *edata,
                   const ReturnData *rdata);
        
        void fdJydx(std::vector<double> dJydx, const int it, const ExpData *edata, const ReturnData *rdata);
        
        void fsJz(const int nroots, const std::vector<double> dJzdx, AmiVectorArray sx, const ReturnData *rdata);
        
        void fdJzdp(const int nroots, realtype t, const ExpData *edata, const ReturnData *rdata);
        
        void fdJzdx(std::vector<double> dJzdx, const int nroots, realtype t, const ExpData *edata, const ReturnData *rdata);
        
        void initialize(AmiVector x, AmiVector dx, std::vector<realtype> h, const UserData *udata);
        
        void initializeStates(AmiVector x, const UserData *udata);
        
        void initHeaviside(AmiVector x, AmiVector dx, std::vector<realtype> h, const UserData *udata);
        
        // Model dimensions 
        int nplist;
        /** total number of model parameters */
        const int np;
        /** number of fixed parameters */
        const int nk;
        /** number of states */
        const int nx;
        /** number of states in the unaugmented system */
        const int nxtrue;
        /** number of observables */
        const int ny;
        /** number of observables in the unaugmented system */
        const int nytrue;
        /** number of event outputs */
        const int nz;
        /** number of event outputs in the unaugmented system */
        const int nztrue;
        /** number of events */
        const int ne;
        /** number of common expressions */
        const int nw;
        /** number of derivatives of common expressions wrt x */
        const int ndwdx;
        /** number of derivatives of common expressions wrt p */
        const int ndwdp;
        /** number of nonzero entries in jacobian */
        const int nnz;
        /** dimension of the augmented objective function for 2nd order ASA */
        const int nJ;
        /** upper bandwith of the jacobian */
        const int ubw;
        /** lower bandwith of the jacobian */
        const int lbw;
        /** flag indicating whether for sensi == AMICI_SENSI_ORDER_SECOND
         * directional or full second order derivative will be computed */
        const AMICI_o2mode o2mode;
        /** index indicating to which event an event output belongs */
        const std::vector<int> z2event;
        /** flag array for DAE equations */
        const std::vector<realtype> idlist;
        
        /** data standard deviation */
        std::vector<double> sigmay;
        /** parameter derivative of data standard deviation */
        std::vector<double> dsigmaydp;
        /** event standard deviation */
        std::vector<double> sigmaz;
        /** parameter derivative of event standard deviation */
        std::vector<double> dsigmazdp;
        /** parameter derivative of data likelihood */
        std::vector<double> dJydp;
        /** parameter derivative of event likelihood */
        std::vector<double> dJzdp;
        
        /** change in x */
        std::vector<realtype> deltax;
        /** change in sx */
        std::vector<realtype> deltasx;
        /** change in xB */
        std::vector<realtype> deltaxB;
        /** change in qB */
        std::vector<realtype> deltaqB;
        
        /** tempory storage of dxdotdp data across functions */
        std::vector<realtype> dxdotdp;
        
    protected:
        int checkVals(const int N,const realtype *array, const char* fun){
            for(int idx = 0; idx < N; idx++) {
                if(amiIsNaN(array[idx])) {
                    warnMsgIdAndTxt("AMICI:mex:fJDiag:NaN","AMICI replaced a NaN value at index (%i) of (%i) in (%s)! Aborting simulation ... ",idx,N,fun);
                    return(AMICI_ERROR);
                }
                if(amiIsInf(array[idx])) {
                    warnMsgIdAndTxt("AMICI:mex:fJDiag:Inf","AMICI encountered an Inf value at index (%i) of (%i) in (%s)! Aborting simulation ... ",idx,N,fun);
                    return(AMICI_ERROR);
                }
            }
            return(AMICI_SUCCESS);
        }
        

        void fw(const realtype t, const N_Vector x, const UserData *udata);
        
        /** model specific implementation of fw
         * @param[out] w Recurring terms in xdot
         * @param[in] t timepoint
         * @param[in] x Vector with the states
         * @param[in] p parameter vector
         * @param[in] k constants vector
         */
        virtual void model_w(realtype *w, const realtype t, const realtype *x, const realtype *p,
                                const realtype *k) {};
        
        void fdwdp(const realtype t, const N_Vector x, const UserData *udata);
        
        /** model specific implementation of dwdp
         * @param[out] dwdp Recurring terms in xdot, parameter derivative
         * @param[in] t timepoint
         * @param[in] x Vector with the states
         * @param[in] p parameter vector
         * @param[in] k constants vector
         * @param[in] w vector with helper variables
         */
        virtual void model_dwdp(realtype *dwdp, const realtype t, const realtype *x, const realtype *p,
                                const realtype *k, const realtype *w) {};
        
        void fdwdx(const realtype t, const N_Vector x, const UserData *udata);
        
        /** model specific implementation of dwdx
         * @param[out] dwdx Recurring terms in xdot, state derivative
         * @param[in] t timepoint
         * @param[in] x Vector with the states
         * @param[in] p parameter vector
         * @param[in] k constants vector
         * @param[in] w vector with helper variables
         */
        virtual void model_dwdx(realtype *dwdx, const realtype t, const realtype *x, const realtype *p,
                                const realtype *k, const realtype *w) {};
        
    private:
        
        void getmy(const int it, const ExpData *edata);
        
        void gety(const int it, const ReturnData *rdata);
        
        void getx(const int it, const ReturnData *rdata);
        
        void getsx(const int it, const ReturnData *rdata);
        
        const realtype gett(const int it, const ReturnData *rdata) const;
        
        void getmz(const int nroots, const ExpData *edata);
        
        void getz(const int nroots, const ReturnData *rdata);
        
        void getrz(const int nroots, const ReturnData *rdata);

        /** storage for dJydy slice */
        std::vector<double> dJydyTmp;
        /** storage for dJydx slice */
        std::vector<double> dJydxTmp;
        /** storage for dJydsigma slice */
        std::vector<double> dJydsigmaTmp;
        /** storage for dJzdz slice */
        std::vector<double> dJzdzTmp;
        /** storage for dJzdx slice */
        std::vector<double> dJzdxTmp;
        /** storage for dJzdsigma slice */
        std::vector<double> dJzdsigmaTmp;
        /** storage for dJrzdsigma slice */
        std::vector<double> dJrzdsigmaTmp;
        
        /** Jacobian */
        SlsMat J;
        
        /** current state */
        std::vector<double> x;
        /** current state */
        std::vector<std::vector<double>> sx;
        /** current observable */
        std::vector<double> y;
        /** current observable measurement */
        std::vector<double> my;
        /** current event output */
        std::vector<double> z;
        /** current event measurement */
        std::vector<double> mz;
        /** current root output */
        std::vector<double> rz;
        /** observable derivative of data likelihood */
        std::vector<double> dJydy;
        /** observable sigma derivative of data likelihood */
        std::vector<double> dJydsigma;
        /** event ouput derivative of event likelihood */
        std::vector<double> dJzdz;
        /** event sigma derivative of event likelihood */
        std::vector<double> dJzdsigma;
        /** event ouput derivative of event likelihood at final timepoint */
        std::vector<double> dJrzdz;
        /** event sigma derivative of event likelihood at final timepoint */
        std::vector<double> dJrzdsigma;
        /** state derivative of event output */
        std::vector<double> dzdx;
        /** parameter derivative of event output */
        std::vector<double> dzdp;
        /** state derivative of event timepoint */
        std::vector<double> drzdx;
        /** parameter derivative of event timepoint */
        std::vector<double> drzdp;
        /** parameter derivative of observable */
        std::vector<double> dydp;
        /** state derivative of observable */
        std::vector<double> dydx;
        /** tempory storage of w data across functions */
        std::vector<realtype> w;
        /** tempory storage of dwdx data across functions */
        std::vector<realtype> dwdx;
        /** tempory storage of dwdp data across functions */
        std::vector<realtype> dwdp;
        /** tempory storage of M data across functions */
        std::vector<realtype> M;
        /** tempory storage of stau data across functions */
        std::vector<realtype> stau;
    };
    
} // namespace amici

#endif // MODEL_H
