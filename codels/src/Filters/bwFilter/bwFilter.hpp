#ifndef BWFILTER_HPP
#define BWFILTER_HPP

#include "../biquadFilter.hpp"
#include "bwCoef.hpp"


namespace openAFE {

	template<typename T>
	class bwFilter : public biquadFilter < T, double, double > {
		
	public:
	
		// typedef std::shared_ptr<bwFilter<T> > bwFilterPtr;
	
		bwFilter(double fs, unsigned short argOrder = 2, float f1 = 1000, bwType type = _bwlp, float f2 = 0) : biquadFilter < T, double, double > ( fs, argOrder ) {
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);

			// The coeffs of a second order filter
			bwCoef(2, fs, f1, vectDcof, vectCcof, type, f2);
			
			if ( this->numFilters > 1 ) {
				// We create filters until numFilters-1
				for( unsigned int i = 0 ; i < this->allFilters.size()-1; ++i ) {
				
					apf::BiQuad< T > *filter = new apf::BiQuad< T >;
					*filter = this->createCoefMap(vectDcof, vectCcof);
					this->allFilters[i] = *filter;
				}
			}
			
			// numFilter-1 can be biquiad or 1st order
			apf::BiQuad< T > *filter = new apf::BiQuad< T >;
			
			if ( (int)this->order % 2 == 1 ) {
				// Than we get the coeffs of a first order filter
				std::fill(vectDcof.begin(), vectDcof.end(), 0);
				std::fill(vectCcof.begin(), vectCcof.end(), 0);

				bwCoef(1, fs, f1, vectDcof, vectCcof, type, f2);
			}
			
			*filter = this->createCoefMap(vectDcof, vectCcof);
			
			// We place the last filter to the vector
			this->allFilters[this->allFilters.size()-1] = *filter;
			
			// This line creates the cascading
			this->casFilt->set(this->allFilters.begin(), this->allFilters.end());
		}
		
		~bwFilter() {
		}
		
	};
};

#endif /* BWFILTER_HPP */
