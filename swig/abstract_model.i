%module abstract_model

%ignore fJ;
%ignore fJDiag;
%ignore fJSparse;
%ignore fJv;
%ignore fJB;
%ignore fJSparseB;
%ignore fxBdot_ss;
%ignore fJSparseB_ss;
%ignore writeSteadystateJB;
%ignore fdx0;
%ignore fdxdotdp;
%ignore froot;
%ignore fsxdot;
%ignore fxdot;
%ignore fdwdw;
%ignore fdwdw_rowvals;
%ignore fdwdw_colptrs;
%ignore fdwdp;
%ignore fdwdp_rowvals;
%ignore fdwdp_colptrs;
%ignore fdwdx;
%ignore fdwdx_rowvals;
%ignore fdwdx_colptrs;
%ignore fdJydy;
%ignore fdJydy_colptrs;
%ignore fdJydy_rowvals;
%ignore fJrz;
%ignore fJy;
%ignore fJz;
%ignore fdJrzdsigma;
%ignore fdJrzdz;
%ignore fdJzdsigma;
%ignore fdJzdz;
%ignore fdJydsigma;
%ignore fdeltaqB;
%ignore fdeltasx;
%ignore fdeltax;
%ignore fdeltaxB;
%ignore fdrzdp;
%ignore fdrzdx;
%ignore fdsigmaydp;
%ignore fdsigmazdp;
%ignore fdydp;
%ignore fdydx;
%ignore fdzdp;
%ignore fdzdx;
%ignore frz;
%ignore fsdx0;
%ignore fsigmay;
%ignore fsigmaz;
%ignore fsrz;
%ignore fstau;
%ignore fsz;
%ignore fw;
%ignore fy;
%ignore fz;
%ignore fdwdp;
%ignore fdwdx;
%ignore fx0;
%ignore fx0_fixedParameters;
%ignore fsx0;
%ignore fsx0_fixedParameters;
%ignore fdx_rdatadtcl_colptrs;
%ignore fdx_rdatadtcl_rowvals;
%ignore fdx_rdatadx_solver_colptrs;
%ignore fdx_rdatadx_solver_rowvals;
%ignore fdtotal_cldx_rdata_colptrs;
%ignore fdtotal_cldx_rdata_rowvals;
%include "amici/abstract_model.h"
