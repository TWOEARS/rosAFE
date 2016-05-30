#ifndef GAMMATONEPROC_HPP
#define GAMMATONEPROC_HPP
	
#include "../Signals/TimeDomainSignal.hpp"
#include "../Signals/TimeFrequencySignal.hpp"

#include "Processor.hpp"


/* 
 * GamamtoneProc :
 * 
 * 
 * */
 
#include "../Filters/GammatoneFilter.hpp"
#include "../tools/mathTools.hpp"

namespace openAFE {

	template<typename T>
	class GamamtoneProc : public Processor < PreProc, TimeDomainSignal<T>, TimeFrequencySignal<T> > {

		private:

			using PB = Processor< PreProc, TimeDomainSignal<T>, TimeFrequencySignal<T> >;

			using inT_nTwoCTypeBlockAccessorPtr = typename PB::inT_nTwoCTypeBlockAccessorPtr;
			using outT_nTwoCTypeBlockAccessorPtr = typename PB::outT_nTwoCTypeBlockAccessorPtr;

			using inputPtrIterator = typename PB::inputPtrIterator;
			using outT_SignalIter = typename PB::outT_SignalIter;
			
			typedef std::shared_ptr< GamamtoneProc<T> > processorSharedPtr;
			
			std::vector<float> cfHz;            								// Filters center frequencies
			std::vector<float> nERBs;           								// Distance between neighboring filters in ERBs
			float nGamma;          							    				// Gammatone order of the filters
			float bwERBs;          							    				// Bandwidth of the filters in ERBs
			float lowFreqHz;       							   	 				// Lowest center frequency used at instantiation
			float highFreqHz;      							    				// Highest center frequency used at instantiation
        
            typedef std::shared_ptr < GammatoneFilter > gammatoneFilterPtr;
            typedef std::vector <gammatoneFilterPtr > filterPtrVector;
			filterPtrVector leftFilters, rightFilters;		    				// Array of filter pointer objects
						
			void setToDefaultParams () {
						
				this->processorParams.set("toFill", 0);
			}
			
			void setPInfo(const std::string& nameArg,
						  const std::string& labelArg = "to fill",
						  const std::string& requestNameArg = "to fill",
						  const std::string& requestLabelArg = "to fill",
						  const std::string& outputTypeArg = "to fill",
						  unsigned int isBinauralArg = 2 // % Indicates that the processor can behave as mono or binaural
						) {

				this->pInfo.name = nameArg;
				this->pInfo.label = labelArg;
				this->pInfo.requestName = requestNameArg;
				this->pInfo.requestLabel = requestLabelArg;
				this->pInfo.outputType = outputTypeArg;
				this->pInfo.isBinaural = isBinauralArg;
			}

            void verifyParameters() {

				const apfMap map = this->getCurrentParameters();				
				/* Solve the conflicts between center frequencies, number of channels, and
				 * distance between channels.
				 */
		 /*       if ~isempty(pObj.parameters.map('fb_cfHz'))
					 Highest priority case: a vector of channels center 
					%   frequencies is provided
					centerFreq = pObj.parameters.map('fb_cfHz');
					
					pObj.parameters.map('fb_lowFreqHz') = centerFreq(1);
					pObj.parameters.map('fb_highFreqHz') = centerFreq(end);
					pObj.parameters.map('fb_nChannels') = numel(centerFreq);
					pObj.parameters.map('fb_nERBs') = 'n/a'; */ 

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
								
		public:
		
			using typename PB::outT_SignalSharedPtr;

			/* PreProc */
			GamamtoneProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, apf::parameter_map& paramsArg) : PB (fsIn, fsOut, _gammatone) {

				/* Setting the user's parameters */
				this->processorParams = paramsArg;
				
				/* Setting the name of this processor and other informations */
				this->setPInfo(nameArg);
				
				
				this->verifyParameters();
				this->prepareForProcessing();
			}
				
			~GamamtoneProc () {
				leftFilters.clear();
				rightFilters.clear();
			}
			
			void processChunk ( ) {
				
			}
			
			void prepareForProcessing () {
				
				this->populateFilters( leftFilters );
				this->populateFilters( rightFilters );
			}			

	}; /* class GamamtoneProc */
}; /* namespace openAFE */

#endif /* GAMMATONEPROC_HPP */
