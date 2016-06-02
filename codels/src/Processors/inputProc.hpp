#ifndef INPUTPROC_HPP
#define INPUTPROC_HPP

#define MAXCODABLEVALUE 2147483647;

#include <thread>

#include "TDSProcessor.hpp"
#include "../tools/mathTools.hpp"

/* 
 * inputProc is the first processor to receve the audio.
 * It has two time domain signals as output (left and right).
 * As input, it can accept just one dimentinal and continous data
 * per channel.
 * 
 * It has no parameters. As processing, it normalises the input signal.
 * */
 
namespace openAFE {

	class InputProc : public TDSProcessor<float> {
					
		private:
									
			void process ( float* firstValue, size_t dim ) {
				for ( unsigned int i = 0 ; i < dim ; ++i )
					*( firstValue + i ) = *( firstValue + i ) / MAXCODABLEVALUE;
			}

		public:

			InputProc (const std::string nameArg, const uint32_t fs, const uint32_t bufferSize_s) : TDSProcessor<float> (nameArg, fs, fs, bufferSize_s, _inputProc) {
				
			}
				
			~InputProc () {	}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not publiched yet on the output vectors.
			 */
			void processChunk (float* inChunkLeft, size_t leftDim, float* inChunkRight, size_t rightDim ) {
				
				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendTChunk( inChunkLeft, leftDim );
				rightPMZ->appendTChunk( inChunkRight, rightDim );
				
				std::shared_ptr<twoCTypeBlock<float> > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::shared_ptr<twoCTypeBlock<float> > r_PMZ = rightPMZ->getLastChunkAccesor();
					
				// 0- Initialization
				unsigned long dim1_l = l_PMZ->array1.second;
				unsigned long dim2_l = l_PMZ->array2.second;
				unsigned long dim1_r = r_PMZ->array1.second;
				unsigned long dim2_r = r_PMZ->array2.second;
							
				float* firstValue1_l = l_PMZ->array1.first;
				float* firstValue2_l = l_PMZ->array2.first;
				float* firstValue1_r = r_PMZ->array1.first;
				float* firstValue2_r = r_PMZ->array2.first;				
					
				if ( ( dim1_l > 0 ) && ( dim1_r > 0 ) ) {
					std::thread leftThread( &InputProc::process, this, firstValue1_l, dim1_l );
					std::thread rightThread( &InputProc::process, this, firstValue1_r, dim1_r );
					
					leftThread.join();                // pauses until left finishes
					rightThread.join();               // pauses until right finishes
				}
				if ( ( dim2_l > 0 ) && ( dim2_r > 0 ) )	{
					std::thread leftThread( &InputProc::process, this, firstValue2_l, dim2_l );
					std::thread rightThread( &InputProc::process, this, firstValue2_r, dim2_r );
					
					leftThread.join();                // pauses until left finishes
					rightThread.join();               // pauses until right finishes
				}			
			}
			
			void processChunk () { }
						
			void prepareForProcessing () { }

			/* Comapres informations and the current parameters of two processors */
			bool operator==( InputProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					return true;
				return false;
			}
			 
	}; /* class InputProc */
}; /* namespace openAFE */

#endif /* INPUTPROC_HPP */
