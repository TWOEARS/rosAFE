#ifndef ILDPROC_HPP
#define ILDPROC_HPP

#include <assert.h>
	
#include "TFSProcessor.hpp"
#include "ihcProc.hpp"

#include "../tools/window.hpp"

/* 
 * ILD Proc :
 * 
 * 
 * */
 
namespace openAFE {

	enum ildWindowType {
        _hamming,
        _hanning,
        _hann,
        _blackman,
        _triang,
        _sqrt_win
	};
	
	class ILDProc : public TFSProcessor<double > {

		private:
		
			ildWindowType ild_wname;       		// Window shape descriptor (see window.m)
			double ild_wSizeSec;    			  	// Window duration in seconds
			double ild_hSizeSec;    			  	// Step size between windows in seconds

			uint64_t ild_wSize;														// Window duration in samples
			uint64_t ild_hSize;														// Step size between windows in samples
			std::vector<double> win;         										// Window vector
			std::shared_ptr <TimeFrequencySignal<double> > buffer_l, buffer_r;    	// Buffered input signals

			uint32_t fb_nChannels;
			
			std::shared_ptr<IHCProc > upperProcPtr;

			std::vector<double> zerosVector;
			std::shared_ptr<twoCTypeBlock<double> > zerosAccecor;
			
			/// Output sampling frequency;
			uint32_t calcFsOut( double ild_hSizeSec ) {
				return 1 / ild_hSizeSec;
			}

			void prepareForProcessing() {
            
				/// Compute internal parameters
				this->ild_wSize = 2 * round( this->ild_wSizeSec * this->getFsIn() / 2 );
				this->ild_hSize = round( this->ild_hSizeSec * this->getFsIn() );		
		
				switch ( this->ild_wname ) {
					case _hamming: 
						this->win = hamming( this->ild_wSize );
						break;
					case _hanning: 
						this->win = hanning( this->ild_wSize );
						break;
					case _hann: 
						this->win = hann( this->ild_wSize );
						break;
					case _blackman: 
						this->win = blackman( this->ild_wSize );
						break;
					case _triang: 
						this->win = triang( this->ild_wSize );
						break;
					case _sqrt_win: 
						this->win = sqrt_win( this->ild_wSize );
						break;
					default:
						this->win.resize( this->ild_wSize, 0 );
						break;
				}
				
				this->zerosAccecor.reset( new twoCTypeBlock<double>() );
			}

			void processChannel( double* firstValue_l, double* firstValue_r, std::shared_ptr<twoCTypeBlock<double> > PMZ, size_t ii ) {
				
				multiplication( this->win.data(), firstValue_l, this->ild_wSize, firstValue_l );
				multiplication( this->win.data(), firstValue_r, this->ild_wSize, firstValue_r );
							
				double mSq_l = meanSquare( firstValue_l, this->ild_wSize );
				double mSq_r = meanSquare( firstValue_r, this->ild_wSize );
							
				// std::cout << ild( mSq_r, mSq_l ) << std::endl;
				if ( PMZ->array1.second >= ii )
					*(PMZ->array1.first + ii ) = ild( mSq_r, mSq_l );
				else
					*(PMZ->array2.first + ( ii - PMZ->array1.second ) ) = ild( mSq_r, mSq_l );
			}
			
		public:
		
			ILDProc (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr,
				double ild_wSizeSec = 0.02, double ild_hSizeSec = 0.01, ildWindowType ild_wname = _hann )
			: TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), this->calcFsOut( ild_hSizeSec ), upperProcPtr->getBufferSize_s(), upperProcPtr->get_ihc_nChannels(), "magnitude", _ild) {
					
				this->fb_nChannels = upperProcPtr->get_ihc_nChannels();
				
				this->upperProcPtr = upperProcPtr;
				this->ild_wSizeSec = ild_wSizeSec;
				this->ild_hSizeSec = ild_hSizeSec;
				this->ild_wname = ild_wname;
								
				buffer_l.reset( new TimeFrequencySignal<double>( this->getFsIn(), this->getBufferSize_s(), this->fb_nChannels, "inner buffer", "magnitude", _left) );
				buffer_r.reset( new TimeFrequencySignal<double>( this->getFsIn(), this->getBufferSize_s(), this->fb_nChannels, "inner buffer", "magnitude", _right) );
								
				this->prepareForProcessing();

			}

			~ILDProc () {
				
				win.clear();
				buffer_l.reset();
				buffer_r.reset();
			}
			
			void processChunk () {				
				this->setNFR ( this->upperProcPtr->getNFR() );
							
				// Append provided input to the buffer
				buffer_l->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				buffer_r->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_innerBuffer = buffer_l->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_innerBuffer = buffer_r->getLastChunkAccesor();
				
				// Quick control of dimensionality
				assert( l_innerBuffer.size() == r_innerBuffer.size() );
				
				uint32_t nSamples = l_innerBuffer[0]->getSize();
				
				for ( size_t ii = 0 ; ii < l_innerBuffer.size() ; ++ii ) {
					assert ( nSamples == r_innerBuffer[ii]->getSize() );
					assert ( l_innerBuffer[ii]->getSize() == nSamples );
				}

				size_t nSamples_1 = l_innerBuffer[0]->array1.second;
				size_t nSamples_2 = l_innerBuffer[0]->array2.second;

				// How many frames are in the buffered input?
				size_t nFrames_1 = 0, nFrames_2 = 0;

				if ( nSamples_1 > this->ild_wSize ) 
					nFrames_1 = floor( ( nSamples_1 - ( this->ild_wSize - this->ild_hSize ) ) / this->ild_hSize );
				if ( nSamples_2 > this->ild_wSize ) 
					nFrames_2 = floor( ( nSamples_2 - ( this->ild_wSize - this->ild_hSize ) ) / this->ild_hSize );
					
	/*			std::cout << "OK" << std::endl;
				std::cout << "nSamples_1 : " << nSamples_1 << " nSamples_2 : " << nSamples_2 << std::endl;		
				std::cout << "this->ild_wSize : " << this->ild_wSize << " this->ild_hSize : " << this->ild_hSize << std::endl;	
				std::cout << "nFrames_1 : " << nFrames_1 << " nFrames_2 : " << nFrames_2 << std::endl;		*/		
				
				// Creating a chunk of zeros.
				this->zerosVector.resize( nFrames_1 + nFrames_2, 0 );

				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;
				

				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > PMZ = leftPMZ->getLastChunkAccesor();
				
				// Processing on PMZ
            	if ( nFrames_1 > 0 ) {
					// Loop on the time frame				
					for ( size_t ii = 0 ; ii < nFrames_1 ; ++ii ) {
						// Get start and end indexes for the current frame
						uint32_t n_start = ii * this->ild_hSize;
						// uint32_t n_end = ii * this->ild_hSize + this->ild_wSize - 1;
					
						// Loop on the channel
						for ( size_t jj = 0 ; jj < this->fb_nChannels ; ++jj ) {
							processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, PMZ[jj], ii );
						}
						
			/*			std::vector<std::thread> threads;
						for ( size_t jj = 0 ; ii < this->fb_nChannels ; ++jj )
							threads.push_back(std::thread( &ILDProc::processChannel, this, l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, PMZ[jj], ii ));

						// Waiting to join the threads
						for (auto& t : threads)
							t.join();						
			*/			
						
						
					}
				}
				
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool operator==( ILDProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( ( this->get_ild_wSizeSec() == toCompare.get_ild_wSizeSec() ) and 
						 ( this->get_ild_hSizeSec() == toCompare.get_ild_hSizeSec() ) and 					
						 ( this->get_ild_wname() == toCompare.get_ild_wname() ) )				     		     			     
						return true;
				return false;
			}

			// getters
			const double get_ild_wSizeSec() {return this->ild_wSizeSec;}
			const double get_ild_hSizeSec() {return this->ild_hSizeSec;}
			const ildWindowType get_ild_wname() {return this->ild_wname;}
			
			const uint32_t get_ild_nChannels() {return this->fb_nChannels;}

			// setters			
			void set_ild_wSizeSec(const double arg) {this->ild_wSizeSec=arg; this->prepareForProcessing ();}
			void set_ild_hSizeSec(const double arg) {this->ild_hSizeSec=arg; this->prepareForProcessing ();}
			void set_ild_wname(const ildWindowType arg) {this->ild_wname=arg; this->prepareForProcessing ();}

			std::string get_upperProcName() {return this->upperProcPtr->getName();}
			
	}; /* class ILDProc */
}; /* namespace openAFE */

#endif /* ILDPROC_HPP */
