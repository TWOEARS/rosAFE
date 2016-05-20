#ifndef AGCFILTER_HPP
#define AGCFILTER_HPP

#include "biquadFilter.hpp"
#include <math.h>       /* exp */

namespace openAFE {

	template<typename T>
	class AgcFilter : public biquadFilter < T, double, double > {
		
	public:
	
		// typedef std::shared_ptr<bwFilter<T> > bwFilterPtr;
	
		AgcFilter( double fs, double a_2 ) : biquadFilter < T, double, double > ( fs ) {
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);
			
			apf::BiQuad< T > *filter = new apf::BiQuad< T >;
			
			vectCcof[1] = 1;
			vectCcof[2] = -1 * exp( -1 / ( a_2 * fs ) );
			
			vectDcof[1] = vectCcof[1] + vectCcof[2];
									
			*filter = this->createCoefMap(vectDcof, vectCcof);
			
			// We place the last filter to the vector
			this->allFilters[this->allFilters.size()-1] = *filter;
			
			// This line creates the cascading
			this->casFilt->set(this->allFilters.begin(), this->allFilters.end());
		}
		
		~AgcFilter() {
		}
		
	};
};

#endif /* AGCFILTER_HPP */
