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
/*

		std::vector<float> cfHz            // Filters center frequencies
        std::vector<float> nERBs           // Distance between neighboring filters in ERBs
        float nGamma          				// Gammatone order of the filters
        float bwERBs          				// Bandwidth of the filters in ERBs
        float lowFreqHz       				// Lowest center frequency used at instantiation
        float highFreqHz      				// Highest center frequency used at instantiation
        
        */
namespace openAFE {

	template<typename T>
	class GamamtoneProc : public Processor < PreProc<T>, TimeDomainSignal<T>, TimeFrequencySignal<T> > {

		private:

			using PB = Processor< PreProc<T>, TimeDomainSignal<T>, TimeFrequencySignal<T> >;

			using inT_nTwoCTypeBlockAccessorPtr = typename PB::inT_nTwoCTypeBlockAccessorPtr;					
			using outT_nTwoCTypeBlockAccessorPtr = typename PB::outT_nTwoCTypeBlockAccessorPtr;	

			using inputPtrIterator = typename PB::inputPtrIterator;
			using outT_SignalIter = typename PB::outT_SignalIter;
			
			typedef std::shared_ptr< GamamtoneProc<T> > processorSharedPtr;
			
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
				
			}
			
			void populateFilters() {
				
				
			}

									
		public:
		
			using typename PB::outT_SignalSharedPtr;

			/* PreProc */
			GamamtoneProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, apf::parameter_map& paramsArg) : PB (fsIn, fsOut, _gammatone) {

				/* Setting the user's parameters */
				this->processorParams = paramsArg;
				
				/* Setting the name of this processor and other informations */
				this->setPInfo(nameArg);
			}
				
			~GamamtoneProc () {}
			
			void processChunk ( ) {
				
			}
			
			void prepareForProcessing () {
				
				this->populateFilters();
			}			

	}; /* class GamamtoneProc */
}; /* namespace openAFE */

#endif /* GAMMATONEPROC_HPP */
