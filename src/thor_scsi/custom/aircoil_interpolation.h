#ifndef _THOR_SCSI_CUSTOM_AIRCOIL_INTERPOLATION_H_
#define _THOR_SCSI_CUSTOM_AIRCOIL_INTERPOLATION_H_ 1

#include <ostream>
#include <vector>
#include <gtpsa/utils.hpp>
#include <gtpsa/utils_tps.hpp>
#include <thor_scsi/core/field_interpolation.h>


namespace thor_scsi::custom {
	struct aircoil_filament {
		double x;
		double y;
		double current;

	    inline void show(std::ostream& strm, int level) const {
		strm << "x = " << x
		     << ", y = " << y
		     << ", current = " << current;
	    }
	};
	typedef struct aircoil_filament aircoil_filament_t;

	template<class C>
	class AirCoilMagneticFieldKnobbed : public thor_scsi::core::Field2DInterpolationKnobbed<C> {
	        const std::vector<aircoil_filament_t> m_filaments;
	        double m_scale;

	protected:
	    // subclasses will "multiply the number of filaments during initalisation"
	    inline void setFilaments(const std::vector<aircoil_filament_t> filaments){ this->m_filaments = filaments;}

	public:
	    AirCoilMagneticFieldKnobbed(const std::vector<aircoil_filament_t> filaments, const double scale=0e0)
			: m_filaments(filaments)
			, m_scale(scale)
			{}

		virtual ~AirCoilMagneticFieldKnobbed() {}

		void setScale(const double scale) { this->m_scale = scale; }
		const double getScale(void) const { return this->m_scale;  }

		// required for inspection: e.g. if python transfers filaments correctly
		inline std::vector<aircoil_filament_t> getUsedFilaments(void) const {
			return this->m_filaments;
		}


		/*
		 * equivalent python code
		 * r2 = np.sum(dpos ** 2, axis=0)
		 * dx, dy = dpos
		 * dpos_cross = np.array([dy, dx])
		 * B = self.precomp * 1 / r2 * dpos_cross
		 * B = np.sum(B, axis=1)
		 * field[0] = B[0]
		 * field[1] = B[1]
		 */
		template<typename T>
		inline void _field(const T& x, const T& y, T *pBx, T *pBy) const {
			const double mu0 = 4 * M_PI * 1e-7;
			const double precomp = mu0 / (2 * M_PI) * this->m_scale;

			/*
			 * better allocate new ones and copy back
			 * otherwise: for tpsa objects don't forget to clear them
			 * including gradients!
			 *
			 * new allocation does that for us ...
			 */
			T Bx = gtpsa::same_as_instance(*pBx);
			T By = gtpsa::same_as_instance(*pBx);

			for(const auto& f: this->m_filaments){
				const auto dx = x - f.x;
				const auto dy = y - f.y;
				const auto r2 = dx*dx + dy*dy;
				By += precomp * f.current / r2 * dx;
				Bx += precomp * f.current / r2 * dy;
			}

			*pBx = std::move(Bx);
			*pBy = std::move(By);

		}

		template<typename T>
		inline void _gradient(T& x, T& y, T *Gx, T *Gy){
		    std::runtime_error("air coil interpolation: gradient needs to be added");
		}
		inline void field(const double&      x, const double&      y, double      *Bx, double      *By) const override final
			{
				this->_field(x, y, Bx, By);
			}
		inline void field(const gtpsa::tpsa& x, const gtpsa::tpsa& y, gtpsa::tpsa *Bx, gtpsa::tpsa *By) const override final
			{
				this->_field(x, y, Bx, By);
            }
		inline void field(const tps& x, const tps& y, tps *Bx, tps *By) const override final
			{
				this->_field(x, y, Bx, By);
			}

		inline void gradient(const double& x, const double& y, double *Gx, double *Gy) const override final
			{ /* this->gradient(x, y, Gx, Gy); */ };

		inline void gradient(const tps& x, const tps& y, tps *Gx, tps *Gy) const override final
			{ /* this->gradient(x, y, Gx, Gy); */ };

		inline void gradient(const gtpsa::tpsa& x, const gtpsa::tpsa& y, gtpsa::tpsa *Gx, gtpsa::tpsa *Gy) const override final
			{ /* this->gradient(x, y, Gx, Gy); */ };

		inline void gradient(const tps& x, const tps& y, double *Gx, double *Gy) const override final
			{ /* this->gradient(x, y, Gx, Gy); */ };

		inline void gradient(const gtpsa::tpsa& x, const gtpsa::tpsa& y, double *Gx, double *Gy) const override final
			{ /* this->gradient(x, y, Gx, Gy); */ };

	        void show(std::ostream&, int level) const override;
	};

	typedef AirCoilMagneticFieldKnobbed<thor_scsi::core::StandardDoubleType> AirCoilMagneticField;
} //namespace thor_scsi::custom

#endif /* _THOR_SCSI_CUSTOM_AIRCOIL_INTERPOLATION_H_ */
