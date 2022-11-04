#include <thor_scsi/elements/element_helpers.h>
#include <thor_scsi/elements/elements_enums.h>
#include <thor_scsi/elements/utils.h>

#include <tps/tps_type.h>
#include <tps/tps.h>

namespace tse = thor_scsi::elements;
namespace tsc = thor_scsi::core;

// template tps sqr(const tps &);


double tse::get_psi(const double irho, const double phi, const double gap)
{

  double psi;
  const double phir = degtorad(phi);

  const double k1 = 0.5e0, k2 = 0e0;

  /* replace with local approximation */
  if (phi == 0e0){
	  psi = 0e0;
  } else {
	  psi = k1*gap*irho*(1e0+sqr(sin(phir)))/cos(phir)
		  *(1e0 - k2*gap*irho*tan(phir));
  }
  return psi;
}


void tse::get_twoJ(const int n_DOF, const ss_vect<double> &ps, const ss_vect<tps> &A,
	      double twoJ[])
{
  int             j, no;
  long int        jj[ps_dim];
  ss_vect<double> z;

  no = no_tps;
  danot_(1);

  for (j = 0; j < ps_dim; j++)
    jj[j] = (j < 2*n_DOF)? 1 : 0;

  z = (PInv(A, jj)*ps).cst();

  for (j = 0; j < n_DOF; j++)
    twoJ[j] = sqr(z[2*j]) + sqr(z[2*j+1]);

  danot_(no);
}


namespace thor_scsi::elements{

	template<typename T>
	void tse::drift_pass(const tsc::ConfigType &conf, const double L, ss_vect<T> &ps)
	{
		T u;

		if (!conf.H_exact) {
			// Small angle axproximation.
			u = L/(1e0+ps[delta_]);
			ps[x_]  += u*ps[px_]; ps[y_] += u*ps[py_];
			ps[ct_] += u*(sqr(ps[px_])+sqr(ps[py_]))/(2e0*(1e0+ps[delta_]));
		} else {
			u = L/tse::get_p_s(conf, ps);
			ps[x_]  += u*ps[px_]; ps[y_] += u*ps[py_];
			ps[ct_] += u*(1e0+ps[delta_]) - L;
		}
		if (conf.pathlength){
			ps[ct_] += L;
		}
	}

	/**
	 *
	 * The vector potential for the combined-function sector bend is from:
	 *  C. Iselin "Lie Transformations and Transport Equations for Combined-
	 * Function Dipoles" Part. Accel. 17, 143-155 (1985).
	 *
	 * Args:
	 *   order: optimisation for number of coefficients that require to be evaulatued
	 *          should go to field interpolation
	 *
	 *   h_bend: 1/rho_bend (radius of the the bend, magnetic rigidty of the dipoles)
	 *   h_ref: $1 / \rho$ for the reference curve / for the comoving frame (curvature of the design or reference orbit)
	 *
	 *   ps0 : initial condistins (when start propagation trough the element)
	 *   ps  : final condistions
	 *
	 * Todo:
	 *     Review kick and radiate code organisation ....
	 *
	 *     Split up function in different parts or functions
	 *     E.g.: one for dipoles and one for anything else...
	 */
	template<typename T>
	void tse::thin_kick(const tsc::ConfigType &conf, const T BxoBrho, const T ByoBrho,
			    const double L, const double h_bend, const double h_ref,
			    const ss_vect<T> &ps0,  ss_vect<T> &ps)
	{
		int        j;
		// T          BxoBrho, ByoBrho, ByoBrho1, B[3],
		T u, p_s;


		const int debug = false;

		// remains of calculation optimisation
		const int Order = 1;

		if(debug){
		    std::cerr << "Length " << L << " Order " << Order << " h_bend " << h_bend << " h_ref " << h_ref << std::endl
			      << "ps in " << ps;
		}
		// Todo: what kind of bend is that?
		if ((h_bend != 0e0) || ((1 <= Order) //... needs to be removed
#if 0
					//  optimisation of number
		    && (Order <= HOMmax)
#endif
			    )) {
			{
				// Warning: machinery there for non double
				// Need to understand
				if(debug){
					std::cerr << "interpolated field " << BxoBrho << ", "  << ByoBrho << std::endl;
				}

			}

			if (h_ref != 0e0) {
				// Sector bend.
				if (true) {
					// std::cerr << "h_bend - h_ref" << h_bend - h_ref << std::endl;
					ps[px_] -= L*(ByoBrho+(h_bend-h_ref)/2e0+h_ref*h_bend*ps0[x_]
						      -h_ref*ps0[delta_]);
					ps[ct_] += L*h_ref*ps0[x_];
					// std::cerr << "h_ref ps[px_]" <<  ps[px_] << " ps[ct_] " << ps[ct_]
					//	  << "ps0[delta_] " << ps0[delta_] << std::endl;
				} else {
				        /*
					 * Code idea  following Teapot symplectic integrator
					 *
					 * Currently not used ....
					 */

					// The Hamiltonian is split into: H_d + H_k; with [H_d, H_d] = 0.
					p_s = get_p_s(conf, ps0); u = L*h_ref*ps0[x_]/p_s;
					ps[x_]  += u*ps0[px_];
					ps[y_]  += u*ps0[py_];
					ps[ct_] += u*(1e0+ps0[delta_]);
					// ps[px_] -= L*(h_bend*(1e0+h_ref*ps0[x_])-h_ref*p_s);

#if 1
					// #warning "field interpolation for sector bends missing"
#else
					// Field expansion up to sextupole like terms.
					//const double B2 = MB[Quad+HOMmax];
					// need to understand it
					T Gy, Gx;
					intp.gradient(ps[x_], ps[y_], &Gx, &Gy);
					ByoBrho += h_bend - Gy *h_ref*sqr(ps0[y_])/2e0;
					ps[px_] -= L*((1e0+h_ref*ps0[x_])*ByoBrho-h_ref*p_s);
					ps[py_] += L*(1e0+h_ref*ps0[x_])*BxoBrho;
#endif
				}
			} else {
				// std::cerr << "Calculating horizontal thin kick: By"  << ByoBrho <<  " h_bend=1/rho_bend " << h_bend;
				// Cartesian bend.
				ps[px_] -= L*(h_bend+ByoBrho);
				// std::cerr << "ps[px_] " << ps[px_]  << std::endl;

			}
			ps[py_] += L*BxoBrho;
		}
	}

}

template void tse::drift_pass(const tsc::ConfigType &conf, const double, ss_vect<double> &);
template void tse::drift_pass(const tsc::ConfigType &conf, const double, ss_vect<tps> &);

template void tse::thin_kick(const tsc::ConfigType &conf, const double BxoBrho, const double ByoBrho,
			     const double L, const double h_bend, const double h_ref, const ss_vect<double> &ps0, ss_vect<double> &ps);
template void tse::thin_kick(const tsc::ConfigType &conf, const tps BxoBrho, const tps ByoBrho,
			     const double L, const double h_bend, const double h_ref, const ss_vect<tps> &ps0, ss_vect<tps> &ps);
