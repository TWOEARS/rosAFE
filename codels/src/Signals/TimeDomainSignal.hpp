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
	
		void populateProperties(const std::string argName, const std::string argLabel) {
			
			apf::parameter_map params;

			params.set("Label", argLabel);
			params.set("Name", argName);
			params.set("Dimensions", "nSamples x 1");
			
			Signal<T>::populateProperties(params);
		}

	public:
	
		using nTwoCTypeBlockAccessorPtr = typename nTwoCTypeBlockAccessor<T>::nTwoCTypeBlockAccessorPtr;
		typedef typename std::shared_ptr<TimeDomainSignal<T> > signalSharedPtr;
		
		/* Create a TimeDomainSignal without initialising a first chunk */
		TimeDomainSignal(uint32_t fs, uint32_t bufferSize_s, std::string argName = "Time", std::string argLabel = "Waveform", channel cha = _mono) : Signal<T>(fs, bufferSize_s) {
	
			this->populateProperties(argName, argLabel);
			Signal<T>::Channel = cha;
		}

		/* Create a TimeDomainSignal with initialising a first chunk */		 
		TimeDomainSignal(uint32_t fs, uint32_t bufferSize_s, std::string argName, std::string argLabel, nTwoCTypeBlockAccessorPtr data, channel cha) : Signal<T>(fs, bufferSize_s) {
	
		    // Check dimensionality of data
			assert (data->getDimOfSignal() == 1);
				
			populateProperties(argName, argLabel);
			Signal<T>::Channel = cha;
			
			Signal<T>::setData(data);
		}
		
		/* Calls automatically Signal's destructor */
		~TimeDomainSignal() {
			std::cout << "Destructor of a TimeDomainSignal<T>" << std::endl;
		}
		
		void appendTChunk( T* inChunk, uint32_t dim ) {
			
			Signal<T>::appendChunk( inChunk, dim );
		}
		
		void appendChunk( nTwoCTypeBlockAccessorPtr inChunk ) {
			
			Signal<T>::appendChunk( inChunk );
		}

	};
};

#endif /* TimeDomainSignal_H */
