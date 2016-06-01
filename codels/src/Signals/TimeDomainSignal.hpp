#ifndef TIMEDOMAINSIGNAL_H
#define TIMEDOMAINSIGNAL_H

#include <string>
#include <stdint.h>
#include <memory>

#include "Signal.hpp"
#include "circularContainer.hpp"
#include "dataType.hpp"

namespace openAFE {
	
	template<typename T>
	class TimeDomainSignal : public Signal {
	
	private:
	
	    std::shared_ptr<CircularContainer<T> > buffer;
	    std::shared_ptr<twoCTypeBlock<T> > lastChunkInfo, wholeBufferInfo;
			    
	public:
			
		/* Create a TimeDomainSignal without initialising a first chunk */
		TimeDomainSignal( const uint32_t fs, const uint32_t bufferSize_s, const std::string argName = "Time", channel cha = _mono) : Signal(fs, argName, bufferSize_s, cha) {
	
			this->buffer.reset( new CircularContainer<T>( this->bufferSizeSamples ) );
			this->lastChunkInfo.reset( new twoCTypeBlock<float> );
			this->wholeBufferInfo.reset( new twoCTypeBlock<float> );
		}
		
		/* Calls automatically Signal's destructor */
		~TimeDomainSignal() {
			this->buffer.reset();
			this->lastChunkInfo.reset();
			this->wholeBufferInfo.reset();
		}
		
		void appendTChunk( T* inChunk, size_t dim ) {							
			this->buffer->push_chunk( inChunk, dim );
		}
		
		void appendChunk( std::shared_ptr<twoCTypeBlock<T> > inChunk ) {
			this->buffer->push_chunk( inChunk );
		}
		
		std::shared_ptr<twoCTypeBlock<T> > getLastChunkAccesor() {
			this->buffer->calcLastChunk();
			this->lastChunkInfo->setData( this->buffer->getLastChunkAccesor() );
			return this->lastChunkInfo;
		}

		std::shared_ptr<twoCTypeBlock<T> > getWholeBufferAccesor() {
			this->buffer->calcWholeBuffer();
			this->wholeBufferInfo->setData( this->buffer->getWholeBufferAccesor() );
			return this->wholeBufferInfo;
		}
		
		/* Puts zero to all over the buffer */
		void reset () {
			this->buffer->reset();
		}
		
	};
};

#endif /* TIMEDOMAINSIGNAL_H */
