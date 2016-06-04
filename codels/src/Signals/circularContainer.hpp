// TODOs : if we change the capacity (++) are datas still continious ?


#ifndef CIRCULARCONTAINER_HPP
#define CIRCULARCONTAINER_HPP

#include <boost/circular_buffer.hpp>
#include <boost/circular_buffer/base.hpp>

#include <stdint.h>
#include <iostream>

#include "dataType.hpp"

namespace openAFE {
	
	template<typename T>
	class CircularContainer {
		
	public:
		// One dimention buffer
		typedef boost::circular_buffer<T> circularBuffer;
		typedef std::shared_ptr<CircularContainer<T> > containerPtr;

	private:

		using twoCTypeBlockPtr = typename twoCTypeBlock<T>::twoCTypeBlockPtr;
		typedef typename CircularContainer<T>::circularBuffer::iterator bufferIter;
		typedef typename boost::circular_buffer<T, std::allocator<T> >::array_range boostArrayRange;

		circularBuffer buffer;
		
		twoCTypeBlockPtr lastChunkInfo, lastDataInfo, oldDataInfo, wholeBufferInfo;
				
		/*
		 * freshData : Distance between the newest and the oldest - unseen data
		 * lastChunkSize : the size of last inputed chunk.
		 * */
		volatile uint32_t lastChunkSize;
		volatile long int freshData;
						
		inline
		void push_back(const T& val) {
			this->buffer.push_back(val);
		}
		
		/* getLastChunkSize : return the number of samples, appended by the last chunk */
		inline
		uint32_t getLastChunkSize() const {
			if (this->buffer.size() > 0)
				return this->lastChunkSize;
			else return 0;
		}

		/* getCapacity : Returns the total capacity of this buffer */
		uint32_t getCapacity() const {
			return this->buffer.capacity();
		}

		/* Update the sample number of the lately pushed chunk.
		 * Call this funtion if the argument "setNow" of push_chunk
		 * function is false.
		 * */
		void setLastChunkSize( uint32_t numSamples ) { // if referenced, then error in l 124 : this->setLastChunkSize ( 0 );
			
			if (numSamples > this->getCapacity() )
				this->lastChunkSize = this->getCapacity();
			else 
				this->lastChunkSize = numSamples ;
		}
													
	public :		
		
		/* Empty ctor. The buffer's capacity is zero. */
		/* Ctor with dimention : Buffer has a capacity */
		CircularContainer( unsigned int argDim = 0) {
			this->init( argDim );
		}

		/* Ctor with copy */
		CircularContainer( CircularContainer<T>& toCopy ) {
			this->init( toCopy );
		}
		
		/* Destroys the circular_buffer. */
		~CircularContainer() {
			this->clear();
		}
	
		/* This is the main initialisation, for each constructor */
		void init ( unsigned int argDims = 0 ) {
			
			// Set the buffer capacity
			this->buffer.set_capacity( argDims );
			
			this->lastChunkInfo.reset ( new twoCTypeBlock<T>() );
			this->lastDataInfo.reset ( new twoCTypeBlock<T>() );
			this->oldDataInfo.reset ( new twoCTypeBlock<T>() );
			this->wholeBufferInfo.reset ( new twoCTypeBlock<T>() );

			this->lastChunkSize = 0;
			this->freshData = 0;
		}
		
		/* Intermediaire initialisation function for copy constructor */
		void init ( CircularContainer<T>& toCopy ) {
			
			this->clear();
			this->init(toCopy.getCapacity());
			this->buffer = toCopy.buffer;		// verified : copys to another memory zone
		}

		/* Push to the back of the buffer a continuous c type vector
		 * 
		 * bool setNow : a signle chunk may be contained in two distinct
		 * c type vectors. In this case you should enter the
		 * lastChunkSize manually (setNow = false).
		 * This argument is then used to bypass the automatic update of 
		 * the lastChunkSize value. If your data is continious in a
		 * signle c type vector, then setNow = true
		 *
		 * */
		void push_chunk(T* firstValue, uint32_t dim, bool setNow = true) {
			
			if ( setNow == true )
				this->setLastChunkSize (dim);
				
			// Pushing back all values one by one.
			for (unsigned int i = 0 ; i < dim ; ++i )
				this->push_back( *(firstValue + i) );
			
			freshData += dim;
			if ( freshData > this->getCapacity() )
				freshData = this->getCapacity();
		}

		void push_chunk(twoCTypeBlockPtr inChunk) {	
			
			/* The first array */
			if ( inChunk->array1.second > 0 )
				this->push_chunk( inChunk->array1.first, inChunk->array1.second, false );
			/* If there is any data, the second array */
			if ( inChunk->array2.second > 0 )
				this->push_chunk( inChunk->array2.first, inChunk->array2.second, false );
			/* The total size of the last chunk */
			this->setLastChunkSize( inChunk->getSize() );
		}
		
		twoCTypeBlockPtr getLastChunkAccesor() {
			return this->lastChunkInfo;		
		}
		
		twoCTypeBlockPtr getLastDataAccesor() {
			return this->lastDataInfo;
		}

		twoCTypeBlockPtr getOldDataAccesor() {
			return this->oldDataInfo;
		}

		twoCTypeBlockPtr getWholeBufferAccesor() {
			return this->wholeBufferInfo;
		}
		
		/* calcLastChunk : updates the lastChunkInfo */
		void calcLastChunk() {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			 
			if ( ar2.second == 0 ) {	
				this->lastChunkInfo->array1.first = ar1.first + ar1.second - this->getLastChunkSize();
				this->lastChunkInfo->array1.second = this->getLastChunkSize();
				this->lastChunkInfo->array2.first = NULL;
				this->lastChunkInfo->array2.second = 0;
			} else {
				if ( this->getLastChunkSize() >= ar2.second ) {	
					this->lastChunkInfo->array1.second = lastChunkSize - ar2.second;
					this->lastChunkInfo->array1.first = ar1.first + ar1.second - this->lastChunkInfo->array1.second;
					this->lastChunkInfo->array2.second  = ar2.second;
					this->lastChunkInfo->array2.first = ar2.first;
				} else /* if ( lastChunkSize <= ar2.second ) */ {
					this->lastChunkInfo->array1.first = ar2.first + ar2.second - this->getLastChunkSize();
					this->lastChunkInfo->array1.second = this->getLastChunkSize();

					this->lastChunkInfo->array2.first = NULL;
					this->lastChunkInfo->array2.second  = 0;
					}
				}		
		}
		
		/* calcLatestData : updates the lastDataInfo 
		 * 
		 * Arguments : 
		 * 	samplesArg : the amount asked data
		 * 
		 * */
		void calcLatestData(uint32_t samplesArg) {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			
			if ( samplesArg > buffer.size() ) {
				samplesArg = buffer.size();	
			}
				
			if ( ar2.second == 0 ) {
				this->lastDataInfo->array1.first = ar1.first + ar1.second - samplesArg;
				this->lastDataInfo->array1.second = samplesArg;
				this->lastDataInfo->array1.first = NULL;
				this->lastDataInfo->array2.second = 0;
			} else if ( samplesArg >= ar2.second ) {
				this->lastDataInfo->array1.second = samplesArg - ar2.second;
				this->lastDataInfo->array1.first = ar1.first + ar1.second - this->lastDataInfo->array1.second;
				this->lastDataInfo->array2.second  = ar2.second;
				this->lastDataInfo->array1.first = ar2.first;
			} else /* if ( samplesArg <= ar2.second ) */ {
				this->lastDataInfo->array1.first = ar2.first + ar2.second - samplesArg;
				this->lastDataInfo->array1.second = samplesArg;

				this->lastDataInfo->array1.first = NULL;
				this->lastDataInfo->array2.second = 0;
				}
		}

		/* calcOldData : updates the oldDataInfo 
		 * 
		 * Arguments : 
		 * 	samplesArg : the amount asked data
		 *  setNow : if true, the fresh data is updated automatically.
		 * 	set false if you need to acces to the same oldDataInfo
		 *  mutiple times.
		 * 
		 * */
		void calcOldData(uint32_t samplesArg = 0) {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			
			if ( samplesArg > buffer.size() ) {
				samplesArg = buffer.size();	
			}
			
			if ( ( samplesArg > freshData ) || ( samplesArg == 0 ) ) {
				samplesArg = freshData;	
			}
						
			/* Eveything is in ar1 */
			if ( ar2.second == 0 ) {
				this->oldDataInfo->array1.first = ar1.first + ar1.second - freshData;
				this->oldDataInfo->array1.second = samplesArg;
				this->oldDataInfo->array1.first = NULL;
				this->oldDataInfo->array2.second = 0;
			       /* It is in ar1 ( but ar2 exists ) */
			} else if ( ar2.second < freshData ) {
					if ( ( freshData - samplesArg ) > ar2.second) {
						this->oldDataInfo->array1.first = ar1.first + ar1.second + ar2.second - freshData;
						this->oldDataInfo->array1.second = samplesArg;
						this->oldDataInfo->array1.first = NULL;
						this->oldDataInfo->array2.second = 0;	
					/* It is in ar1 and  ar2 */	
					} else {
						this->oldDataInfo->array1.first = ar1.first + ar1.second + ar2.second - freshData;
						this->oldDataInfo->array1.second = freshData - ar2.second;
						this->oldDataInfo->array1.first = ar2.first;
						this->oldDataInfo->array2.second = samplesArg - freshData + ar2.second ;	
					}
			/* Eveything is in ar2 */
			} else {
				this->oldDataInfo->array1.first = ar2.first + ar2.second - freshData;
				this->oldDataInfo->array1.second = samplesArg;
				this->oldDataInfo->array1.first = NULL;
				this->oldDataInfo->array2.second = 0;	
			}		
			
			this->freshData -= samplesArg;
			if ( this->freshData < 0 )
				this->freshData = 0;

		}

		/* calcWholeBuffer : updates the wholeBufferInfo */
		void calcWholeBuffer() {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();

			this->wholeBufferInfo->array1.first = ar1.first;
			this->wholeBufferInfo->array1.second = ar1.second;
			if ( ar2.second > 0 )
				this->wholeBufferInfo->array2.first = ar2.first;
			else 
				this->wholeBufferInfo->array2.first = NULL;
			this->wholeBufferInfo->array2.second = ar2.second;
		}

		/* getFreshDataSize : Returns the number of available non seen samples */
		uint32_t getFreshDataSize() {
			return this->freshData;
		}
		
		/* The buffer will contain only zeros after calling this function.
		 * However the capacity  will reamin as it was.
		 *
		 * */
 		void reset() {
			for(bufferIter it = buffer.begin(); it != buffer.end(); ++it)
				*it = 0;				
		}
		
		/* capacity will be zero after calling the clear function */
		void clear() noexcept {
			
			this->buffer.clear();
		}								
	};
};

#endif /* CIRCULARCONTAINER_HPP */

