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
	typedef std::vector<uint64_t> uint64Vector;
	typedef std::vector<double> doubleVector;
	
	
	typedef std::vector<std::string> stringVector;
	typedef std::shared_ptr<std::string> stringP;
			
	/* cTypeBlock : Used to transform std::vectors (or similar) to classic c vectors.
	 * Here I could have used the std::pair. But I prefered to name
	 * the elements as firstValue and dim instead of first and second */
	template<typename T>
	struct cTypeBlock {
		T *firstValue;
		uint64_t dim = 0;
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
			
			uint64_t getSize () {
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
								
			uint64_t dimOfSignal;
			twoCTypeBlockPtrVector audioVector;
			
			void assertAccessor( uint64_t argDim ){
				assert ( argDim <= audioVector.size() );
			}	
					
		public:

			nTwoCTypeBlockAccessor(uint64_t dim) {
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
			
			uint64_t getDimOfSignal() {
				return this->dimOfSignal;
			}

			void setData(uint64_t dimOfSignal, twoCTypeBlockPtr toCopy) {
				this->assertAccessor ( dimOfSignal );
				this->audioVector[dimOfSignal]->first = toCopy->first;
				this->audioVector[dimOfSignal]->second = toCopy->second;
			}
			
			twoCTypeBlockPtr getTwoCTypeBlockAccessor ( uint64_t dimOfSignal ) {
				return this->audioVector[dimOfSignal];
			}
	};
};

#endif /* DATATYPE_HPP */
