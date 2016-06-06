#ifndef GAMMATONEFILTER_HPP
#define GAMMATONEFILTER_HPP

#include <complex>

#include "GenericFilter.hpp"

#include "../tools/mathTools.hpp"

namespace openAFE {

	class GammatoneFilter : public GenericFilter<double, std::complex<double>, double, std::complex<double> > {
	
	private:
	
        double CenterFrequency;     		// Center frequency for the filter (Hz)
        uint32_t FilterOrder;         	// Gammatone slope order
        double IRduration;         		// Duration of the impulse response for truncation (s)
        double delay;               		// Delay in samples for time alignment	
		
	public:
		
		GammatoneFilter( double cf, double fs, uint32_t n = 4, double bwERB = 1.018 ) : GenericFilter<double, std::complex<double>, double, std::complex<double> > ( ) {

			/*
             *         cf : center frequency of the filter (Hz)
             *         fs : sampling frequency (Hz)
             *          n : Gammatone rising slope order (default, n=4)
             *         bw : Bandwidth of the filter in ERBS 
             *              (default: bw = 1.018 ERBS)
			*/
				
            // One ERB value in Hz at this center frequency
			// double ERBHz = freq2erb ( cf );
            double ERBHz = 24.7 + 0.108 * cf;

            // Bandwidth of the filter in Hertz
            double bwHz = bwERB * ERBHz;

            // Generate an IIR Gammatone filter
            double btmp = 1 - exp( -2*M_PI*bwHz/fs );
                        
            std::vector<std::complex<double> > atmp(2,1);
            std::complex<double> tmp ( -2*M_PI*bwHz/fs, -2*M_PI*cf/fs);
			atmp[1] = -exp ( tmp );
            
            std::vector<double> b(1,1);
            std::vector<std::complex<double> > a(1,1);

			for ( size_t ii = 0 ; ii < n ; ++ii ) {
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
			this->reset();
			
			//   2- Specific properties
			this->CenterFrequency = cf;     		
			this->FilterOrder = n;
			this->delay = 0; // delaySpl
			
	/*		std::cout << "cf ; " << getCenterFrequency() << std::endl;
				std::cout << " A : " << std::endl;
				for ( unsigned int i = 0 ; i < a.size() ; i++ )
					std::cout << a[i] << " ";
				std::cout << std::endl;	
				std::cout << " B : " << std::endl;
				for ( unsigned int i = 0 ; i < b.size() ; i++ )
					std::cout << b[i] << " ";
				std::cout << std::endl;*/
		}
		
		~GammatoneFilter() {}
		
		const double getCenterFrequency() {
			return this->CenterFrequency;
		}

	};
};

#endif /* GAMMATONEFILTER_HPP */
