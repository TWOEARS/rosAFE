#ifndef GENERICFILTER_HPP
#define GENERICFILTER_HPP

#include "../Filter.hpp"
#include "bwCoef.hpp"


namespace openAFE {

	template< typename T, typename T_a, typename T_b >
	class genericFilter : public biquadFilter < T, T_a, T_b > {
		
	public:
	
		// typedef std::shared_ptr<bwFilter<T> > bwFilterPtr;
	
		genericFilter( std::vector<T_a> vectDcof /* a */ , std::vector<T_b> vectCcof /* b */, double fs, unsigned short argOrder = 1 ) : biquadFilter < T, double, double > ( fs, argOrder ) {
			
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
		
		~genericFilter() {
		}
		
	};
};

#endif /* GENERICFILTER_HPP */
