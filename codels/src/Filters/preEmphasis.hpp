#ifndef PREEMPHASIS_HPP
#define PREEMPHASIS_HPP

#include "biquadFilter.hpp"
#include <stdlib.h>		/* abs */

namespace openAFE {

	template<typename T>
	class PreEmphasis : public biquadFilter < T, double, double > {
		
	public:
	
		// typedef std::shared_ptr<bwFilter<T> > bwFilterPtr;
	
		PreEmphasis( double fs, double b_2 ) : biquadFilter < T, double, double > ( fs ) {
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);
			
			// numFilter-1 can be biquiad or 1st order
			apf::BiQuad< T > *filter = new apf::BiQuad< T >;
			
			vectDcof[0] = 1;
			vectCcof[0] = 1;
			vectCcof[1] = -1 * abs(b_2);
									
			*filter = this->createCoefMap(vectDcof, vectCcof);
			
			// We place the last filter to the vector
			this->allFilters[this->allFilters.size()-1] = *filter;
			
			// This line creates the cascading
			this->casFilt->set(this->allFilters.begin(), this->allFilters.end());
		}
		
		~PreEmphasis() {
		}
		
	};
};

#endif /* PREEMPHASIS_HPP */
