#ifndef PREPROC_HPP
#define PREPROC_HPP
	
#include "../Signals/TimeDomainSignal.hpp"
#include "Processor.hpp"


/* 
 * preProc :
 * 
 * 
 * */
 
#include "preProcLib/preProcLib.hpp"

namespace openAFE {

	template<typename T>
	class PreProc : public Processor < InputProc<T>, TimeDomainSignal<T>, TimeDomainSignal<T> > {

		private:

			using PB = Processor< InputProc<T>, TimeDomainSignal<T>, TimeDomainSignal<T> >;

			using inT_nTwoCTypeBlockAccessorPtr = typename PB::inT_nTwoCTypeBlockAccessorPtr;					
			using outT_nTwoCTypeBlockAccessorPtr = typename PB::outT_nTwoCTypeBlockAccessorPtr;	

			using inputPtrIterator = typename PB::inputPtrIterator;
			using outT_SignalIter = typename PB::outT_SignalIter;
			
			typedef std::shared_ptr< PreProc<T> > processorSharedPtr;
			
			void setDefaultParams () {
						
				this->defaultParams.set("bRemoveDC", 0);
				this->defaultParams.set("cutoffHzDC", 20);
				this->defaultParams.set("bPreEmphasis", 0);
				this->defaultParams.set("coefPreEmphasis", 0.97);
				this->defaultParams.set("bNormalizeRMS", 0);
				this->defaultParams.set("bBinauralRMS", 1);
				this->defaultParams.set("intTimeSecRMS", 500E-3);
				this->defaultParams.set("bLevelScaling", 0);
				this->defaultParams.set("refSPLdB", 100);
				this->defaultParams.set("bMiddleEarFiltering", 0);
				this->defaultParams.set("middleEarModel", "jespen");
				this->defaultParams.set("bUnityComp", 1);
			
			}
			
			void setPInfo(const std::string& nameArg,
						  const std::string& labelArg = "Pre-processing stage",
						  const std::string& requestNameArg = "time",
						  const std::string& requestLabelArg = "Time domain signal",
						  const std::string& outputTypeArg = "TimeDomainSignal",
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
				
				/* TODO : Follow Matlab AFE to update this function
				 * Nothing at : 17.02.2016
				 */
				
			}

			/* Pointers to Filter Objects */
			// dcFilter_l | dcFilter_r | preEmphFilter_l | preEmphFilter_r | agcFilter_l | agcFilter_r | midEarFilter_l | midEarFilter_r | meFilterPeakdB
									
		public:
		
			using typename PB::outT_SignalSharedPtr;

			/* PreProc */
			PreProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, apf::parameter_map& paramsArg) : PB (fsIn, fsOut, _preProc) {
				
				this->setDefaultParams ();

				/* Setting the user's parameters */
				this->processorParams = paramsArg;
				
				/* Extending with default parameters */			
				this->extendParameters ();
				/* Setting the name of this processor and other informations */
				this->setPInfo(nameArg);
				
				/* Creating the output signals */
				outT_SignalSharedPtr leftOutput( new TimeDomainSignal<T>(fsOut, bufferSize_s, this->pInfo.requestName, this->pInfo.name, _left) );
				outT_SignalSharedPtr rightOutput ( new TimeDomainSignal<T>(fsOut, bufferSize_s, this->pInfo.requestName, this->pInfo.name, _right) );
				
				/* Setting those signals as the output signals of this processor */
				this->outputSignals.push_back( std::move( leftOutput ) );
				this->outputSignals.push_back( std::move( rightOutput ) );
				
				/* Linking the output accesors of each signal */
				this->linkAccesors ();
				
				/* This processor can take two inputs and two outputs */
				// Processor::isBinaural = true;
				this->hasTwoOutputs = true;
	
				/* Creating the PMZ signals */
				outT_SignalSharedPtr outLeftPMZ( new TimeDomainSignal<T>(fsOut, bufferSize_s, "Left TDS PMZ", "Left TDS PMZ", _left) );
				outT_SignalSharedPtr outRightPMZ( new TimeDomainSignal<T>(fsOut, bufferSize_s, "Right TDS PMZ", "Right TDS PMZ", _right) );
				
				/* Setting those signals as the PMZ signals of this processor */
				this->outPrivateMemoryZone.push_back( std::move( outLeftPMZ ) );
				this->outPrivateMemoryZone.push_back( std::move( outRightPMZ ) );
			}
				
			~PreProc () {
				std::cout << "Destructor of a pre processor" << std::endl;
			}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not publiched yet on the output vectors.
			 */
			void processChunk () {
				inputPtrIterator it = this->inputProcessors.processorVector.begin();
   
				int i = 0;
			    for ( outT_SignalIter itPMZ = this->outPrivateMemoryZone.begin() ; itPMZ != this->outPrivateMemoryZone.end() ; ++itPMZ) {
					(*itPMZ)->appendChunk( ((*it)->getLastChunkAccesor())[i++] );
					(*itPMZ)->calcLastChunk();
					preProcLib::multiply ((*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue, (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim, 2147483647);
					preProcLib::multiply ((*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue, (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim, 2147483647);
				}
			}
									
			/* TODO : Resets the internat states. */		
			void reset() {
				PB::reset();
			}							

	}; /* class PreProc */
}; /* namespace openAFE */

#endif /* PREPROC_HPP */
