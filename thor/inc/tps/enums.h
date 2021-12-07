#ifndef _TPS_ENUMS_H_
#define _TPS_ENUMS_H_ 1

const int
  max_str   = 132,
  n_m2      = 21,       // No of 1st & 2nd order moments: (6 over 5) + 6.
  ps_tr_dim = 4,        // Transverse phase-space dim.
  ps_dim    = 6,        // 6D phase-space dim.
  tps_n     = ps_dim+1; // 6D phase-space linear terms & cst.

// Spatial components.
enum spatial_ind { X_ = 0, Y_ = 1, Z_ = 2 };

// Phase-space components.
// (Note, e.g. spin components should be added here)
enum phase_space_ind { x_ = 0, px_ = 1, y_ = 2, py_ = 3, delta_ = 4, ct_ = 5 };


// Truncated Power Series Algebra (TPSA)
extern const int nv_tps, nd_tps, iref_tps;
extern int       no_tps, ndpt_tps;
extern double    eps_tps;

#endif
/*
 * Local Variables:
 * mode: c++
 * c-file-style: "python"
 * End:
 */
