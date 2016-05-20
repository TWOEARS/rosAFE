 #include "gammatoneTools.hpp"

namespace openAFE {
	 
	template<typename T>
	void GTT::freq2erb(T* firstValue_freq, uint32_t dim, T* firstValue_erb) {
		for ( unsigned int i = 0 ; i < dim ; ++i )
			*( firstValue_erb + i ) = 24.7 + *( firstValue_freq + i ) / 9.265;
	}

	template<typename T>	
	void GTT::erb2freq(T* firstValue_erb, uint32_t dim, T* firstValue_freq) {
		for ( unsigned int i = 0 ; i < dim ; ++i )
			*( firstValue_freq + i ) = *( firstValue_erb + i ) * 9.265 - 24.7;
	}


	template<typename T>	
	std::vector<T> GTT::conv( T* A, T* B, uint32_t lenA, uint32_t lenB ) {
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
	
}; /* namespace openAFE */
