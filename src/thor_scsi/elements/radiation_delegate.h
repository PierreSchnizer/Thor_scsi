#ifndef _THOR_SCSI_RADIATION_DELEGATE_H_
#define _THOR_SCSI_RADIATION_DELEGATE_H_ 1
#include <thor_scsi/elements/radiation_delegate_api.h>
#include <thor_scsi/elements/constants.h>
#include <tps/tpsa_lin.h>
#include <array>

namespace thor_scsi::elements {
	class RadiationDelegate: public  RadiationDelegateInterface{
	public:
		inline RadiationDelegate(void){
			this->reset();
		}

		inline void reset(void) {
			this->curly_dH_x = 0e0;
		}

		inline double getCurlydHx(void) const {
			return this->curly_dH_x;
		}
		/*
		 * Used for computing curly_dHx
		 */
		virtual void view(const ElemType& kick, const ss_vect<double> &ps, const enum ObservedState state, const int cnt) override;
		virtual void view(const ElemType& kick, const ss_vect<tps> &ps, const enum ObservedState state, const int cnt) override;

		virtual void show(std::ostream& strm, int level) const override final;

		std::string getDelegatorName(void){
			return this->delegator_name;
		}
		int getDelegatorIndex(void){
			return this->delegator_index;
		}
	private:
		template<typename T>
		inline void _view(const ElemType&, const ss_vect<T> &ps, const enum ObservedState state, const int cnt);

		template<typename T>
		inline void computeAndStoreCurlyH(const ss_vect<T> &ps);

		double curly_dH_x = 0e0;
		std::string delegator_name = "";
		int delegator_index = -1;
	};

	class RadiationDelegateKick: public  RadiationDelegateKickInterface{
	public:
		RadiationDelegateKick(void){
			this->reset();
		}

		/*
		 * @brief: reset parameters for radiation
		 *
		 * passing ps to template the function. currently these functions are void
		 * (perhaps required later on)
		 */
		inline void reset(void) {
			this->curly_dH_x = 0e0;
			this->dI.fill({0e0});
			this->D_rad.fill({0e0});
			// this->dEnergy = 0e0;

		}

		// should be renamed to diffusion coefficient ...
		// add in coment that it is a prerequisite for emittance calculations
		inline void computeDiffusion(const bool flag){
			this->compute_diffusion = flag;
		}
		inline bool isComputingDiffusion() const {
			return this->compute_diffusion;
		}

		// See if not link to a global machine setting property
		void setEnergy(const double val);

		inline double getEnergy(void) const {
			return this->energy;
		}

		/*
		 * Used for computing synchrotron integrals
		 */
		virtual void view(const FieldKickAPI& kick, const ss_vect<double> &ps, const enum ObservedState state, const int cnt) override;
		virtual void view(const FieldKickAPI& kick, const ss_vect<tps> &ps, const enum ObservedState state, const int cnt) override;

		virtual void show(std::ostream& strm, int level) const override final;
		//virtual void view(const ElemType& kick, const ss_vect<double> &ps, const enum ObservedState state, const int cnt) override final;
		//virtual void view(const ElemType& kick, const ss_vect<tps> &ps, const enum ObservedState state, const int cnt) override final;
		/**
		 * @brief Radiation effect due to local field
		 *
		 * @f[
		 *     \frac{\mathrm{d}\delta}{\mathrm{d}(ds)} =
		 *           -C_{\gamma} \, E_0^3 \, (1 + \delta)^2 \, \left( \frac{B_{perp}}{B \rho}\right)^2
		 *                \frac{1}{2\pi}
		 * @f]
		 *
		 * @todo: depends on energy of ring .... currently taken from config ...
		 *
		 * M. Sands "The hysics of Electron Storage Rings" SLAC-121, p. 98.
		 */
		template<typename T>
		void radiate(const thor_scsi::core::ConfigType &conf, ss_vect<T> &x, const double L, const double h_ref, const std::array<T, 3> B);

		inline auto getSynchrotronIntegralsIncrement(void) const {
			return this->dI;
		}

		inline auto getDiffusionCoefficientsIncrement(void) const {
			return this->D_rad;
		}

		inline double getCurlydHx(void) const {
			return this->curly_dH_x;
		}

		std::string getDelegatorName(void) const {
			return this->delegator_name;
		}

		int getDelegatorIndex(void) const {
			return this->delegator_index;
		}

	private:
		//using RadiationDelegateKickInterface::view;

		/*
		 * @brief: finalise calculation of radiation integrals
		 *
		 * Applies appropriate balance to curly_H and integral 4.
		 * Calculates all other integrals
		 */
		//
		template<typename T>
		inline void synchrotronIntegralsFinish(const FieldKickAPI &kick, const ss_vect<T> &ps);

		// calculate the effect of radiation
		template<typename T>
		inline void //radiate
		synchrotronIntegralsStep(const ss_vect<T> &ps);

		template<typename T>
		inline void _view(const FieldKickAPI&, const ss_vect<T> &ps, const enum ObservedState state, const int cnt);

		inline void diffusion(const double B2, const double u, const double ps0, const ss_vect<double> &xp) { }
		inline void diffusion(const tps &B2_perp,  const tps &ds, const tps &p_s0,  const ss_vect<tps> &A);

		double curly_dH_x = 0e0;
		int index = -1;
		std::array<double, 6> dI;           ///< Local contributions to the synchrotron integrals
		std::array<double, 3> D_rad;        //< Diffusion coefficients (Floquet space).
		bool compute_diffusion = false;
		double energy = NAN;
		double q_fluct = NAN;

		std::string delegator_name = "";
		int delegator_index = -1;

	};
} // namespace thor_scsi::elements

#endif /* _THOR_SCSI_RADIATION_DELEGATOR_H_ */
/*
 * Local Variables:
 * mode: c++
 * c-file-style: "python"
 * End:
 */
