#ifndef GAMMATONTOOLS_HPP
#define GAMMATONTOOLS_HPP

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
		void freq2erb(T* firstValue_freq, uint32_t dim, T* firstValue_erb);

		/* erb2freq - converts erbscale to frequencyscale
		 * 
		 * Look : freq2erb
		 * 
		 * */	
		template<typename T>
		void erb2freq(T* firstValue_erb, uint32_t dim, T* firstValue_freq);
		
		
		/* conv - Convolution and polynomial multiplication.
		 * C = conv(A, B) convolves vectors A and B.  The resulting vector is
		 * length MAX([LENGTH(A)+LENGTH(B)-1,LENGTH(A),LENGTH(B)]).
		 * 
		 * Based On : http://toto-share.com/2011/11/cc-convolution-source-code/
		 * 
		 */		
		template<typename T>
		std::vector<T> conv( T* A, T* B, unsigned int lenA, unsigned int lenB );
		
	}
	
	
	
}; /* namespace openAFE */


#endif /* GAMMATONTOOLS_HPP */
