#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <apf/parameter_map.h>
#include <memory>
#include <string>
#include <stdint.h>

namespace openAFE {

	using apfMap = apf::parameter_map;
	typedef apfMap::const_iterator mapIterator;

	/* The type of the processing */
	enum procType {
		_unknow,
		_inputProc,
		_preProc,
		_gammatone,
		_ihc,
		_ild		
	};
	
	/* The father class for all processors in rosAFE */
	class Processor {
		
		protected:
			
			procType type;						// The type of this processing
			std::string name;
			bool hasTwoOutputs; 				// Flag indicating the need for two outputs
			uint64_t nfr;
			
			uint32_t fsIn;						// Sampling frequency of input (i.e., prior to processing)
			uint32_t fsOut;			 			// Sampling frequency of output (i.e., resulting from processing)
			
			double bufferSize_s;
			
		public:
					
			/* PROCESSOR Super-constructor of the processor class
			  * 
			  * INPUT ARGUMENTS:
			  * fsIn : Input sampling frequency (Hz)
			  * fsOut : Output sampling frequency (Hz)
			  * procName : Name of the processor to implement
			  * parObj : Parameters instance to use for this processor
			  */
			Processor (const double bufferSize_s, const uint32_t fsIn, const uint32_t fsOut, const std::string& nameArg, procType typeArg) {
								
				this->fsIn = fsIn;
				this->fsOut = fsOut;
				this->type = typeArg;
				this->bufferSize_s = bufferSize_s;
								
				this->name = nameArg;
					
				switch ( typeArg ) {
					case _inputProc:
					  this->hasTwoOutputs = true;
					  break;
					case _preProc:
					  this->hasTwoOutputs = true;
					  break;
					case _gammatone:
					  this->hasTwoOutputs = true;
					  break;
					case _ihc:
					  this->hasTwoOutputs = true;
					  break;
					case _ild:
					  this->hasTwoOutputs = false;
					  break;
					default:
					  this->hasTwoOutputs = true;
					  break;
					}						
			}
			
			~Processor () {	}
			
			/* PROCESSOR abstract methods (to be implemented by each subclasses): */
			/* PROCESSCHUNK : Returns the output from the processing of a new chunk of input */
			virtual void processChunk () = 0;
			/* RESET : Resets internal states of the processor, if any */
			virtual void reset () = 0;
            
            virtual void prepareForProcessing () = 0;

			/* Returns a const reference of the type of this processor */		
			const procType getType() {
				return type;
			}
			
			/* Compare only the information of the two processors */
			const bool compareBase ( Processor& toCompare ) {
				if ( ( this->name == toCompare.getName() ) && ( this->type == toCompare.getType() ) )
					return true;
				return false;
			}

			const uint32_t getFsOut() {
				return this->fsOut;
			}

			const uint32_t getFsIn() {
				return this->fsIn;
			}
			
			const uint64_t getNFR() {
				return this->nfr;
			}
			
			void setNFR ( const uint64_t nfrArg ) {
				this->nfr = nfrArg;
			}
			
			const std::string getName() {
				return this->name;
			}							
			
			const double getBufferSize_s() {
				return this->bufferSize_s;
			}

			virtual std::string get_upperProcName()	= 0;										
	};

};


#endif /* PROCESSOR_HPP */
