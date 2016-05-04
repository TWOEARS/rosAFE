#ifndef PREPROC_HPP
#define PREPROC_HPP
	
#include "../Signals/TimeDomainSignal.hpp"
#include "Processor.hpp"

// #include "preProcLib/preProcLib.hpp"

#include "../Filters/bwFilter/bwFilter.hpp" 

/* 
 * preProc :
 * 
 * 
 * */


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
			
			void setToDefaultParams () {
						
				this->processorParams.set("pp_bRemoveDC", 0);
				this->processorParams.set("pp_cutoffHzDC", 20);
				this->processorParams.set("pp_bPreEmphasis", 0);
				this->processorParams.set("pp_coefPreEmphasis", 0.97);
				this->processorParams.set("pp_bNormalizeRMS", 0);
				this->processorParams.set("pp_bBinauralRMS", 1);
				this->processorParams.set("pp_intTimeSecRMS", 500E-3);
				this->processorParams.set("pp_bLevelScaling", 0);
				this->processorParams.set("pp_refSPLdB", 100);
				this->processorParams.set("pp_bMiddleEarFiltering", 0);
				this->processorParams.set("pp_middleEarModel", "jespen");
				this->processorParams.set("pp_bUnityComp", 1);
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
				// The filter object
			
			typedef std::shared_ptr< bwFilter<T> > bwFilterPtr;
			// using bwFilterPtr = bwFilter<T>::bwFilterPtr;
				
			bwFilterPtr dcFilter_l;
			bwFilterPtr dcFilter_r;
									
		public:
		
			using typename PB::outT_SignalSharedPtr;

			/* PreProc */
			PreProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, apf::parameter_map& paramsArg) : PB (fsIn, fsOut, _preProc) {

				/* Setting the user's parameters */
				this->processorParams = paramsArg;
				
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
				
				this->prepareForProcessing ();
				
				std::cout << " End of ctor " << std::endl;
			}
				
			~PreProc () {
				std::cout << "Destructor of a pre processor" << std::endl;
			}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not published yet on the output vectors.
			 */
			void processChunk () {
				
				    const apfMap map = this->getCurrentParameters();
				    
					inputPtrIterator it = this->inputProcessors.processorVector.begin();
					outT_SignalIter itPMZ = this->outPrivateMemoryZone.begin();
						
					// Appending the chunk to process (the processing must be done on the PMZ)
					(*itPMZ)->appendChunk( ((*it)->getLastChunkAccesor())[0] );
					(*(itPMZ+1))->appendChunk( ((*it)->getLastChunkAccesor())[1] );
										
					// Getting acces to that chunk
					(*itPMZ)->calcLastChunk();
					(*(itPMZ+1))->calcLastChunk();
											
						// Actual Processing
						if ( map.get<unsigned short>("pp_bRemoveDC") ) {
							
							
							if ( (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim > 0 )
								this->dcFilter_l->filterChunk( (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue, (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue + (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim , (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue );
							if ( (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim > 0 )
								this->dcFilter_l->filterChunk( (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue, (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue + (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim , (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue );

							if ( (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim > 0 )
								this->dcFilter_r->filterChunk( (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue, (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue + (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim , (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue );
							if ( (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim > 0 )
								this->dcFilter_r->filterChunk( (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue, (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue + (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim , (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue );
							
						}
						// Processed data is on PMZ				
			}

			void prepareForProcessing () {
						
				const apfMap map = this->getCurrentParameters();
	
				if ( map.get<unsigned short>("pp_bRemoveDC") ) {
					
					// preProcLib::createBWFilter ( this->dcFilter_l, this->getFsOut(), map.get<float>("pp_cutoffHzDC") );
					// preProcLib::createBWFilter ( this->dcFilter_r, this->getFsOut(), map.get<float>("pp_cutoffHzDC") );
					
					this->dcFilter_l.reset ( new bwFilter<T> ( this->getFsOut(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					this->dcFilter_r.reset ( new bwFilter<T> ( this->getFsOut(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					
				} else {
					
					// Deleting the filter objects
					this->dcFilter_l.reset();
					this->dcFilter_r.reset();
				}
				
			}
									
			/* TODO : Resets the internat states. */		
			void reset() {
				PB::reset();
			}							

	}; /* class PreProc */
}; /* namespace openAFE */

#endif /* PREPROC_HPP */
