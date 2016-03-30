#ifndef DATATYPE_HPP
#define DATATYPE_HPP

#include <vector>
#include <memory>
#include <assert.h>
#include <iostream>
/*
 * This file contains the pre-defined macros for the openAFE.
 */
 
namespace openAFE {
	// Alias definitions
	typedef std::vector<unsigned int> uintVector;
	typedef std::vector<uint32_t> uint32Vector;
	typedef std::vector<double> doubleVector;
	
	
	typedef std::vector<std::string> stringVector;
	typedef std::shared_ptr<std::string> stringP;
			
	/* cTypeBlock : Used to transform std::vectors (or similar) to classic c vectors.
	 * Here I could have used the std::pair. But I prefered to name
	 * the elements as firstValue and dim instead of first and second */
	template<typename T>
	struct cTypeBlock {
		T *firstValue;
		uint32_t dim = 0;
	};
	
	/* twoCTypeBlock : This is needed to save the
	 * first and the second parts of the last chunk from a 
	 * circular buffer
	 * */
	template<typename T>
	class twoCTypeBlock {		
		public :
		
			typedef std::shared_ptr<twoCTypeBlock<T> > twoCTypeBlockPtr;
			typedef std::vector<twoCTypeBlockPtr> twoCTypeBlockPtrVector;
						
			cTypeBlock<T> *first;
			cTypeBlock<T> *second;
			
			twoCTypeBlock() {
				first = new cTypeBlock<T>;
				second = new cTypeBlock<T>;
			}
			
			~twoCTypeBlock() {
				delete first;
				delete second;
			}
			
			uint32_t getSize () {
				return first->dim + second->dim;
			}
			
	};

	/* nTwoCTypeBlockAccessor : A multidmentinal chunk may contains multiple twoCTypeBlock(s).
	 * This class contains them all.
	 * ATTENTION : This class do not contains the audio. It contains
	 * smart pointers to the audio. You have to point the pointers
	 * before using.
	 * */		
	template<typename T>
	class nTwoCTypeBlockAccessor {
		public:
			
			typedef typename std::shared_ptr<nTwoCTypeBlockAccessor<T> > nTwoCTypeBlockAccessorPtr;			
			typedef std::vector<nTwoCTypeBlockAccessorPtr >	 nTwoCTypeBlockAccessorPtrVector;
			typedef typename std::vector<nTwoCTypeBlockAccessorPtrVector >::iterator nTwoCTypeBlockAccessorPtrVectorIter;
			typedef typename std::vector<nTwoCTypeBlockAccessorPtrVector >::const_iterator nTwoCTypeBlockAccessorPtrVectorConstIter;
					
		private:
		
			using twoCTypeBlockPtr = typename twoCTypeBlock<T>::twoCTypeBlockPtr;
			using twoCTypeBlockPtrVector = typename twoCTypeBlock<T>::twoCTypeBlockPtrVector;
								
			uint32_t dimOfSignal;
			twoCTypeBlockPtrVector audioVector;
			
			void assertAccessor( uint32_t argDim ){
				assert ( argDim <= audioVector.size() );
			}	
					
		public:

			nTwoCTypeBlockAccessor(uint32_t dim) {
				/* dimOfSignal : the total number of vectors :
				 * example : signal is 44100 x 2 x 3
				 * dimOfSignal should be 6 (6 vectors of 44100 samples)
				 */
				audioVector.reserve( dim );
				this->dimOfSignal = dim;
				for (unsigned int i = 0 ; i < dimOfSignal ; ++i) {
					twoCTypeBlockPtr newSignal( new twoCTypeBlock<T>() );
					audioVector.push_back ( std::move ( newSignal ) );
				}
			}
			
			~nTwoCTypeBlockAccessor() {
				audioVector.clear();
			}
			
			uint32_t getDimOfSignal() {
				return this->dimOfSignal;
			}

			void setData(uint32_t dimOfSignal, twoCTypeBlockPtr toCopy) {
				this->assertAccessor ( dimOfSignal );
				this->audioVector[dimOfSignal]->first->firstValue = toCopy->first->firstValue;
				this->audioVector[dimOfSignal]->first->dim = toCopy->first->dim;				
				this->audioVector[dimOfSignal]->second->firstValue = toCopy->second->firstValue;
				this->audioVector[dimOfSignal]->second->dim = toCopy->second->dim;
			}
			
			twoCTypeBlockPtr getTwoCTypeBlockAccessor ( uint32_t dimOfSignal ) {
				return this->audioVector[dimOfSignal];
			}
	};
};

#endif /* DATATYPE_HPP */
