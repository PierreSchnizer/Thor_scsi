#ifndef _THOR_SCSI_ELEMENTS_CLASSICAL_MAGNET_H_
#define _THOR_SCSI_ELEMENTS_CLASSICAL_MAGNET_H_

#include <thor_scsi/elements/mpole.h>
#include <thor_scsi/core/multipoles.h>

namespace thor_scsi::elements {
	/**
	 * @brief a magnet with a single strong multipole
	 *
	 * typically not directly used but using derived classes as Quadrupole, Sextupole or Bending
	 */
	class ClassicalMagnet : public MpoleType {
		/*
		 * no type name as not directly used
		 */
	public:
		inline ClassicalMagnet(const Config &config) : MpoleType(config){
			/*
			 * don't now how to access the multipole number derived by
			 *  a subclass
			 */
		}

		/**
		 *
		 * \verbatim embed:rst:leading-asterisk
		 *
		 * .. Todo::
		 *    Check if double function is appropriate ...
		 *
		 *
		 * \endverbatim
		 */
		inline const std::shared_ptr<thor_scsi::core::TwoDimensionalMultipoles> getMultipoles(void) const {
			auto tmp = this->getFieldInterpolator();
			if(!tmp){
				throw std::logic_error("Field interpolator NULL");
			}
			return tmp;
		}

		inline void setMultipoles(std::shared_ptr<thor_scsi::core::TwoDimensionalMultipoles> mul) {
			this->setFieldInterpolator(mul);
		}
		/*
		 * Put here as I am not getting it into the python interface in a straight forward fashion
		 */
		inline void setMultipoles(const std::vector<thor_scsi::core::cdbl_intern> coeffs) {
			auto muls = std::make_shared<thor_scsi::core::TwoDimensionalMultipoles>(thor_scsi::core::TwoDimensionalMultipoles(coeffs));
			this->setMultipoles(muls);
		}

		inline void setMainMultipoleStrength(const Config &config, const int n){
			this->getMultipoles()->setMultipole(n, config.get<double>("K"));
		}

		inline void setMainMultipoleStrength(const Config &config){
			const double K = config.get<double>("K");
			// Watch the apersand ...
			this->setMainMultipoleStrength(K);
		}

		inline void setMainMultipoleStrength(const thor_scsi::core::cdbl mul){
			const int n = this->getMainMultipoleNumber();
			this->getMultipoles()->setMultipole(n, mul);
		}

		inline void setMainMultipoleStrength(const double normal){
			this->setMainMultipoleStrength(thor_scsi::core::cdbl(normal, 0));
		}

		inline thor_scsi::core::cdbl getMainMultipoleStrength(void) const {
			auto n = this->getMainMultipoleNumber();
			return this->getMultipoles()->getMultipole(n);
		};

		/**
		 * get the major harmonic number
		 */
		virtual int getMainMultipoleNumber(void) const = 0;
		//thor_scsi::core::TwoDimensionalMultipoles* intp;


	};


} // Name space

#endif // _THOR_SCSI_ELEMENTS_CLASSICAL_MAGNET_H_
/*
 * Local Variables:
 * mode: c++
 * c-file-style: "python"
 * End:
 */
