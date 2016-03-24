#ifndef TimeDomainSignal_H
#define TimeDomainSignal_H

#include <string>
#include <stdexcept>
#include <stdint.h>

#include "Signal.hpp"

namespace openAFE {
	
	template<typename T>	
	class TimeDomainSignal : public Signal<T> {
	
	private:

	public:
	
		using nTwoCTypeBlockAccessorPtr = typename nTwoCTypeBlockAccessor<T>::nTwoCTypeBlockAccessorPtr;
		typedef typename std::shared_ptr<TimeDomainSignal<T> > signalSharedPtr;
		
		/* Create a TimeDomainSignal without initialising a first chunk */
		TimeDomainSignal( const uint32_t fs, const uint32_t bufferSize_s, const std::string argName = "Time", const std::string argLabel = "Waveform", channel cha = _mono) : Signal<T>(fs, bufferSize_s) {
	
			this->populateProperties(argName, argLabel, "nSamples x 1");
			this->Channel = cha;
		}
		
		/* Calls automatically Signal's destructor */
		~TimeDomainSignal() {}
		
		void appendTChunk( T* inChunk, uint32_t dim ) {
			
			Signal<T>::appendChunk( inChunk, dim );
		}
		
		void appendChunk( nTwoCTypeBlockAccessorPtr inChunk ) {
			
			Signal<T>::appendChunk( inChunk );
		}

	};
};

#endif /* TimeDomainSignal_H */
