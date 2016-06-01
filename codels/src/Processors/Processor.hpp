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
			
			apfMap processorParams;				// The parameters used by this processor
			
			uint32_t fsIn;						// Sampling frequency of input (i.e., prior to processing)
			uint32_t fsOut;			 			// Sampling frequency of output (i.e., resulting from processing)

           /* This method is called at setup of a processor parameters, to verify that the
             * provided parameters are valid, and correct conflicts if needed.
             * Not needed by many processors, hence is not made virtual pure, but need to be
             * overriden in processors where it is needed.
             * */
			virtual void verifyParameters() = 0;

			/* This function fills the defaultParams map with default
			 * parameters for each processor
			 * */
			virtual void setToDefaultParams () = 0;
						
			/* Add default value(s) to missing parameters in a processor */
			void givenParameters( apf::parameter_map& paramsArg ) {
				for(mapIterator it = paramsArg.begin(); it != paramsArg.end(); it++)
					if ( this->processorParams.has_key( it->first ) != 0 )
						this->processorParams.set( it->first, it->second );
					// else do nothing
			}
			
            /* Returns the list of parameters that cannot be affected in real-time. Most
             * (all) of them cannot be modified as they involve a change in dimension of
             * the output signal. A work-around allowing changes in dimension might be
             * implemented in the future but remains a technical challenge at the moment.
             * 
             * To modify one of these parameters, the corresponding processor should be
             * deleted and a new one, with the new parameter value, instantiated via a 
             * user request.
             */
			bool verifyBlacklistedParameters(std::string& paramToChange) {
				if ( ( paramToChange == "fb_type" ) or 
				     ( paramToChange == "fb_lowFreqHz" ) or 
				     ( paramToChange == "fb_highFreqHz" ) or 
				     ( paramToChange == "fb_nERBs" ) or 
				     ( paramToChange == "fb_nCfHz" ) )
					return true;
				return false;
			}
			
		public:
					
			/* PROCESSOR Super-constructor of the processor class
			  * 
			  * INPUT ARGUMENTS:
			  * fsIn : Input sampling frequency (Hz)
			  * fsOut : Output sampling frequency (Hz)
			  * procName : Name of the processor to implement
			  * parObj : Parameters instance to use for this processor
			  */
			Processor (const uint32_t fsIn, const uint32_t fsOut, const std::string& nameArg, procType typeArg) {
								
				this->fsIn = fsIn;
				this->fsOut = fsOut;
				this->type = typeArg;
				
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

			/* GETCURRENTPARAMETERS  This methods returns a list of parameter
			 * values used by a given processor. */
			const apfMap& getCurrentParameters() {
				return this->processorParams;
			}
			
			/* HASPARAMETERS Test if the processor uses specific parameters */
			bool hasParameters(const std::string& parName) {
				return processorParams.has_key( parName );
			}
            
            virtual void prepareForProcessing () = 0;
            
			/* MODIFYPARAMETER Requests a change of value of one parameter.
			 * The modification will be done if the parameter is not blacklisted and if it is a real parameter. */
			bool modifyParameter(std::string parName, std::string newValue) {
				if ( ! verifyBlacklistedParameters(parName) )
					if ( processorParams.has_key( parName ) ) {
						this->processorParams.set( parName, newValue );
						this->prepareForProcessing();
						return true;
					}
				return false;	
			}

			/* Returns a const reference of the type of this processor */		
			const procType getType () {
				return type;
			}
			
			/* Getter methods : This funtion sends a const reference for the asked parameter's value */
			const std::string& getParameter( std::string paramArg ) {
				return this->processorParams[ paramArg ];
			}
			
			/* Compare only the information of the two processors */
			const bool compareInfos (const Processor& toCompare) {
				if ( this->name == toCompare.name )
					return true;
				return false;
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool operator==(const Processor& toCompare) {
				if ( this->compareInfos( toCompare ) )
					if ( this->processorParams == toCompare.processorParams )
						return true;
				return false;
			}

			const uint32_t getFsOut() {
				return this->fsOut;
			}

			const uint32_t getFsIn() {
				return this->fsIn;
			}
			
			const uint64_t getNFR () {
				return this->nfr;
			}
			
			void setNFR ( const uint64_t nfrArg ) {
				this->nfr = nfrArg;
			}
			
			const std::string getName () {
				return this->name;
			}							
	};

};


#endif /* PROCESSOR_HPP */
