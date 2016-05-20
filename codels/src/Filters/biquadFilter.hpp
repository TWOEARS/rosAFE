#ifndef BIQUADFILTER_HPP
#define BIQUADFILTER_HPP

#include "apf/biquad.h"
#include <assert.h>
#include <vector>
#include <iostream>
#include <memory>

namespace openAFE {

	template< typename T, typename T_a, typename T_b >
	class biquadFilter {
		
	protected:

		typedef std::shared_ptr< apf::Cascade< apf::BiQuad< T > > >  casFiltPtr;
		typedef std::shared_ptr< apf::BiQuad< T > >  biQuadPtr;
	
	    double fs;
	    float order;
	    float numFilters;
	    
		casFiltPtr casFilt;
		std::vector< apf::BiQuad< T > > allFilters;
			
		// http://audioprocessingframework.github.io/structapf_1_1SosCoefficients.html
		inline
		apf::SosCoefficients< T > createCoefMap(std::vector<T_a>& a, std::vector<T_b>& b) {
				
			assert ( a[0] == 1 );
				
			apf::SosCoefficients< T > coefs;
				
			coefs.b0 = b[0];
			coefs.b1 = b[1];
			coefs.b2 = b[2];
				
			coefs.a1 = a[1];
			coefs.a2 = a[2];
				
			return coefs;
		}

	public:
				
		biquadFilter( double fs, unsigned short argOrder = 1 ) {
			
			this->fs = fs;
			this->order = (float)argOrder;
			
			// We cascade numFilters filters
			this->numFilters = ceil(order/2);	
			casFilt.reset( new apf::Cascade< apf::BiQuad< T > > (numFilters) );
			
			// We place filters in a vector
			allFilters.resize(numFilters);
		}
			
		void filterChunk(T* sourceFront, T* sourceEnd, T* destFront) {
			casFilt->execute(sourceFront, sourceEnd, destFront);
		}			

		~biquadFilter() {
			// casFilt is a smart pointer
			allFilters.clear();
		}

	}; /* class Filter */
}; /* namespace openAFE */

#endif /* BIQUADFILTER_HPP */
