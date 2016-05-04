#ifndef BWFILTER_HPP
#define BWFILTER_HPP

#include <assert.h>
#include <vector>
#include <iostream>

#include "apf/biquad.h"
#include "bwCoef.hpp"

#include <memory>


namespace openAFE {

	template<typename T>
	class bwFilter {

	private:	
		apf::Cascade< apf::BiQuad< T > > *casFilt;
		
		// http://audioprocessingframework.github.io/structapf_1_1SosCoefficients.html
		inline
		apf::SosCoefficients< T > createCoefMap(std::vector<double>& a, std::vector<double>& b) {
			
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
	
		// typedef std::shared_ptr<bwFilter<T> > bwFilterPtr;
	
		bwFilter(double fs, unsigned short argOrder = 2, float f1 = 1000, bwType type = _bwlp, float f2 = 0) {
			
			float order = (float)argOrder;
			
			// We cascade numFilters filters
			float numFilters = ceil(order/2);	
			casFilt = new apf::Cascade< apf::BiQuad< T > > (numFilters);
			
			// We place filters in a vector
			std::vector< apf::BiQuad< T > > allFilters(numFilters);
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);

			
			// We tke the coeffs of a second order filter
			bwCoef(2, fs, f1, vectDcof, vectCcof, type, f2);
			
			if (numFilters > 1) {
				// We create filters until numFilters-1
				for(unsigned int i = 0 ; i < allFilters.size()-1; ++i) {
				
					apf::BiQuad< T > *filter = new apf::BiQuad< T >;
					*filter = createCoefMap(vectDcof, vectCcof);
					allFilters[i] = *filter;
				}
			}
			
			// numFilter-1 can be biquiad or 1st order
			apf::BiQuad< T > *filter = new apf::BiQuad< T >;
			
			if ( (int)order % 2 == 1 ) {
				// Than we get the coeffs of a first order filter
				std::fill(vectDcof.begin(), vectDcof.end(), 0);
				std::fill(vectCcof.begin(), vectCcof.end(), 0);

				bwCoef(1, fs, f1, vectDcof, vectCcof, type, f2);
			}
			
			*filter = createCoefMap(vectDcof, vectCcof);
			
			// We place the last filter to the vector
			allFilters[allFilters.size()-1] = *filter;
			
			// This line creates the cascading
			casFilt->set(allFilters.begin(), allFilters.end());
			
			std::cout << allFilters.size() << std::endl;
		}
		
		~bwFilter() {

			delete casFilt;
		}
		

		void filterChunk(T* sourceFront, T* sourceEnd, T* destFront) {
			casFilt->execute(sourceFront, sourceEnd, destFront);
		}
	
		
	};
};

#endif /* BWFILTER_HPP */
