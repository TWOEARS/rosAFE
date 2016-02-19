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
	
	private:
		
		// One dimention buffer
		typedef boost::circular_buffer<T> circularBuffer;
		typedef typename boost::circular_buffer<T, std::allocator<T> >::array_range boostArrayRange;
		typedef typename CircularContainer<T>::circularBuffer::iterator bufferIter;

		circularBuffer buffer;
		
		twoCTypeBlock<T> *lastChunkInfo, *newDataInfo, *oldDataInfo, *wholeBufferInfo;
				
		/*
		 * freshData : Distance between the newest and the oldest - unseen data
		 * lastChunkSize : the size of last inputed chunk.
		 * */
		volatile uint64_t lastChunkSize;
		volatile long int freshData;
						
		inline
		void push_back(const T& val) {
			this->buffer.push_back(val);
		}
		
		void calcLastChunk() {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			
			if ( ar2.second == 0 ) {
				this->lastChunkInfo->first->firstValue = ar1.first + ar1.second - this->getLastChunkSize();
				this->lastChunkInfo->first->dim = this->getLastChunkSize();
				this->lastChunkInfo->second->firstValue = NULL;
				this->lastChunkInfo->second->dim = 0;
			} else if ( this->getLastChunkSize() >= ar2.second ) {
				this->lastChunkInfo->first->dim = lastChunkSize - ar2.second;
				this->lastChunkInfo->first->firstValue = ar1.first + ar1.second - this->lastChunkInfo->first->dim;
				this->lastChunkInfo->second->dim  = ar2.second;
				this->lastChunkInfo->second->firstValue = ar2.first;
			} else /* if ( lastChunkSize <= ar2.second ) */ {
				this->lastChunkInfo->first->firstValue = ar2.first + ar2.second - this->getLastChunkSize();
				this->lastChunkInfo->first->dim = this->getLastChunkSize();

				this->lastChunkInfo->second->firstValue = NULL;
				this->lastChunkInfo->second->dim  = 0;
				}
		}

		void calcLatestData(uint64_t& samplesArg) {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			
			if ( samplesArg > buffer.size() ) {
				samplesArg = buffer.size();	
			}
				
			if ( ar2.second == 0 ) {
				this->newDataInfo->first->firstValue = ar1.first + ar1.second - samplesArg;
				this->newDataInfo->first->dim = samplesArg;
				this->newDataInfo->second->firstValue = NULL;
				this->newDataInfo->second->dim = 0;
			} else if ( samplesArg >= ar2.second ) {
				this->newDataInfo->first->dim = samplesArg - ar2.second;
				this->newDataInfo->first->firstValue = ar1.first + ar1.second - this->newDataInfo->first->dim;
				this->newDataInfo->second->dim  = ar2.second;
				this->newDataInfo->second->firstValue = ar2.first;
			} else /* if ( samplesArg <= ar2.second ) */ {
				this->newDataInfo->first->firstValue = ar2.first + ar2.second - samplesArg;
				this->newDataInfo->first->dim = samplesArg;

				this->newDataInfo->second->firstValue = NULL;
				this->newDataInfo->second->dim = 0;
				}
		}

		void calcOldData(uint64_t& samplesArg) {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();
			
			if ( samplesArg > buffer.size() ) {
				samplesArg = buffer.size();	
			}
			
			if ( samplesArg > freshData ) {
				samplesArg = freshData;	
			}
			
			/* Eveything is in ar1 */
			if ( ar2.second == 0 ) {
				this->oldDataInfo->first->firstValue = ar1.first + ar1.second - freshData;
				this->oldDataInfo->first->dim = samplesArg;
				this->oldDataInfo->second->firstValue = NULL;
				this->oldDataInfo->second->dim = 0;
			       /* It is in ar1 ( but ar2 exists ) */
			} else if ( ar2.second < freshData ) {
					if ( ( freshData - samplesArg ) > ar2.second) {
						this->oldDataInfo->first->firstValue = ar1.first + ar1.second + ar2.second - freshData;
						this->oldDataInfo->first->dim = samplesArg;
						this->oldDataInfo->second->firstValue = NULL;
						this->oldDataInfo->second->dim = 0;	
					/* It is in ar1 and  ar2 */	
					} else {
						this->oldDataInfo->first->firstValue = ar1.first + ar1.second + ar2.second - freshData;
						this->oldDataInfo->first->dim = freshData - ar2.second;
						this->oldDataInfo->second->firstValue = ar2.first;
						this->oldDataInfo->second->dim = samplesArg - freshData + ar2.second ;	
					}
			/* Eveything is in ar2 */
			} else {
				this->oldDataInfo->first->firstValue = ar2.first + ar2.second - freshData;
				this->oldDataInfo->first->dim = samplesArg;
				this->oldDataInfo->second->firstValue = NULL;
				this->oldDataInfo->second->dim = 0;	
			}		
						
			freshData -= samplesArg;
			if ( freshData < 0 )
				freshData = 0;
		}

		void calcWholeBuffer() {
			
			boostArrayRange ar1 = buffer.array_one();
			boostArrayRange ar2 = buffer.array_two();

			this->wholeBufferInfo->first->firstValue = ar1.first;
			this->wholeBufferInfo->first->dim = ar1.second;
			if ( ar2.second > 0 )
				this->wholeBufferInfo->second->firstValue = ar2.first;
			else 
				this->wholeBufferInfo->second->firstValue = NULL;
			this->wholeBufferInfo->second->dim = ar2.second;
		}
		
		inline
		uint64_t getLastChunkSize() const {
			if (this->buffer.size() > 0)
				return this->lastChunkSize;
			else return 0;
		}

		// Tells if the memory is linear
		bool isLinearized() const {
			return this->buffer.is_linearized();
		}
							
	public :		
		
		// Empty ctor
		CircularContainer( ) {
			}

		// Ctor with dimensions	
		CircularContainer( unsigned int argDim ) {
			std::cout << "Buffer of : " << argDim << std::endl;
			this->init( argDim );
		}

		// Ctor with copy		
		CircularContainer( CircularContainer<T>& toCopy ) {
			
			this->init( toCopy );
		}
		
		/* Destroys the circular_buffer. */
		~CircularContainer() {
			this->buffer.clear();
			delete lastChunkInfo;		// FIXME : smart pointer (?)
			delete newDataInfo;
			delete oldDataInfo;
			std::cout << "Destructor of a buffer" << std::endl;
		}
	
		/* This is the main initialisation, for each constructor */
		void init ( unsigned int argDims ) {
			
			// We set the capacity
			this->buffer.set_capacity( argDims );
			this->lastChunkInfo = new twoCTypeBlock<T>();
			this->newDataInfo = new twoCTypeBlock<T>();
			this->oldDataInfo = new twoCTypeBlock<T>();
			this->wholeBufferInfo = new twoCTypeBlock<T>();

			this->setLastChunkSize ( 0 );
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
		void push_chunk(T* firstValue, uint64_t dim, bool setNow = true) {
			
			if ( setNow == true )
				this->setLastChunkSize (dim);
				
			// Pushing back all values one by one.
			for (unsigned int i = 0 ; i < dim ; ++i )
				this->push_back( *(firstValue + i) );
			
			freshData += dim;
			if ( freshData > this->getCapacity() )
				freshData = this->getCapacity();
		}

		/* Update the sample number of the lately pushed chunk.
		 * Call this funtion if the argument "setNow" of push_chunk
		 * function is false.
		 * */
		void setLastChunkSize( uint64_t numSamples ) { // if referenced, then error in l 124 : this->setLastChunkSize ( 0 );
			
			if (numSamples > this->getCapacity() )
				this->lastChunkSize = this->getCapacity();
			else 
				this->lastChunkSize = numSamples ;
		}

		bool empty() const {
			return this->buffer.empty();
		}

		twoCTypeBlock<T>* getLastChunk() {
			this->calcLastChunk();
			return this->lastChunkInfo;		
		}
		
		twoCTypeBlock<T>* getLastData(uint64_t samplesArg) {
			this->calcLatestData( samplesArg );
			return this->newDataInfo;
		}

		twoCTypeBlock<T>* getOldData(uint64_t samplesArg = 0) {
			if ( samplesArg == 0 ) samplesArg = freshData;
			this->calcOldData( samplesArg );
			return this->oldDataInfo;
		}

		twoCTypeBlock<T>* getWholeBuffer() {
			this->calcWholeBuffer( );
			return this->wholeBufferInfo;
		}
		
		uint64_t getCapacity() {
			return this->buffer.capacity();
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

