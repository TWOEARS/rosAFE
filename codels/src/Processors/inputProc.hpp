#ifndef INPUTPROC_HPP
#define INPUTPROC_HPP

#include "../Signals/TimeDomainSignal.hpp"
#include "Processor.hpp"

#include <assert.h>
#include <memory>
#include <iostream>
#include <thread>

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
	class InputProc : public Processor {

		private:
			
			typedef std::shared_ptr<TimeDomainSignal<T> > TimeDomainSignalSharedPtr;
			typedef std::shared_ptr<nTwoCTypeBlockAccessor<T> > nTwoCTypeBlockAccessorPtr;

			TimeDomainSignalSharedPtr leftOutput, rightOutput;
						
			void setDefaultParams () {
				Processor::defaultParams.set("Type", "Input Processor");
			}
			
			void setPInfo(const std::string& nameArg,
						  const std::string& labelArg = "Input Processor",
						  const std::string& requestNameArg = "input",
						  const std::string& requestLabelArg = "TimeDomainSignal",
						  const std::string& outputTypeArg = "TimeDomainSignal",
						  unsigned int isBinauralArg = 1						 				 
						) {

				Processor::pInfo.name = nameArg;
				Processor::pInfo.label = labelArg;
				Processor::pInfo.requestName = requestNameArg;
				Processor::pInfo.requestLabel = requestLabelArg;
				Processor::pInfo.outputType = outputTypeArg;
				Processor::pInfo.isBinaural = isBinauralArg;
			}

		public:

			/* inputProc   This processor multiply the input chunk with the "Multiple" parameter ! */
			InputProc (const std::string& nameArg, const uint64_t fsIn, const uint64_t fsOut, const uint64_t bufferSize_s) : Processor (fsIn, fsOut, _inputProc) {
				
				this->setDefaultParams ();

				/* Extending with default parameters */			
				this->extendParameters ();
				/* Setting the name of this processor and other informations */
				this->setPInfo(nameArg);
				
				/* Creating the output signals */
				TimeDomainSignalSharedPtr left ( new TimeDomainSignal<T>(fsOut, bufferSize_s, this->pInfo.requestName, this->pInfo.name, _left) );
				TimeDomainSignalSharedPtr right ( new TimeDomainSignal<T>(fsOut, bufferSize_s, this->pInfo.requestName, this->pInfo.name, _right) );

				/* Setting those signals as the output signals of this processor */
				leftOutput = std::move ( left );
				rightOutput = std::move ( right );
			}
				
			~InputProc () {
				std::cout << "Destructor of a input processor" << std::endl;
			}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not publiched yet on the output vectors.
			 */
			void processChunk (T* inChunkLeft, uint64_t leftDim, T* inChunkRight, uint64_t rightDim) {
				
				/* There is just one dimention */
				std::thread leftThread(inputProcLib::normaliseData<T>, inChunkLeft, leftDim);
				std::thread rightThread(inputProcLib::normaliseData<T>, inChunkRight, rightDim);
				
				leftThread.join();                // pauses until left finishes
				rightThread.join();               // pauses until right finishes
			}
						
			/* This funcion published (appends) the signals to the outputs of the processor */			
			void appendChunk (T* inChunkLeft, uint64_t leftDim, T* inChunkRight, uint64_t rightDim) {
								
				std::thread leftAppendThread( &TimeDomainSignal<T>::appendChunk, this->leftOutput, inChunkLeft, leftDim);
				std::thread rightAppendThread( &TimeDomainSignal<T>::appendChunk, this->rightOutput, inChunkRight, rightDim);
				
				leftAppendThread.join();                // pauses until left finishes
				rightAppendThread.join();               // pauses until right finishes
			}

			nTwoCTypeBlockAccessorPtr getLastChunk(bool right = false) {
				if ( right == true )
					return this->rightOutput->getLastChunk();
				else
					return this->leftOutput->getLastChunk();
			}
			
			nTwoCTypeBlockAccessorPtr getLastData( uint64_t samplesArg, bool right = false  ) {
				if ( right == true )
					return this->rightOutput->getLastData( samplesArg );
				else
					return this->leftOutput->getLastData( samplesArg );
			}
			
			nTwoCTypeBlockAccessorPtr getOldData( bool right = false, uint64_t samplesArg = 0 ) {
				if ( right == true )
					return this->rightOutput->getOldData( samplesArg );
				else
					return this->leftOutput->getOldData( samplesArg );
			}

			nTwoCTypeBlockAccessorPtr getWholeBuffer(bool right = false) {
				if ( right == true )
					return this->rightOutput->getWholeBuffer();
				else
					return this->leftOutput->getWholeBuffer();
			}
					
			void reset() {
				// TODO : reset the signals.
			}
			
	};
};

#endif /* PREPROC_HPP */
