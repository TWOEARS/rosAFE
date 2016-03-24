#ifndef TIMEFREQUENCYSIGNAL_HPP
#define TIMEFREQUENCYSIGNAL_HPP

/*
 * TIMEFREQUENCYSIGNAL Signal class for two-dimensional, time-frequency representations.
 * This children signal class regroups all signal that are some sort of time frequency 
 * representation, including representation that were decimated in time.
 * 
 */ 

#include <string>
#include <stdexcept>
#include <stdint.h>

#include "Signal.hpp"

namespace openAFE {
	
	template<typename T>	
	class TimeFrequencySignal : public Signal<T> {
	
	private:

		/* cfHz : Center frequencies of the frequency channels */
		doubleVector cfHz;
		
		uint32Vector createDims( const doubleVector& cfHzArg ) {
			uint32Vector dims(1);
			if ( cfHzArg.size() )
				dims[0] = cfHzArg.size();
			else dims[0] = 1;
			
			return dims;
		}		
		
	protected:
	
		std::string scaling;	

	public:
	
		using nTwoCTypeBlockAccessorPtr = typename nTwoCTypeBlockAccessor<T>::nTwoCTypeBlockAccessorPtr;
		typedef typename std::shared_ptr<TimeFrequencySignal<T> > signalSharedPtr;

		/*       fs : Sampling frequency (Hz)
         *     name : Name tag of the signal, should be compatible with
         *            variable name syntax.
         *     cfHz : Center frequencies of the channels in Hertz.
         *    label : Label for the signal, to be used in e.g. figures
         *                 (default: label = name)
         *  channel : Flag indicating 'left', 'right', or 'mono'
         *               (default: channel = 'mono')
        */    

		TimeFrequencySignal( const uint32_t fs, const uint32_t bufferSize_s, const doubleVector& argCfHz,
							 const std::string argName = "tfRepresentation", 
							 const std::string argLabel = "tfRepresentation",
							 std::string argScaling = "magnitude", channel cha = _mono) : Signal<T>(fs, bufferSize_s, createDims( argCfHz ) ) {
													
			this->populateProperties( argLabel, argName, "nSamples x nFilters" );
			this->Channel = cha;
			this->cfHz = argCfHz;
			this->scaling = argScaling;
		}
		
		/* Calls automatically Signal's destructor */
		~TimeFrequencySignal() {
			cfHz.clear();		
		}
		
		void appendChunk( nTwoCTypeBlockAccessorPtr inChunk ) {	
			Signal<T>::appendChunk( inChunk );
		}

	}; /* class TimeFrequencySignal */
};	/* namespace openAFE */

#endif /* TIMEFREQUENCYSIGNAL_HPP */
