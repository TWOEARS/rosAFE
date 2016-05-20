#ifndef GAMMATONEFILTER_HPP
#define GAMMATONEFILTER_HPP

#include <assert.h>
#include <vector>
#include <iostream>

#include "apf/biquad.h"
#include "bwCoef.hpp"

#include <memory>


namespace openAFE {

	/* The type of the gammatoneFilter */
	enum gammatoneType {
		_iir,
		_fir
	};
		
	template<typename T>
	class gammatoneFilter {

	private:
	
		std::vector<double> cfHz            // Filters center frequencies
        std::vector<double> nERBs           // Distance between neighboring filters in ERBs
        double nGamma          				// Gammatone order of the filters
        double bwERBs          				// Bandwidth of the filters in ERBs
        double lowFreqHz       				// Lowest center frequency used at instantiation
        double highFreqHz      				// Highest center frequency used at instantiation
		
	public:
		
		gammatoneFilter(double cf, double fs, unsigned int n = 4, double bwERB = 1.08 , bool do_align = false, unsigned int cascade = 1) {

			/*
             *         cf : center frequency of the filter (Hz)
             *         fs : sampling frequency (Hz)
             *        type : 'fir' for finite impulse response or 'iir' for
             *              infinite (default: type = 'fir')
             *          n : Gammatone rising slope order (default, n=4)
             *         bw : Bandwidth of the filter in ERBS 
             *              (default: bw = 1.08 ERBS) 
             *   do_align : If true, applies phase compensation and compute
             *              delay for time alignment (default : false)
             *     durSec : Duration of the impulse response in seconds 
             *              (default: durSec = 0.128)
             *    cascade : Cascading order of the filter (default : 1)			
			*/
			
			float order = (float)argOrder;
			
            // One ERB value in Hz at this center frequency
            double ERBHz = 24.7 + 0.108 * cf;			

            // Bandwidth of the filter in Hertz
            double bwHz = bwERB * ERBHz;
            
            // Generate an IIR Gammatone filter
            double btmp=1-exp(-2*pi*bwHz/fs);
            std::complex<double> atmp = [1, -exp(-(2*pi*bwHz + 1i*2*pi*cf)/fs)];
		}
		
		~gammatoneFilter() {

			// delete casFilt;
		}
		
/*
		void filterChunk(T* sourceFront, T* sourceEnd, T* destFront) {
			casFilt->execute(sourceFront, sourceEnd, destFront);
		}
	*/
		
	};
};

#endif /* GAMMATONEFILTER_HPP */
