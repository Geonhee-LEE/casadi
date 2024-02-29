//
//    MIT No Attribution
//
//    Copyright (C) 2010-2023 Joel Andersson, Joris Gillis, Moritz Diehl, KU Leuven.
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy of this
//    software and associated documentation files (the "Software"), to deal in the Software
//    without restriction, including without limitation the rights to use, copy, modify,
//    merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//    permit persons to whom the Software is furnished to do so.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


// C-REPLACE "casadi_nlpsol_prob<T1>" "struct casadi_nlpsol_prob"
// C-REPLACE "casadi_nlpsol_data<T1>" "struct casadi_nlpsol_data"

// C-REPLACE "reinterpret_cast<int**>" "(int**) "
// C-REPLACE "reinterpret_cast<int*>" "(int*) "
// C-REPLACE "const_cast<int*>" "(int*) "

template<typename T1>
struct casadi_ipopt_prob {
  const casadi_nlpsol_prob<T1>* nlp;
  // Sparsity patterns
  const casadi_int *sp_h, *sp_a;

  Eval_F_CB eval_f;
  Eval_G_CB eval_g;
  Eval_Grad_F_CB eval_grad_f;
  Eval_Jac_G_CB eval_jac_g;
  Eval_H_CB eval_h;
};
// C-REPLACE "casadi_ipopt_prob<T1>" "struct casadi_ipopt_prob"

// SYMBOL "ipopt_setup"
template<typename T1>
void casadi_ipopt_setup(casadi_ipopt_prob<T1>* p) {

}



// SYMBOL "ipopt_data"
template<typename T1>
struct casadi_ipopt_data {
  // Problem structure
  const casadi_ipopt_prob<T1>* prob;
  // Problem structure
  casadi_nlpsol_data<T1>* nlp;

  IpoptProblem ipopt;

  const casadi_real** arg;
  casadi_real** res;
  casadi_int* iw;
  casadi_real* w;

  casadi_real *z_L, *z_U;

  enum ApplicationReturnStatus status;

  int return_status;

};
// C-REPLACE "casadi_ipopt_data<T1>" "struct casadi_ipopt_data"

// SYMBOL "ipopt_init_mem"
template<typename T1>
int ipopt_init_mem(casadi_ipopt_data<T1>* d) {
  return 0;
}

// SYMBOL "ipopt_free_mem"
template<typename T1>
void ipopt_free_mem(casadi_ipopt_data<T1>* d) {
  //Highs_destroy(d->ipopt);
}

// SYMBOL "ipopt_work"
template<typename T1>
void casadi_ipopt_work(const casadi_ipopt_prob<T1>* p, casadi_int* sz_arg, casadi_int* sz_res, casadi_int* sz_iw, casadi_int* sz_w) {
  casadi_nlpsol_work(p->nlp, sz_arg, sz_res, sz_iw, sz_w);

  *sz_w += p->nlp->nx; // z_L
  *sz_w += p->nlp->nx; // z_U
}

// SYMBOL "ipopt_init"
template<typename T1>
void casadi_ipopt_init(casadi_ipopt_data<T1>* d, const T1*** arg, T1*** res, casadi_int** iw, T1** w) {
  // Problem structure
  const casadi_ipopt_prob<T1>* p = d->prob;
  const casadi_nlpsol_prob<T1>* p_nlp = d->prob->nlp;
  const casadi_nlpsol_data<T1>* d_nlp = d->nlp;
  
  // Local variables
  casadi_int nnz_h, nnz_a;
  nnz_h = p->sp_h[2+p->sp_h[1]];
  nnz_a = p->sp_a[2+p->sp_a[1]];

  d->z_L = *w; *w += p_nlp->nx;
  d->z_U = *w; *w += p_nlp->nx;

  d->ipopt = CreateIpoptProblem(
                p_nlp->nx, (double *) d_nlp->lbx, (double *) d_nlp->ubx,
                p_nlp->ng, (double *) d_nlp->lbg, (double *) d_nlp->ubg,
                nnz_a, nnz_h, 0,
                p->eval_f, p->eval_g, p->eval_grad_f,
                p->eval_jac_g, p->eval_h);

  d->arg = *arg;
  d->res = *res;
  d->iw = *iw;
  d->w = *w;
}

// SYMBOL "ipopt_solve"
template<typename T1>
void casadi_ipopt_solve(casadi_ipopt_data<T1>* d) {
  // Problem structure
  const casadi_ipopt_prob<T1>* p = d->prob;
  const casadi_nlpsol_prob<T1>* p_nlp = p->nlp;
  const casadi_nlpsol_data<T1>* d_nlp = d->nlp;

  casadi_copy(d_nlp->x0, p_nlp->nx, d_nlp->x);
  d->status = IpoptSolve(d->ipopt, d_nlp->x, d_nlp->g, d_nlp->f, d_nlp->lam_x, d->z_L, d->z_U, d);

  // Get dual solution (simple bounds)
  for (casadi_int i=0; i<p_nlp->nx; ++i) {
    d_nlp->lam_g[i] = d->z_U[i]-d->z_L[i];
  }

}

// SYMBOL "ipopt_sparsity"
template<typename T1>
void casadi_ipopt_sparsity(const casadi_int* sp, ipindex *iRow, ipindex *jCol) {
    casadi_int ncol = sp[1];
    const casadi_int* colind = sp+2;
    const casadi_int* row = colind+ncol+1;

    for (casadi_int cc=0; cc<ncol; ++cc) {
        for (casadi_int el=colind[cc]; el<colind[cc+1]; ++el) {
            *iRow++ = row[el];
            *jCol++ = cc;
        }
    }
}