#ifndef GENERICFILTER_HPP
#define GENERICFILTER_HPP

#include <vector>
#include <algorithm> /* std::max */
#include <exception>
#include <string>
#include <iostream>

namespace openAFE {


	template< typename T_in = double, typename T_out = double, typename T_b = double, typename T_a = double >
	class GenericFilter {

	/* GenericFilter filters the data in vector X with the
	 * filter described by vectors A and B to create the filtered
	 * data Y. The filter is a "Direct Form II Transposed"
	 * implementation of the standard difference equation.
     */
     
	private:
		std::vector<T_b > vectB;
		std::vector<T_a > vectA;
		std::vector<T_out > states;
		uint32_t order;
		bool realTF = true;
		
	public:
		GenericFilter ( ) {
			
			this->order = 0;
		}

		GenericFilter ( const T_b* startB, const size_t lenB, const T_a* startA, const size_t lenA ) {

			this->setCoeff ( startB, lenB, startA, lenA );
		}

		virtual ~GenericFilter () {
			
			vectB.clear();
			vectA.clear();
			states.clear();					
		}
		
		void setCoeff ( const T_b* startB, const uint32_t lenB, const T_a* startA, const uint32_t lenA ) {
			
			size_t maxAB = std::max(lenA,lenB);
			this->order = maxAB - 1;
			
			if ( this->order > 0) {		
				this->vectB.resize( maxAB, 0 );
				this->vectA.resize( maxAB, 0 );
							
				for ( size_t i = 0 ; i < lenB ; i++ )
					this->vectB[i] = *( startB + i );
			
				for ( size_t i = 0 ; i < lenA ; i++ )
					this->vectA[i] = *( startA + i );

				/* If a(1) is not equal to 1, filter normalizes the filter
				 * coefficients by a(1).
				 */
				if ( *(startA) == (T_a)1 )
					for ( uint32_t i = 0 ; i < lenA ; i++ )
						vectA[i] /= vectA[0];
				
				this->states.resize( this->order, 0 );
			} else throw std::string("Filters order is zero.");
		}

		// a(1)*y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
        //             - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
		void exec ( const T_in* srcStart, const size_t lenSrc, T_out* destStart ) {
			if ( this->order > 0 ) {
				T_in exVar;			
				for ( size_t ii = 0 ; ii < lenSrc ; ++ii ) {
					
					// The srcStart and the destStart can point to the same adress.
					exVar = *( srcStart + ii );
					
					*( destStart + ii ) = exVar * vectB[0] + states[0];
					size_t jj;
					for ( jj = 0 ; jj < states.size() - 1 ; ++jj ) {
						states[ jj ] = states[ jj+1 ] + exVar * this->vectB[ jj+1 ] - *( destStart + ii ) * this->vectA[ jj+1 ];
					}
					states[ jj ] = exVar * this->vectB[ jj+1 ] - *( destStart + ii ) * this->vectA[ jj + 1 ];
				}
			} else throw std::string("Filters order is zero.");
		}

		void reset () {

			for ( size_t i = 0 ; i < states.size() ; i++ )
				this->states[i] = (T_out)0;
		}

		void reset ( const T_out* stateStart, const size_t lenStates ) {

			if ( this->order == lenStates ) {
				for ( size_t i = 0 ; i < lenStates ; i++ )
					this->states[i] = *( stateStart + i );
			} else throw std::string("Wrong length of states.");		
		}
		
		const bool isInitialized () {

			for ( unsigned int i = 0 ; i < this->states.size() ; ++i ) {
				if ( this->states[ i ] != (T_out)0 )
					return true;
			} return false;
		}
		
		const uint32_t getOrder () noexcept {
			return this->order;
		}

		void setRealTF ( const bool realTF_Arg ) noexcept {
			this->realTF = realTF_Arg;
		}
		
		const bool getRealTF () noexcept {
			return this->realTF;
		}
		
		const std::vector<T_out > getStates () noexcept {
			return this->states;
		}
		
	}; /* class GenericFilter */
}; /* namespace openAFE */

#endif /* GENERICFILTER_HPP */
