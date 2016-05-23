#ifndef GAMMATONTOOLS_HPP
#define GAMMATONTOOLS_HPP

#include <stdint.h>
#include <vector>

namespace openAFE {
	
	namespace GTT{

		/* freq2erb - converts to frequencyscale erbscale.
		 * Uses a scaling based on the equivalent rectangular bandwidth
		 * (ERB) of an auditory filter at centre frequency fc:
		 * ERB(fc) = 24.7 + fc[Hz]/9.265 (Glasberg and Moore, JASA 1990). 
		 * 
		 *   freq = input vector in Hz
		 *   erb  = erbscaled output vector
		 * 
		 */
		template<typename T>
		void freq2erb(T* firstValue_freq, uint32_t dim, T* firstValue_erb)  {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue_erb + i ) = 24.7 + *( firstValue_freq + i ) / 9.265;
		}

		/* erb2freq - converts erbscale to frequencyscale
		 * 
		 * Look : freq2erb
		 * 
		 * */	
		template<typename T>
		void erb2freq(T* firstValue_erb, uint32_t dim, T* firstValue_freq) {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue_freq + i ) = *( firstValue_erb + i ) * 9.265 - 24.7;
		}
		
		
		/* conv - Convolution and polynomial multiplication.
		 * C = conv(A, B) convolves vectors A and B.  The resulting vector is
		 * length MAX([LENGTH(A)+LENGTH(B)-1,LENGTH(A),LENGTH(B)]).
		 * 
		 * Based On : http://toto-share.com/2011/11/cc-convolution-source-code/
		 * 
		 */		
		template<typename T>
		std::vector<T> conv( T* A, T* B, uint32_t lenA, uint32_t lenB ) {
			int32_t nconv;
			int32_t j, i1;
			T tmp;
				 
			//allocated convolution array	
			nconv = lenA+lenB-1;
			std::vector<T > C ( nconv, 0 );
		 
			//convolution process
			for ( uint32_t i = 0 ; i < nconv; i++ ) {
				i1 = i;
				tmp = 0.0;
				for (j=0; j<lenB; j++)
				{
					if(i1>=0 && i1<lenA)
						tmp = tmp + (A[i1]*B[j]);
		 
					i1 = i1-1;
					C[i] = tmp;
				}
			}
		 
			//return convolution array
			return( C );
		}	
		
	}
	
	
	
}; /* namespace openAFE */


#endif /* GAMMATONTOOLS_HPP */
