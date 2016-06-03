#ifndef TDSPROCESSOR_HPP
#define TDSPROCESSOR_HPP

#include <thread>

#include "Processor.hpp"
#include "../Signals/TimeDomainSignal.hpp"

namespace openAFE {
   template<typename T>
   class TDSProcessor : public Processor {
	   
	   protected:
            			
			std::shared_ptr <TimeDomainSignal<T> > leftOutput, rightOutput;
			std::shared_ptr <TimeDomainSignal<T> > leftPMZ, rightPMZ;
						
		public:

		    TDSProcessor (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, procType typeArg) : Processor (fsIn, fsOut, nameArg, typeArg) {
				
				/* Creating the output signals */
				if ( this->hasTwoOutputs == true ) {
					leftOutput.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, nameArg, _left) );
					rightOutput.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, nameArg, _right) );
							
					/* Creating the PMZ signals */
					leftPMZ.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, "Left_PMZ", _left) );
					rightPMZ.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, "Right_PMZ", _right) );
				} else {
					leftOutput.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, nameArg, _mono ) );
							
					/* Creating the PMZ signals */
					leftPMZ.reset( new TimeDomainSignal<float>(fsOut, bufferSize_s, "Mono_PMZ", _mono ) );					
				}
			}

			~TDSProcessor() {
				
				leftOutput.reset();
				rightOutput.reset();
				leftPMZ.reset();
				rightPMZ.reset();
			}
			
			void reset() {
				leftOutput->reset();
				rightOutput->reset();
				leftPMZ->reset();
				rightPMZ->reset();
			}

			std::shared_ptr<twoCTypeBlock<T> > getLeftLastChunkAccessor() {
				return this->leftOutput->getLastChunkAccesor();
			}

			std::shared_ptr<twoCTypeBlock<T> > getRightLastChunkAccessor() {
				return this->rightOutput->getLastChunkAccesor();
			}

			std::shared_ptr<twoCTypeBlock<T> > getLeftWholeBufferAccessor() {
				return this->leftOutput->getWholeBufferAccesor();
			}

			std::shared_ptr<twoCTypeBlock<T> > getRightWholeBufferAccessor() {
				return this->rightOutput->getWholeBufferAccesor();
			}
			
			void releaseChunk () {
				if ( this->hasTwoOutputs ) {						
					std::thread leftAppendThread( &TimeDomainSignal<float>::appendChunk, this->leftOutput, leftPMZ->getLastChunkAccesor() );
					std::thread rightAppendThread( &TimeDomainSignal<float>::appendChunk, this->rightOutput, rightPMZ->getLastChunkAccesor() );
						
					leftAppendThread.join();                // pauses until left finishes
					rightAppendThread.join();               // pauses until right finishes
				}	
			}
									
	}; /* TDSProcessor */

}; /* namespace openAFE */			
#endif /* TDSPROCESSOR_HPP */			