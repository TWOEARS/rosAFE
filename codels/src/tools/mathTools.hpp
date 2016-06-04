#ifndef MATHTOOLS_HPP
#define MATHTOOLS_HPP

#define ERB_L 24.7
#define ERB_Q 9.265

#include <stdint.h>
#include <vector>
#include <math.h> /* M_PI, exp, log */

namespace openAFE {
		
		/* freq2erb - converts to frequencyscale erbscale.
		 * Uses a scaling based on the equivalent rectangular bandwidth
		 * (ERB) of an auditory filter at centre frequency fc:
		 * ERB(fc) = 24.7 + fc[Hz]/9.265 (Glasberg and Moore, JASA 1990). 
		 * 
		 *   freq = input vector in Hz
		 *   erb  = erbscaled output vector
		 * 
		 */
			 
		template<typename T = float>
		T freq2erb( T freq )  {
			return ERB_Q * log ( 1 + freq / ( ERB_L * ERB_Q ));
		}		 
		 
		template<typename T = float>
		void freq2erb(T* firstValue_freq, size_t dim, T* firstValue_erb)  {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue_erb + i ) = freq2erb ( *( firstValue_freq + i ) );
		}
		
		/* erb2freq - converts erbscale to frequencyscale
		 * 
		 * Look : freq2erb
		 * 
		 * */
		template<typename T = float>
		T erb2freq( T erb )  {
			return ERB_L * ERB_Q * ( exp( erb / ERB_Q ) - 1 );
		}
				 
		template<typename T = float>
		void erb2freq( T* firstValue_erb, size_t dim, T* firstValue_freq ) {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue_freq + i ) = erb2freq ( *( firstValue_erb + i ) );
		}
			
		/* conv - Convolution and polynomial multiplication.
		 * C = conv(A, B) convolves vectors A and B.  The resulting vector is
		 * length MAX([LENGTH(A)+LENGTH(B)-1,LENGTH(A),LENGTH(B)]).
		 * 
		 * Based On : http://toto-share.com/2011/11/cc-convolution-source-code/
		 * 
		 */		
		template<typename T = float>
		std::vector<T> conv( T* A, size_t lenA, T* B, size_t lenB ) {
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
	
		/* linspace - Linearly spaced vector.
		 * linspace(X1, X2, N) generates a std::vector of N linearly
		 * equally spaced points between X1 and X2.
		 * 
		 * Source : https://gist.github.com/jmbr/2375233
		 * 
         */
		template <typename T = float>
		std::vector<T> linspace(T a, T b, size_t N) {
		  T h = (b - a) / static_cast<T>(N-1);
		  std::vector<T> xs(N);
		  typename std::vector<T>::iterator x;
		  T val;
		  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
			*x = val;
		  return xs;
		}
		
}; /* namespace openAFE */


#endif /* MATHTOOLS_HPP */
