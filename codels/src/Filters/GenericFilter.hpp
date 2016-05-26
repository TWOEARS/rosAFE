#include <vector>
#include <algorithm> /* std::max */

namespace openAFE {


	template< typename T, typename T_ab >
	class GenericFilter {

	/* Y = filter(B,A,X) filters the data in vector X with the
	 * filter described by vectors A and B to create the filtered
	 * data Y.  The filter is a "Direct Form II Transposed"
	 * implementation of the standard difference equation.
     */
     
	private:

		std::vector<T_ab > vectB, vectA;
		
		std::vector<T > states;
		
		uint32_t order;
		
	public:
		GenericFilter ( const T_ab* startB, const uint32_t lenB, const T_ab* startA, const uint32_t lenA ) {

			this->order = std::max(lenA,lenB) - 1;
						
			this->vectB.resize( this->order, 0 );
			this->vectA.resize( this->order, 0 );
						
			for ( uint32_t i = 0 ; i < lenB ; i++ )
				this->vectB[i] = *( startB + i );
		
			for ( uint32_t i = 0 ; i < lenA ; i++ )
				this->vectA[i] = *( startA + i );

			/* If a(1) is not equal to 1, filter normalizes the filter
			 * coefficients by a(1).
			 */			
			if ( *(startA) == 1 )
				for ( uint32_t i = 0 ; i < lenA ; i++ )
					vectA[i] /= vectA[0];
			
			this->states.resize( this->order, 0 );
		}

		~GenericFilter () {
			vectB.clear();
			vectA.clear();
			states.clear();					
		}
		
		bool reset ( const T* stateStart, const uint32_t lenStates ) {

			if ( this->order == lenStates ) {
				for ( uint32_t i = 0 ; i < lenStates ; i++ )
					this->states[i] = *( stateStart + i );
				return true;	
			} else return false;			
		}
		
		const bool isInitialized () {

			for ( unsigned int i = 0 ; i < this->states.size() ; ++i ) {
				if ( this->states[ i ] != 0 )
					return true;
			} return false;
		}
		
		// a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
        //             - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
		void exec ( const T* srcStart, const uint32_t lenSrc, T* destStart ) {

			T exVar;			
			for ( uint32_t ii = 0 ; ii < lenSrc ; ++ii ) {
				
				exVar = *( srcStart + ii );    // Thanks to this line the srcStart and the destStart can be same
				*( destStart + ii ) = exVar * vectB[0] + states[0];
				uint32_t jj;
				for ( jj = 0 ; jj < states.size() - 1 ; ++jj ) {
					states[ jj ] = states[ jj+1 ] + exVar * this->vectB[ jj+1 ] - *( destStart + ii ) * this->vectA[ jj+1 ];
				}
				states[ jj ] = exVar * this->vectB[ jj+1 ] - *( destStart + ii ) * this->vectA[ jj + 1 ];				
			}
		}

		const uint32_t getOrder () {
			return this->order;
		}
		
	}; /* class GenericFilter */
}; /* namespace openAFE */
