#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <stdint.h>
#include <string> 
#include <math.h>       /* ceil */

#include <memory>

#include <apf/parameter_map.h>

namespace openAFE {
	
	enum channel {
		_mono,
		_left,
		_right
	};
	  
	class Signal {
	    		
	protected:

		std::string Name;            			// Used as an instance name
		uint32_t  FsHz;       					// Sampling frequency
		channel Channel;         				// Flag keeping track of the channel

		uint32_t bufferSizeSamples;
		
	public:

		/*	
		 *  INPUT ARGUMENTS:
		 *  fs : Sampling frequency (Hz)
		 *  bufferSize_s : Buffer duration in s
		*/
				      
		Signal( const uint32_t fs, const std::string& nameArg, const double bufferSize_s, channel cha ) {
		
			this->FsHz = fs;
			this->Name = nameArg;
			this->Channel = cha;

			/* Calculation of the buffer size in samples */
			this->bufferSizeSamples = floor( bufferSize_s * this->FsHz );
		}
		
		~Signal() { }
				
		virtual void reset () = 0;
							
		const std::string getName() {
			return Name;
		}
		
		/* getChannel : returns the channel of this signal */
		const channel getChannel() {  
			return Channel;
		}
			    
	}; /* class Signal */
}; /* namespace openAFE */

#endif /* SIGNAL_HPP */

