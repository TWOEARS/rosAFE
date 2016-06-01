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

	class GamamtoneProc : public TFSProcessor<float> {

		private:

			std::vector<float> cfHz;            								// Filters center frequencies
			std::vector<float> nERBs;           								// Distance between neighboring filters in ERBs
			float nGamma;          							    				// Gammatone order of the filters
			float bwERBs;          							    				// Bandwidth of the filters in ERBs
			float lowFreqHz;       							   	 				// Lowest center frequency used at instantiation
			float highFreqHz;      							    				// Highest center frequency used at instantiation
			
			void setToDefaultParams () {
						
			}

            size_t verifyParameters( apf::parameter_map& paramsArg ) {
				
				this->setToDefaultParams();
				
				// Setting the user's parameters
				this->givenParameters ( paramsArg );
				
				const apfMap map = this->getCurrentParameters();	
				/* Solve the conflicts between center frequencies, number of channels, and
				 * distance between channels.
				 */
					if ( map.get<unsigned short>("fb_nChannels") != 0 ) {
						/* Medium priority: frequency range and number of channels
						 *  are provided.
						 */ 
					   
						// Build a vector of center ERB frequencies
						std::vector<float> ERBS = linspace( freq2erb(map.get<float>("fb_lowFreqHz")),
															freq2erb(map.get<float>("fb_highFreqHz")),
															map.get<unsigned short>("fb_nChannels") ); 
						cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), cfHz.data() );    // Convert to Hz
									
						this->processorParams.set("fb_nERBs", ( *(ERBS.end()-1) - *(ERBS.begin()) ) / map.get<unsigned short>("fb_nChannels") );
					}
					else {
						/* Lowest (default) priority: frequency range and distance 
						 *   between channels is provided (or taken by default).
						 */
						
						// Build vector of center ERB frequencies
						std::vector<float> ERBS;
						for ( float tmp = freq2erb( map.get<float>("fb_lowFreqHz") ) ; tmp < freq2erb( map.get<float>("fb_highFreqHz") ) ; tmp += map.get<float>("fb_nERBs") )
							ERBS.push_back( tmp );
		
						cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), cfHz.data() );    // Convert to Hz
						
						this->processorParams.set("fb_nChannels", cfHz.size());														
					}
					return map.get<unsigned short>("fb_nChannels");
			}
			
			void populateFilters( filterPtrVector& filters ) {
				
				const apfMap map = this->getCurrentParameters();
				
				uint32_t nFilter = cfHz.size();
				uint32_t n = map.get<unsigned short>("fb_nGamma");
				float bw = map.get<float>("fb_bwERBs");
				const float fs = this->getFsIn();

				filters.resize( cfHz.size() );
								
				for ( unsigned int ii = 0 ; ii < nFilter ; ++ii ) {
					gammatoneFilterPtr thisFilter( new GammatoneFilter( *( this->cfHz.data() + ii ), fs, n, bw ) );
					filters[ ii ] = thisFilter;
				}
			}

			void process ( filterPtrVector& filters, T* firstValue, size_t dim, signalPtr pmz ) {

/*				std::vector<std::thread> threads;
				  for ( uint32_t ii = 0; ii < map.get<unsigned short>("fb_nChannels") ; ++ii )
					threads.push_back(std::thread( &GammatoneFilter::exec, filters[ii], firstValue, dim,  ));

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();				*/

			}

            typedef std::shared_ptr < GammatoneFilter > gammatoneFilterPtr;
            typedef std::vector <gammatoneFilterPtr > filterPtrVector;
			filterPtrVector leftFilters, rightFilters;		    				// Array of filter pointer objects
														
		public:
		
			GamamtoneProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, std::shared_ptr<PreProc > upperProcPtr, apf::parameter_map& paramsArg) : TFSProcessor<float> (nameArg, fsIn, fsOut, bufferSize_s, verifyParameters( paramsArg ), "magnitude", _gammatone)

				this->upperProcPtr = upperProcPtr;				
				this->prepareForProcessing ();
			}
				
			~GamamtoneProc () {
				leftFilters.clear();
				rightFilters.clear();
			}
			
			void processChunk ( ) {
				
				this->setNFR ( this->upperProcPtr->getNFR() );
				
				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				
				std::vector<std::shared_ptr<twoCTypeBlock<T> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<T> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
				/* Continue HERE - Append Chunk is not done yet !!! */
											    
			}
			
			void prepareForProcessing () {
				
				this->populateFilters( leftFilters );
				this->populateFilters( rightFilters );
			}

	}; /* class GamamtoneProc */
}; /* namespace openAFE */

#endif /* GAMMATONEPROC_HPP */
