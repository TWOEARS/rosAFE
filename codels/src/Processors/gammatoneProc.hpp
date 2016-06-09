#ifndef GAMMATONEPROC_HPP
#define GAMMATONEPROC_HPP
	
#include "TFSProcessor.hpp"
#include "preProc.hpp"	

#include "../Filters/GammatoneFilter.hpp"

#include "../tools/mathTools.hpp"


/* 
 * GamamtoneProc :
 * 
 * 
 * */
 
namespace openAFE {
	
	enum filterBankType{
		_gammatoneFilterBank,
		_drnlFilterBank
	};
	
	class GammatoneProc : public TFSProcessor<double > {

		private:

			filterBankType fb_type;

			std::vector<double> cfHz;            									// Filters center frequencies
			double fb_nERBs;          												// Distance between neighboring filters in ERBs
			uint32_t fb_nGamma;       							    				// Gammatone order of the filters
			double fb_bwERBs;        							    				// Bandwidth of the filters in ERBs
			double fb_lowFreqHz;       							   	 				// Lowest center frequency used at instantiation
			double fb_highFreqHz;      							    				// Highest center frequency used at instantiation
			uint32_t fb_nChannels;

            typedef std::shared_ptr < GammatoneFilter > gammatoneFilterPtr;
            typedef std::vector <gammatoneFilterPtr > filterPtrVector;
			filterPtrVector leftFilters, rightFilters;		    				// Array of filter pointer objects
			      
			std::shared_ptr<PreProc > upperProcPtr;
			      			
            size_t verifyParameters( filterBankType fb_type, double fb_lowFreqHz, double fb_highFreqHz, double fb_nERBs,
									 uint32_t fb_nChannels, double* fb_cfHz, size_t fb_cfHz_length, uint32_t fb_nGamma, double fb_bwERBs ) {
				
				this->fb_type = fb_type;
				this->fb_lowFreqHz = fb_lowFreqHz;
				this->fb_highFreqHz = fb_highFreqHz;
				this->fb_nERBs = fb_nERBs;
				this->fb_nChannels = fb_nChannels;		
				this->fb_nGamma = fb_nGamma;
				this->fb_bwERBs = fb_bwERBs;

				/* Solve the conflicts between center frequencies, number of channels, and
				 * distance between channels.
				 */

				if ( !( fb_cfHz ) and ( fb_cfHz_length > 0 ) ) {
                // Highest priority case: a vector of channels center 
                //   frequencies is provided
					this->cfHz.resize( fb_cfHz_length );
					for ( size_t ii = 0 ; ii < fb_cfHz_length ; ++ii )
						cfHz[ii]= *( fb_cfHz + ii );
             
					this->fb_lowFreqHz = *( cfHz.begin() );
					this->fb_highFreqHz = *( cfHz.end() - 1 );
					this->fb_nChannels = fb_cfHz_length;
					this->fb_nERBs = 0;

				} else 
					if ( this->fb_nChannels > 0 ) {
						/* Medium priority: frequency range and number of channels
						 *  are provided.
						 */ 
					   
						// Build a vector of center ERB frequencies
						std::vector<double> ERBS = linspace( freq2erb(this->fb_lowFreqHz),
															freq2erb(this->fb_highFreqHz),
															this->fb_nChannels ); 
						this->cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), cfHz.data() );    // Convert to Hz
									
						this->fb_nERBs = ( *(ERBS.end()-1) - *(ERBS.begin()) ) / this->fb_nChannels ;
					}
					else {
						/* Lowest (default) priority: frequency range and distance 
						 *   between channels is provided (or taken by default).
						 */
						
						// Build vector of center ERB frequencies
						std::vector<double> ERBS;
						for ( double tmp = freq2erb( this->fb_lowFreqHz ) ; tmp < freq2erb( this->fb_highFreqHz ) ; tmp += this->fb_nERBs )
							ERBS.push_back( tmp );
		
						this->cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), this->cfHz.data() );    // Convert to Hz

						this->fb_nChannels = this->cfHz.size();														
					}
				return this->fb_nChannels;
			}
			
			void populateFilters( filterPtrVector& filters ) {
				
				const uint32_t fs = this->getFsIn();
				filters.clear();
				filters.resize(cfHz.size());
								
				for ( unsigned int ii = 0 ; ii < this->cfHz.size() ; ++ii ) {
					gammatoneFilterPtr thisFilter( new GammatoneFilter( this->cfHz[ii], fs, this->fb_nGamma, this->fb_bwERBs ) );
					filters[ii] = thisFilter ;
				}
			}

			void processChannel ( gammatoneFilterPtr oneFilter, std::shared_ptr<twoCTypeBlock<double> > oneChannel ) {
				// 0- Initialization
				size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				double* firstValue1 = oneChannel->array1.first;
				double* firstValue2 = oneChannel->array2.first;
													
				std::vector< std::complex<double > > tmpComplex;
				if ( dim1 > 0 ) {
					tmpComplex.resize(dim1, 0);
					oneFilter->exec( firstValue1, dim1, tmpComplex.data() );							
					for ( size_t ii = 0 ; ii < dim1 ; ++ii )
						*( firstValue1 + ii ) = tmpComplex[ii].real() * 2;
				}
				if ( dim2 > 0 )	{
					tmpComplex.resize(dim2, 0);
					oneFilter->exec( firstValue2, dim2, tmpComplex.data() );
					for ( size_t ii = 0 ; ii < dim2 ; ++ii )
						*( firstValue2 + ii ) = tmpComplex[ii].real() * 2;	
				}

			/*	if ( oneFilter->getCenterFrequency() < 81 ) {
						std::cout << oneFilter->getCenterFrequency() << std::endl;
						for ( size_t iix = 0 ; iix < oneFilter->getStates().size() ; ++iix )
							std::cout << oneFilter->getStates()[iix] << ", ";	
						std::cout << std::endl;
				}*/
			}

			void processLR ( filterPtrVector& filters, std::vector<std::shared_ptr<twoCTypeBlock<double> > > PMZ ) {
				std::vector<std::thread> threads;
				  for ( size_t ii = 0 ; ii < this->cfHz.size() ; ++ii )
					threads.push_back(std::thread( &GammatoneProc::processChannel, this, filters[ii], PMZ[ii] ));

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();
			}
														
		public:
		
			GammatoneProc (const std::string nameArg, std::shared_ptr<PreProc > upperProcPtr,
																																						filterBankType fb_type = _gammatoneFilterBank,
																																						double fb_lowFreqHz = 80,
																																						double fb_highFreqHz = 8000,
																																						double fb_nERBs = 1,
																																						uint32_t fb_nChannels = 0,		
																																						double* fb_cfHz = nullptr,		
																																						size_t fb_cfHz_length = 0,		
																																						uint32_t fb_nGamma = 4,
																																						double fb_bwERBs = 1.0180
						  ) : TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), verifyParameters( fb_type, fb_lowFreqHz, fb_highFreqHz, fb_nERBs, fb_nChannels,		
																										 fb_cfHz, fb_cfHz_length, fb_nGamma, fb_bwERBs), "magnitude", _gammatone) {
																											 
				this->verifyParameters( fb_type, fb_lowFreqHz, fb_highFreqHz, fb_nERBs, fb_nChannels, fb_cfHz, fb_cfHz_length, fb_nGamma, fb_bwERBs);

				this->upperProcPtr = upperProcPtr;				
				this->prepareForProcessing ();
			}
				
			~GammatoneProc () {
				leftFilters.clear();
				rightFilters.clear();
			}
			
			void processChunk ( ) {
				this->setNFR ( this->upperProcPtr->getNFR() );
				
				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
			/*	std::thread leftThread( &GammatoneProc::processLR, this->leftFilters, l_PMZ );
				std::thread rightThread( &GammatoneProc::processLR, this->rightFilters, r_PMZ );
							
				leftThread.join();                // pauses until left finishes
				rightThread.join();               // pauses until right finishes*/
				this->processLR( this->leftFilters, l_PMZ );
				this->processLR( this->rightFilters, r_PMZ );
			}
			
			void prepareForProcessing () {

				this->populateFilters( leftFilters );
				this->populateFilters( rightFilters );
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool operator==( GammatoneProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( ( this->get_fb_type() == toCompare.get_fb_type() ) and
					     ( this->get_fb_lowFreqHz() == toCompare.get_fb_lowFreqHz() ) and
					     ( this->get_fb_highFreqHz() == toCompare.get_fb_highFreqHz() ) and	
					     ( this->get_fb_nERBs() == toCompare.get_fb_nERBs() ) and	     
					     ( this->get_fb_nChannels() == toCompare.get_fb_nChannels() ) and
					     ( this->get_fb_nGamma() == toCompare.get_fb_nGamma() ) and	
					     ( this->get_fb_bwERBs() == toCompare.get_fb_bwERBs() ) )
						return true;
				return false;
			}

			// getters
			const filterBankType get_fb_type() {return this->fb_type;}
			const double get_fb_lowFreqHz() {return this->fb_lowFreqHz;}
			const double get_fb_highFreqHz() {return this->fb_highFreqHz;}
			const double get_fb_nERBs() {return this->fb_nERBs;}
			const uint32_t get_fb_nChannels() {return this->fb_nChannels;}
			const uint32_t get_fb_nGamma() {return this->fb_nGamma;}
			const double get_fb_bwERBs() {return this->fb_bwERBs;}
			const double* get_fb_cfHz() {return this->cfHz.data();}

			// setters
			void set_fb_nGamma(const uint32_t arg) {this->fb_nGamma = arg; this->prepareForProcessing ();}
			void set_fb_bwERBs(const double arg) {this->fb_bwERBs = arg; this->prepareForProcessing ();}

			std::string get_upperProcName() {return this->upperProcPtr->getName();}

	}; /* class GammatoneProc */
}; /* namespace openAFE */

#endif /* GAMMATONEPROC_HPP */
