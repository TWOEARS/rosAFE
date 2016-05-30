#ifndef ILDPROC_HPP
#define ILDPROC_HPP
	
#include "../Signals/TimeFrequencySignal.hpp"

#include "Processor.hpp"


/* 
 * ILD Proc :
 * 
 * 
 * */
 
namespace openAFE {

	template<typename T>
	class ILDProc : public Processor < IHCProc<T>, TimeFrequencySignal<T>, TimeFrequencySignal<T> > {

		private:

			using PB = Processor< IHCProc<T>, TimeFrequencySignal<T>, TimeFrequencySignal<T> >;

			using inT_nTwoCTypeBlockAccessorPtr = typename PB::inT_nTwoCTypeBlockAccessorPtr;					
			using outT_nTwoCTypeBlockAccessorPtr = typename PB::outT_nTwoCTypeBlockAccessorPtr;	

			using inputPtrIterator = typename PB::inputPtrIterator;
			using outT_SignalIter = typename PB::outT_SignalIter;
			
			typedef std::shared_ptr< IHCProc<T> > processorSharedPtr;
			
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

									
		public:
		
			using typename PB::outT_SignalSharedPtr;

			/* PreProc */
			ILDProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, apf::parameter_map& paramsArg) : PB (fsIn, fsOut, _ihc) {

				/* Setting the user's parameters */
				this->processorParams = paramsArg;
				
				/* Setting the name of this processor and other informations */
				this->setPInfo(nameArg);
			}
				
			~ILDProc () {
				std::cout << "Destructor of a ILD processor" << std::endl;
			}
			
			void processChunk ( ) {
				
			}			

	}; /* class ILDProc */
}; /* namespace openAFE */

#endif /* ILDPROC_HPP */
