#ifndef PREPROC_HPP
#define PREPROC_HPP
	
#include "../Signals/TimeDomainSignal.hpp"
#include "Processor.hpp"

// #include "preProcLib/preProcLib.hpp"

#include "../Filters/bwFilter/bwFilter.hpp"
#include "../Filters/preEmphasis.hpp"
#include "../Filters/agcFilter.hpp"


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
			// midEarFilter_l | midEarFilter_r | 
			
			typedef std::shared_ptr< bwFilter<T> > bwFilterPtr;
			typedef std::shared_ptr< PreEmphasis<T> > preEmphasisPtr;
			typedef std::shared_ptr< AgcFilter<T> > agcFilterPtr;
			
			double meFilterPeakdB;
				
			bwFilterPtr dcFilter_l;
			bwFilterPtr dcFilter_r;
			
			preEmphasisPtr preEmphFilter_l;
			preEmphasisPtr preEmphFilter_r;
			
			agcFilterPtr agcFilter_l;
			agcFilterPtr agcFilter_r;
												
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
				this->outputSignals.push_back( leftOutput );
				this->outputSignals.push_back( rightOutput );
								
				/* Linking the output accesors of each signal */
				this->linkAccesors ();
				
				/* This processor can take two inputs and two outputs */
				// Processor::isBinaural = true;
				this->hasTwoOutputs = true;
	
				/* Creating the PMZ signals */
				outT_SignalSharedPtr outLeftPMZ( new TimeDomainSignal<T>(fsOut, bufferSize_s, "Left TDS PMZ", "Left TDS PMZ", _left) );
				outT_SignalSharedPtr outRightPMZ( new TimeDomainSignal<T>(fsOut, bufferSize_s, "Right TDS PMZ", "Right TDS PMZ", _right) );
				
				/* Setting those signals as the PMZ signals of this processor */
				this->outPrivateMemoryZone.push_back( outLeftPMZ );
				this->outPrivateMemoryZone.push_back( outRightPMZ );
				
				this->prepareForProcessing ();	
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
					
					this->setNFR ( (*it)->getNFR() ); /* for rosAFE */
					
					outT_SignalIter itPMZ = this->outPrivateMemoryZone.begin();
						
					// Appending the chunk to process (the processing must be done on the PMZ)
					(*itPMZ)->appendChunk( ((*it)->getLastChunkAccesor())[0] );
					(*(itPMZ+1))->appendChunk( ((*it)->getLastChunkAccesor())[1] );
										
					// Getting acces to that chunk
					(*itPMZ)->calcLastChunk();
					(*(itPMZ+1))->calcLastChunk();
					
					unsigned long dim1_l = (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim;
					unsigned long dim2_l = (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim;
					unsigned long dim1_r = (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->dim;
					unsigned long dim2_r = (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->dim;
							
					T* firstValue1_l = (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue;
					T* firstValue2_l = (*itPMZ)->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue;
					T* firstValue1_r = (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->first->firstValue;
					T* firstValue2_r = (*(itPMZ+1))->getLastChunkAccesor()->getTwoCTypeBlockAccessor(0)->second->firstValue;
													
					// Actual Processing
           
					// 1- DC-removal filter					
					if ( map.get<unsigned short>("pp_bRemoveDC") ) {
							
						std::thread leftThread1( &bwFilter<T>::filterChunk, this->dcFilter_l, firstValue1_l, firstValue1_l + dim1_l , firstValue1_l );
						std::thread rightThread1( &bwFilter<T>::filterChunk, this->dcFilter_r, firstValue1_r, firstValue1_r + dim1_r , firstValue1_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
													
						std::thread leftThread2( &bwFilter<T>::filterChunk, this->dcFilter_l, firstValue2_l, firstValue2_l + dim2_l , firstValue2_l );
						std::thread rightThread2( &bwFilter<T>::filterChunk, this->dcFilter_r, firstValue2_r, firstValue2_r + dim2_r , firstValue2_r );

						leftThread2.join();                // pauses until left finishes
						rightThread2.join();               // pauses until right finishes				
					}

					// 2- Pre-whitening
					if ( map.get<unsigned short>("pp_bPreEmphasis") ) {
							
						std::thread leftThread1( &PreEmphasis<T>::filterChunk, this->preEmphFilter_l, firstValue1_l, firstValue1_l + dim1_l , firstValue1_l );
						std::thread rightThread1( &PreEmphasis<T>::filterChunk, this->preEmphFilter_r, firstValue1_r, firstValue1_r + dim1_r , firstValue1_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
													
						std::thread leftThread2( &PreEmphasis<T>::filterChunk, this->preEmphFilter_l, firstValue2_l, firstValue2_l + dim2_l , firstValue2_l );
						std::thread rightThread2( &PreEmphasis<T>::filterChunk, this->preEmphFilter_r, firstValue2_r, firstValue2_r + dim2_r , firstValue2_r );

						leftThread2.join();                // pauses until left finishes
						rightThread2.join();               // pauses until right finishes				
					}					

					// 3- Automatic gain control	
					
					// 4- Level Scaling

					if ( map.get<unsigned short>("pp_bPreEmphasis") ) {
						
						if ( map["pp_bPreEmphasis"] == "jespen" )
							this->meFilterPeakdB = 55.9986;
						else if ( map["pp_bPreEmphasis"] == "lopezpoveda" )
							this->meFilterPeakdB = 66.2888;
						
					} else this->meFilterPeakdB = 0;
														
					// Processed data is on PMZ				
			}

			void prepareForProcessing () {
						
				const apfMap map = this->getCurrentParameters();

				// Filter instantiation (if needed)	
				if ( map.get<unsigned short>("pp_bRemoveDC") ) {
					
					this->dcFilter_l.reset ( new bwFilter<T> ( this->getFsOut(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					this->dcFilter_r.reset ( new bwFilter<T> ( this->getFsOut(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					
				} else {
					
					// Deleting the filter objects
					this->dcFilter_l.reset();
					this->dcFilter_r.reset();
				}

				if ( map.get<unsigned short>("pp_bPreEmphasis") ) {
					
					this->preEmphFilter_l.reset ( new PreEmphasis<T> ( this->getFsOut(), map.get<float>("pp_coefPreEmphasis") ) );
					this->preEmphFilter_r.reset ( new PreEmphasis<T> ( this->getFsOut(), map.get<float>("pp_coefPreEmphasis") ) );
					
				} else {
					
					// Deleting the filter objects
					this->preEmphFilter_l.reset();
					this->preEmphFilter_r.reset();
				}
				
				if ( map.get<unsigned short>("pp_bNormalizeRMS") ) {
					
					this->agcFilter_l.reset ( new AgcFilter<T> ( this->getFsOut(), map.get<float>("pp_intTimeSecRMS") ) );
					this->agcFilter_r.reset ( new AgcFilter<T> ( this->getFsOut(), map.get<float>("pp_intTimeSecRMS") ) );
					
				} else {
					
					// Deleting the filter objects
					this->agcFilter_l.reset();
					this->agcFilter_r.reset();
				}
					
								
			}
									
			/* TODO : Resets the internat states. */		
			void reset() {
				PB::reset();
			}							

	}; /* class PreProc */
}; /* namespace openAFE */

#endif /* PREPROC_HPP */
