#ifndef DATATYPE_HPP
#define DATATYPE_HPP

#include <vector>
#include <complex>
#include <memory>
#include <assert.h>

/*
 * This file contains the pre-defined macros for the openAFE.
 */
 
namespace openAFE {
	// Alias definitions
	typedef std::vector<unsigned int> uintVector;
	typedef std::vector<uint64_t> uint64Vector;
	typedef std::vector<double> doubleVector;
	
	typedef std::complex<double> doubleComplex;
	typedef std::vector<doubleComplex > doubleComplexVector;
	
	typedef std::vector<std::string> stringVector;
	typedef std::shared_ptr<std::string> stringP;
			
	/* Used to transform std::vectors (or similar) to classic c vectors.
	 * Here I could have used the std::pair. But I prefered to name
	 * the elements as firstValue and dim instead of first and second */
	template<typename T>
	struct cTypeBlock {
		T *firstValue;
		uint64_t dim;
	};

	/* A multidmentinal chunk may contains multiple cTypeBlock(s).
	 * This class contains them all.
	 * ATTENTION : This class do not contains the audio. It contains
	 * smart pointers to the audio. You have to point the pointers
	 * before using.
	 * */
	template<typename T>
	class nDimAudioChunkAccessor {
		private:
		
			typedef std::unique_ptr<cTypeBlock<T> > cTypeBlockPtr;
			typedef std::vector<cTypeBlockPtr> audioChunkVector;
			
			uint64_t dimOfSignal;
			audioChunkVector audioVector;
			
			void assertAccessor( uint64_t argDim ) {
				assert ( argDim <= audioVector.size() );
			}	
					
		public:

			nDimAudioChunkAccessor(uint64_t dim) {
				/* dimOfSignal : the total number of vectors :
				 * example : signal is 44100 x 2 x 3
				 * dimOfSignal should be 6 (6 vectors of 44100 samples)
				 */
				audioVector.reserve( dim );
				this->dimOfSignal = dim;
				for (unsigned int i = 0 ; i < dimOfSignal ; ++i) {
					cTypeBlockPtr newSignal( new cTypeBlock<T>() );
					audioVector.push_back ( std::move ( newSignal ) );
				}
			}
			
			~nDimAudioChunkAccessor() {
				audioVector.clear();
				// std::cout << "Destructor of nDimAudioChunkAccessor" << std::endl;
			}
			
			uint64_t getDimOfSignal() {
				return dimOfSignal;
			}

			void setData(uint64_t dimOfSignal, T* startValue, uint64_t length) {
				this->assertAccessor ( dimOfSignal );
				audioVector[dimOfSignal]->firstValue = startValue;
				audioVector[dimOfSignal]->dim = length;
			}

			T* getT(uint64_t dimOfSignal) {
				this->assertAccessor ( dimOfSignal );
				return audioVector[dimOfSignal]->firstValue;
			}
			
			uint64_t getD(uint64_t dimOfSignal) {
				this->assertAccessor ( dimOfSignal );
				return audioVector[dimOfSignal]->dim;
			}
	};

	/* This is needed to save the
	 * first and the second parts of the last chunk from a 
	 * circular buffer
	 * */
	template<typename T>
	class twoCTypeBlock {		
		public :
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
	};

	/* A multidmentinal chunk may contains multiple twoCTypeBlock(s).
	 * This class contains them all.
	 * ATTENTION : This class do not contains the audio. It contains
	 * smart pointers to the audio. You have to point the pointers
	 * before using.
	 * */		
	template<typename T>
	class nTwoCTypeBlockAccessor {
		private:
		
			typedef std::unique_ptr<twoCTypeBlock<T> > twoCTypeBlockPtr;
			typedef std::vector<twoCTypeBlockPtr> audioChunkVector;
			
			uint64_t dimOfSignal;
			audioChunkVector audioVector;
			
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
				// std::cout << "Destructor of nTwoCTypeBlockAccessor" << std::endl;

			}
			
			uint64_t getDimOfSignal() {
				return dimOfSignal;
			}

			void setData(uint64_t dimOfSignal, twoCTypeBlock<T> *toCopy) {
				this->assertAccessor ( dimOfSignal );
				audioVector[dimOfSignal]->first = toCopy->first;
				audioVector[dimOfSignal]->second = toCopy->second;
			}

			T* getT(uint64_t dimOfSignal, bool secondArg = false) {
				this->assertAccessor ( dimOfSignal );
				if ( secondArg )
					return audioVector[dimOfSignal]->second->firstValue;
				else return audioVector[dimOfSignal]->first->firstValue;
			}
			
			uint64_t getD(uint64_t dimOfSignal, bool secondArg = false) {
				this->assertAccessor ( dimOfSignal );
				if ( secondArg )
					return audioVector[dimOfSignal]->second->dim;
				else return audioVector[dimOfSignal]->first->dim;
			}
	};
};

#endif /* DATATYPE_HPP */
