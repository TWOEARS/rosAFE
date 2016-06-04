#ifndef GAMMATONEFILTER_HPP
#define GAMMATONEFILTER_HPP

#include <complex>

#include "GenericFilter.hpp"

#include "../tools/mathTools.hpp"

namespace openAFE {

	class GammatoneFilter : public GenericFilter<float, std::complex<float>, float, std::complex<float> > {
	
	private:
	
        float CenterFrequency;     		// Center frequency for the filter (Hz)
        uint32_t FilterOrder;         	// Gammatone slope order
        float IRduration;         		// Duration of the impulse response for truncation (s)
        float delay;               		// Delay in samples for time alignment	
		
	public:
		
		GammatoneFilter( float cf, float fs, unsigned int n = 4, float bwERB = 1.018 ) : GenericFilter<float, std::complex<float>, float, std::complex<float> > ( ) {

			/*
             *         cf : center frequency of the filter (Hz)
             *         fs : sampling frequency (Hz)
             *          n : Gammatone rising slope order (default, n=4)
             *         bw : Bandwidth of the filter in ERBS 
             *              (default: bw = 1.018 ERBS)
			*/
			
            // One ERB value in Hz at this center frequency
           // float ERBHz = freq2erb ( cf );
            float ERBHz = 24.7 + 0.108 * cf;

            // Bandwidth of the filter in Hertz
            float bwHz = bwERB * ERBHz;

            // Generate an IIR Gammatone filter
            float btmp = 1 - exp( -2*M_PI*bwHz/fs );
                        
            std::vector<std::complex<float> > atmp(2,1);
            std::complex<float> tmp ( -2*M_PI*bwHz/fs, -2*M_PI*cf/fs);
			atmp[1] = -exp ( tmp );
            
            std::vector<float> b(1,1);
            std::vector<std::complex<float> > a(1,1);
      
			for ( unsigned int ii = 0 ; ii < n ; ++ii ) {
				b = conv( &btmp, 1, b.data(), b.size() );				
				a = conv( atmp.data(), atmp.size(), a.data(), a.size() );
			}
											
            // The transfer function is complex-valued
            this->setRealTF ( false );
            
            // Populate filter Object properties
            //   1- Global properties
            try {
				this->setCoeff ( b.data(), b.size(), a.data(), a.size() );
			} catch(std::string const& message) {
			   throw std::string( message );
		    }
        
			//   2- Specific properties
			this->CenterFrequency = cf;     		
			this->FilterOrder = n;
			this->delay = 0; // delaySpl		
		}
		
		~GammatoneFilter() {}
		
		const float getCenterFrequency() {
			return this->CenterFrequency;
		}

	};
};

#endif /* GAMMATONEFILTER_HPP */
