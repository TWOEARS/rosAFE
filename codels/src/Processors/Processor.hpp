#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <apf/parameter_map.h>

#include <assert.h>
#include <memory>
#include <thread>
#include <iostream>

#include "ProcessorVector.hpp"


namespace openAFE {

	using apfMap = apf::parameter_map;
	typedef apfMap::const_iterator mapIterator;

	/* The type of the processing */
	enum procType {
		_inputProc,
		_preProc,
		_gammatone,
		_ihc
	};
	
	/* Processor Info struct */            
	struct pInfoStruct {
		std::string name;
		std::string label;
		std::string requestName;
		std::string requestLabel;
		std::string outputType;
		unsigned int isBinaural;		// Flag indicating the need for two inputs
	};
	
	/* The father class for all processors in rosAFE */
	template<typename inProcT, typename inSignalT, typename outSignalT >
	class Processor {
		public:

			using outT_SignalSharedPtr = typename outSignalT::signalSharedPtr;
			using outT_nTwoCTypeBlockAccessorPtr = typename outSignalT::nTwoCTypeBlockAccessorPtr;

			typedef std::vector<outT_nTwoCTypeBlockAccessorPtr >	 						outT_nTwoCTypeBlockAccessorPtrVector;
			typedef typename std::vector<outT_nTwoCTypeBlockAccessorPtrVector >::iterator 	outT_AccessorIter;
			
			using inT_SignalSharedPtr = typename inSignalT::signalSharedPtr;	
			using inT_nTwoCTypeBlockAccessorPtr = typename inSignalT::nTwoCTypeBlockAccessorPtr;
						
			using inProcessorSharedPtr = typename ProcessorVector<inProcT>::processorSharedPtr;

		private:
			
			uint64_t nfr;
						
			/* This function fills the defaultParams map with default
			 * parameters for each processor
			 * */
			virtual void setToDefaultParams () = 0;
			
			/* This function fills the pInfoStruct of the processor */
			virtual void setPInfo(const std::string& nameArg,
						  const std::string& labelArg,
						  const std::string& requestNameArg,
						  const std::string& requestLabelArg,
						  const std::string& outputTypeArg,
						  unsigned int isBinauralArg ) = 0;		  
									
		protected:

			typedef std::vector<outT_SignalSharedPtr > 										outT_SignalSharedPtrVector;
			typedef typename std::vector<outT_SignalSharedPtr >::iterator 					outT_SignalIter;

			typedef std::vector<inT_SignalSharedPtr > 										inT_SignalSharedPtrVector;
			typedef typename std::vector<inT_SignalSharedPtr >::iterator 					intT_SignalIter;
			
			typedef std::vector<inT_nTwoCTypeBlockAccessorPtr >	 							inT_nTwoCTypeBlockAccessorPtrVector;
			typedef typename std::vector<inT_nTwoCTypeBlockAccessorPtrVector >::iterator 	inT_AccessorIter;


		
		    ProcessorVector<inProcT > inputProcessors;
		    using inputPtrIterator = typename ProcessorVector<inProcT >::processorSharedPtrVectorIterator;
		    
			outT_SignalSharedPtrVector outputSignals;

			inT_SignalSharedPtrVector inPrivateMemoryZone;
			outT_SignalSharedPtrVector outPrivateMemoryZone;
			
			outT_nTwoCTypeBlockAccessorPtrVector outT_lastChunkInfo, outT_lastDataInfo, outT_oldDataInfo, outT_wholeBufferInfo;
						
			apfMap processorParams;				// The parameters used by this processor
			// apfMap defaultParams;				// The default parameters of this processor
			
			uint32_t fsIn;						// Sampling frequency of input (i.e., prior to processing)
			uint32_t fsOut;			 			// Sampling frequency of output (i.e., resulting from processing)

			procType type;						// The type of this processing
			pInfoStruct pInfo;					// The informations of this processor
			
			/* Output signas are conserved in this vector.
			 * This is a shared pointer vecor and not a unique pointer vector, because
			 * the same pointer will be placed in dataObject too.
			 */
			//signalBaseSharedPtrVector outputSignals;
			
			//bool isBinaural;         			// Flag indicating the need for two inputs
			bool hasTwoOutputs = false; 		// Flag indicating the need for two outputs
			// channel Channel;					// On which channel (left, right or mono) the processor operates

           /* This method is called at setup of a processor parameters, to verify that the
             * provided parameters are valid, and correct conflicts if needed.
             * Not needed by many processors, hence is not made virtual pure, but need to be
             * overriden in processors where it is needed.
             * */
			virtual void verifyParameters() {}
			
			/* Add default value(s) to missing parameters in a processor 
			void extendParameters() {
				for(mapIterator it = this->defaultParams.begin(); it != this->defaultParams.end(); it++)
					if ( this->processorParams.has_key( it->first ) == 0 )
						this->processorParams.set( it->first, it->second );
					// else do nothing
			} */
			
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
				if ( ( paramToChange == "type" ) or 
				     ( paramToChange == "lowFreqHz" ) or 
				     ( paramToChange == "highFreqHz" ) or 
				     ( paramToChange == "nERBs" ) or 
				     ( paramToChange == "nCfHz" ) )
					return true;
				return false;
			}
			
			void linkAccesors () {
				unsigned int signalNumber = outputSignals.size();
				outT_lastChunkInfo.reserve(signalNumber); outT_lastDataInfo.reserve(signalNumber);
				outT_oldDataInfo.reserve(signalNumber); outT_wholeBufferInfo.reserve(signalNumber);

				for(outT_SignalIter it = outputSignals.begin() ; it != outputSignals.end() ; ++it) {
					outT_lastChunkInfo.push_back ( (*it)->getLastChunkAccesor() );
					outT_lastDataInfo.push_back ( (*it)->getLastDataAccesor() );
					outT_oldDataInfo.push_back ( (*it)->getOldDataAccesor() );
					outT_wholeBufferInfo.push_back ( (*it)->getWholeBufferAccesor() );
				}
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
			Processor (const uint32_t fsIn, const uint32_t fsOut, procType typeArg) {
								
				this->fsIn = fsIn;
				this->fsOut = fsOut;
				this->type = typeArg;
											
			}
			
			~Processor () {
				inputProcessors.clear();
				outputSignals.clear();
				inPrivateMemoryZone.clear();
				outPrivateMemoryZone.clear();
				outT_lastChunkInfo.clear(); outT_lastDataInfo.clear(); outT_oldDataInfo.clear(); outT_wholeBufferInfo.clear();
			}
			
			/* PROCESSOR abstract methods (to be implemented by each subclasses): */
			/* PROCESSCHUNK : Returns the output from the processing of a new chunk of input */
			virtual void processChunk () {} // = 0;
			/* RESET : Resets internal states of the processor, if any */
			virtual void reset () {
				for(outT_SignalIter it = outputSignals.begin() ; it != outputSignals.end() ; ++it)
					(*it)->reset();
			}

			/* GETCURRENTPARAMETERS  This methods returns a list of parameter
			 * values used by a given processor. */
			const apfMap& getCurrentParameters() {
				return this->processorParams;
			}
			
			/* HASPARAMETERS Test if the processor uses specific parameters */
			bool hasParameters(const std::string& parName) {
				return processorParams.has_key( parName );
			}
            
			/* MODIFYPARAMETER Requests a change of value of one parameter.
			 * The modification will be done if the parameter is not blacklisted and if it is a real parameter. */
			bool modifyParameter(std::string parName, std::string newValue) {
				if ( ! verifyBlacklistedParameters(parName) )
					if ( processorParams.has_key( parName ) ) {
						this->processorParams.set( parName, newValue );
						return true;
					}
				return false;	
			}

			/* Returns a const reference of the type of this processor */		
			const procType& getType () {
				return type;
			}
			
			/* Returns a const reference of the infos struct of this processor */
			const pInfoStruct& getProcessorInfo() {
				return pInfo;
			}

			/* Returns a const reference of the infos struct of the in processor of this processor */
			const pInfoStruct& getInProcessorInfo( const uint32_t inProcNumber ) {
				return this->inputProcessors.getProcessor(inProcNumber)->getProcessorInfo();
			}
			
			/* Getter methods : This funtion sends a const reference for the asked parameter's value */
			const std::string& getParameter( std::string paramArg ) {
				return this->processorParams[ paramArg ];
			}
			
			/* Compare only the information of the two processors */
			const bool compareInfos (const Processor& toCompare) {
				if ( this->pInfo.name == toCompare.pInfo.name )
					if ( this->pInfo.label == toCompare.pInfo.label )
						if ( this->pInfo.requestName == toCompare.pInfo.requestName )
							if ( this->pInfo.requestLabel == toCompare.pInfo.requestLabel )
								if ( this->pInfo.outputType == toCompare.pInfo.outputType )
									if ( this->pInfo.isBinaural == toCompare.pInfo.isBinaural )
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

			void printSignals() {
				for(outT_SignalIter it = outputSignals.begin(); it != outputSignals.end(); ++it)
					(*it)->printSignal( );
			}

			void calcLastChunk() {
				for(outT_SignalIter it = outputSignals.begin(); it != outputSignals.end(); ++it)
					(*it)->calcLastChunk( );
			}

			// FIXME : this may be not useful as it is. Think about it.
			uint32_t getFreshDataSize() {
				outT_SignalIter it = outputSignals.begin();
				return (*it)->getFreshDataSize();
			}

			const uint32_t getFsOut() {
				return this->fsOut;
			}

			const uint32_t getFsIn() {
				return this->fsIn;
			}
						
			void addInputProcessor(inProcessorSharedPtr inProcessor) {
				assert ( this->getFsIn() == inProcessor->getFsOut() );
				inputProcessors.addProcessor ( inProcessor );
			}

			void removeInputProcessor(inProcessorSharedPtr inProcessor) {
				inputProcessors.removeProcessor ( inProcessor );
			}

			const uint32_t getNumberInProcessor() {
				return this->inputProcessors.getSize ( );
			}
								
			void calcLastData( uint32_t samplesArg ) {
				for(outT_SignalIter it = outputSignals.begin(); it != outputSignals.end(); ++it)
					(*it)->calcLastData( samplesArg );
			}
			
			void calcOldData( uint32_t samplesArg = 0 ) {
				for(outT_SignalIter it = outputSignals.begin(); it != outputSignals.end(); ++it)
					(*it)->calcOldData( samplesArg );
			}

			void calcWholeBuffer() {
				for(outT_SignalIter it = outputSignals.begin(); it != outputSignals.end(); ++it)
					(*it)->calcWholeBuffer( );
			}

			outT_nTwoCTypeBlockAccessorPtrVector& getLastChunkAccesor( ) {
				return this->outT_lastChunkInfo;
			}

			outT_nTwoCTypeBlockAccessorPtrVector& getLastDataAccesor( ) {
				return this->outT_lastDataInfo;
			}
			
			outT_nTwoCTypeBlockAccessorPtrVector& getOldDataAccesor( ) {
				return this->outT_oldDataInfo;
			}
			
			outT_nTwoCTypeBlockAccessorPtrVector& getWholeBufferAccesor( ) {
				return this->outT_wholeBufferInfo;
			}
			
			/* This funcion publishes (appends) the signals to the outputs of the processor */
			void appendChunk () {
				
				assert( this->outPrivateMemoryZone.size() == this->outputSignals.size() );
				
				outT_SignalIter itOut = this->outputSignals.begin();
				for ( outT_SignalIter itPMZ = this->outPrivateMemoryZone.begin() ; itPMZ != this->outPrivateMemoryZone.end() ; ++itPMZ ) {
					// (*itPMZ)->calcOldData();
					// (*itOut)->appendChunk( (*itPMZ)->getOldDataAccesor() );
					(*itPMZ)->calcLastChunk();
					(*itOut)->appendChunk( (*itPMZ)->getLastChunkAccesor() );					
					itOut++;
				}
			}

			uint64_t getFrameIndex () {
				return this->outputSignals[0]->getFrameIndex();
			}
			
			const uint64_t getNFR () {
				return this->nfr;
			}
			
			void setNFR ( const uint64_t nfrArg ) {
				this->nfr = nfrArg;
			}			
	};

};


#endif /* PROCESSOR_HPP */
