#ifndef PREPROC_HPP
#define PREPROC_HPP

#include "../Signals/TimeDomainSignal.hpp"
#include "Processor.hpp"

/* 
 * inputProc is the first processor to receve the audio.
 * It has two time domain signals as output (left and right).
 * As input, it can accept just one dimentinal and continous data
 * per channel.
 * 
 * It has no parameters. As processing, it normalises the input signal.
 * */
 
#include "preProcLib/preProcLib.hpp"

namespace openAFE {

	template<typename T>
	class PreProc : public Processor < TimeDomainSignal<T>, TimeDomainSignal<T> > {

		private:

			using PB = Processor<TimeDomainSignal<T>, TimeDomainSignal<T> >;

			using typename PB::inT_SignalSharedPtr;
			using typename PB::inT_SignalSharedPtrVector;
			using typename PB::inT_SignalIter;
			
			using typename PB::outT_SignalSharedPtr;
			using typename PB::outT_SignalSharedPtrVector;
			using typename PB::outT_SignalIter;

			using typename PB::inT_nTwoCTypeBlockAccessorPtr;
			using typename PB::inT_nTwoCTypeBlockAccessorPtrVector;
			using typename PB::inT_AccessorIter;

			using typename PB::outT_nTwoCTypeBlockAccessorPtr;
			using typename PB::outT_nTwoCTypeBlockAccessorPtrVector;
			using typename PB::outT_AccessorIter;
			
			void setDefaultParams () {
						
				PB::defaultParams.set("bRemoveDC", 0);
				PB::defaultParams.set("cutoffHzDC", 20);
				PB::defaultParams.set("bPreEmphasis", 0);
				PB::defaultParams.set("coefPreEmphasis", 0.97);
				PB::defaultParams.set("bNormalizeRMS", 0);
				PB::defaultParams.set("bBinauralRMS", 1);
				PB::defaultParams.set("intTimeSecRMS", 500E-3);
				PB::defaultParams.set("bLevelScaling", 0);
				PB::defaultParams.set("refSPLdB", 100);
				PB::defaultParams.set("bMiddleEarFiltering", 0);
				PB::defaultParams.set("middleEarModel", "jespen");
				PB::defaultParams.set("bUnityComp", 1);
			
			}
			
			void setPInfo(const std::string& nameArg,
						  const std::string& labelArg = "Pre-processing stage",
						  const std::string& requestNameArg = "time",
						  const std::string& requestLabelArg = "Time domain signal",
						  const std::string& outputTypeArg = "TimeDomainSignal",
						  unsigned int isBinauralArg = 2 // % Indicates that the processor can behave as mono or binaural						 				 
						) {

				PB::pInfo.name = nameArg;
				PB::pInfo.label = labelArg;
				PB::pInfo.requestName = requestNameArg;
				PB::pInfo.requestLabel = requestLabelArg;
				PB::pInfo.outputType = outputTypeArg;
				PB::pInfo.isBinaural = isBinauralArg;
			}

            void verifyParameters() {
				
				/* TODO : Follow Matlab AFE to update this function
				 * Nothing at : 17.02.2016
				 */
				
			}

			/* Pointers to Filter Objects */
			// dcFilter_l | dcFilter_r | preEmphFilter_l | preEmphFilter_r | agcFilter_l | agcFilter_r | midEarFilter_l | midEarFilter_r | meFilterPeakdB
									
		public:

			/* PreProc */
			PreProc (const std::string nameArg, const uint64_t fsIn, const uint64_t fsOut, const uint64_t bufferSize_s, apf::parameter_map& paramsArg) : Processor < TimeDomainSignal<T>, TimeDomainSignal<T> > (fsIn, fsOut, _preProc) {
				
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
				PB::outputSignals.push_back( std::move( leftOutput ) );
				PB::outputSignals.push_back( std::move( rightOutput ) );
				
				/* Linking the output accesors of each signal */
				PB::linkAccesors ();
				
				/* This processor can take two inputs and two outputs */
				// Processor::isBinaural = true;
				PB::hasTwoOutputs = true;
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
				
				//doNothing( );
			}
						
			/* This funcion publishes (appends) the signals to the outputs of the processor */			
			void appendChunk () {
								
			}
			
			/* TODO : Resets the internat states. */		
			void reset() {
				PB::reset();
			}							

	}; /* class PreProc */
}; /* namespace openAFE */

#endif /* PREPROC_HPP */
