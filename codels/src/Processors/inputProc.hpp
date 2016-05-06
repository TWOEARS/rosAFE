#ifndef INPUTPROC_HPP
#define INPUTPROC_HPP

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
 
#include "inputProcLib/inputProcLib.hpp"

namespace openAFE {

	template<typename T>
	class InputProc : public Processor < InputProc<T>, TimeDomainSignal<T>, TimeDomainSignal<T> > {
		public:

			using PB = Processor<InputProc<T>, TimeDomainSignal<T>, TimeDomainSignal<T> >;
			
			using inT_nTwoCTypeBlockAccessorPtr = typename PB::inT_nTwoCTypeBlockAccessorPtr;					
			using outT_nTwoCTypeBlockAccessorPtr = typename PB::outT_nTwoCTypeBlockAccessorPtr;
			
			typedef std::shared_ptr< InputProc<T> > processorSharedPtr;
					
		private:
			
			using typename PB::outT_SignalSharedPtr;
			
			void setToDefaultParams () {
				this->processorParams.set("Type", "Input Processor");
			}
			
			void setPInfo(const std::string& nameArg,
						  const std::string& labelArg = "Input Processor",
						  const std::string& requestNameArg = "input",
						  const std::string& requestLabelArg = "TimeDomainSignal",
						  const std::string& outputTypeArg = "TimeDomainSignal",
						  unsigned int isBinauralArg = 1						 				 
						) {

				this->pInfo.name = nameArg;
				this->pInfo.label = labelArg;
				this->pInfo.requestName = requestNameArg;
				this->pInfo.requestLabel = requestLabelArg;
				this->pInfo.outputType = outputTypeArg;
				this->pInfo.isBinaural = isBinauralArg;
			}
			
		public:

			/* inputProc */
			InputProc (const std::string& nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s) : PB (fsIn, fsOut, _inputProc) {
				
				this->setToDefaultParams ();

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
			}
				
			~InputProc () {
				std::cout << "Destructor of a input processor" << std::endl;
			}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not publiched yet on the output vectors.
			 */
			void processChunk (T* inChunkLeft, uint32_t leftDim, T* inChunkRight, uint32_t rightDim) {
				
				/* There is just one dimention */
				std::thread leftThread(inputProcLib::normaliseData<T>, inChunkLeft, leftDim);
				std::thread rightThread(inputProcLib::normaliseData<T>, inChunkRight, rightDim);
				
				leftThread.join();                // pauses until left finishes
				rightThread.join();               // pauses until right finishes
			}
						
			/* This funcion publishes (appends) the signals to the outputs of the processor */			
			void appendChunk (T* inChunkLeft, uint32_t leftDim, T* inChunkRight, uint32_t rightDim) {
								
				std::thread leftAppendThread( &TimeDomainSignal<T>::appendTChunk, this->outputSignals[0], inChunkLeft, leftDim);
				std::thread rightAppendThread( &TimeDomainSignal<T>::appendTChunk, this->outputSignals[1], inChunkRight, rightDim);
				
				leftAppendThread.join();                // pauses until left finishes
				rightAppendThread.join();               // pauses until right finishes
			}
			
			/* TODO : Resets the internal states. */		
			void reset() {
				PB::reset();
			}		

	}; /* class InputProc */
}; /* namespace openAFE */

#endif /* INPUTPROC_HPP */
