#ifndef PREPROC_HPP
#define PREPROC_HPP

#include <stdlib.h>		/* abs */
#include <math.h>       /* exp, pow*/
	
#include "TDSProcessor.hpp"
#include "inputProc.hpp"	

#include "../Filters/bwFilter/bwFilter.hpp"
#include "../Filters/GenericFilter.hpp"

#include "../tools/mathTools.hpp"

#define EPSILON 0.00000001

/* 
 * preProc :
 * 
 * 
 * */


namespace openAFE {

	class PreProc : public TDSProcessor<float> {

		private:
			
			void setToDefaultParams () {
				this->processorParams.set("pp_bRemoveDC", 0);
				this->processorParams.set("pp_cutoffHzDC", 20);
				this->processorParams.set("pp_bPreEmphasis", 0);
				this->processorParams.set("pp_coefPreEmphasis", 0.97);
				this->processorParams.set("pp_bNormalizeRMS", 0);
				this->processorParams.set("pp_intTimeSecRMS", 0.5);
				this->processorParams.set("pp_bLevelScaling", 0);
				this->processorParams.set("pp_refSPLdB", 100);
				this->processorParams.set("pp_bMiddleEarFiltering", 0);
				this->processorParams.set("pp_middleEarModel", "jespen");
				this->processorParams.set("pp_bUnityComp", 1);
			}
		
            void verifyParameters() {
				
				/* TODO : Follow Matlab AFE to update this function
				 * Nothing at : 17.02.2016
				 */
				
			}
			
			typedef std::shared_ptr< bwFilter<float> > bwFilterPtr;
			typedef std::shared_ptr< GenericFilter<float, float, float, float> > genericFilterPtr;
			
			double meFilterPeakdB;

			/* Pointers to Filter Objects */				
			bwFilterPtr dcFilter_l;
			bwFilterPtr dcFilter_r;
			
			genericFilterPtr preEmphFilter_l;
			genericFilterPtr preEmphFilter_r;
			
			genericFilterPtr agcFilter_l;
			genericFilterPtr agcFilter_r;

			genericFilterPtr midEarFilter_l;
			genericFilterPtr midEarFilter_r;
			
			std::shared_ptr<InputProc > upperProcPtr;
			
			// Actual Processing
			void process ( float* firstValue_l, size_t dim_l, float* firstValue_r, size_t dim_r ) {
			
					const apfMap map = this->getCurrentParameters();
			    
					// 1- DC-removal filter					
					if ( map.get<unsigned short>("pp_bRemoveDC") ) {
						std::thread leftThread1( &bwFilter<float>::filterChunk, this->dcFilter_l, firstValue_l, firstValue_l + dim_l , firstValue_l );
						std::thread rightThread1( &bwFilter<float>::filterChunk, this->dcFilter_r, firstValue_r, firstValue_r + dim_r , firstValue_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
					}

					// 2- Pre-whitening
					if ( map.get<unsigned short>("pp_bPreEmphasis") ) {
						std::thread leftThread1( &GenericFilter<float,float, float, float>::exec, this->preEmphFilter_l, firstValue_l, dim_l , firstValue_l );
						std::thread rightThread1( &GenericFilter<float,float, float, float>::exec, this->preEmphFilter_r, firstValue_r, dim_r , firstValue_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes		
					}

					// 3- Automatic gain control	
					if ( map.get<unsigned short>("pp_bNormalizeRMS") ) {

						// Initialize the filter states if empty
						if ( !( agcFilter_l->isInitialized() ) ) {

							float intArg = map.get<float>("pp_intTimeSecRMS") * this->getFsIn();
							float sum_l = 0, sum_r = 0, s0_l, s0_r;
							uint32_t minVal_l, minVal_r;
							
							minVal_l = fmin ( dim_l, round( intArg ) );
							minVal_r = fmin ( dim_r, round( intArg ) );

							// Mean square of input over the time constant
							for ( unsigned int i = 0 ; i < minVal_l ; ++i ) {
								sum_l =+ pow( *(firstValue_l + i ) , 2);
								sum_r =+ pow( *(firstValue_r + i ) , 2);
							}
								
							// Initial filter states
							s0_l = exp ( -1 / intArg ) * ( sum_l / minVal_l );							
							s0_r = exp ( -1 / intArg ) * ( sum_r / minVal_r );
								
							this->agcFilter_l->reset( &s0_l, 1 );
							this->agcFilter_r->reset( &s0_r, 1 );
						}
						
						// Estimate normalization constants
						std::vector<float> tmp_l ( dim_l ), tmp_r ( dim_r );
						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							tmp_l[ i ] = pow(*( firstValue_l + i ), 2);
							tmp_r[ i ] = pow(*( firstValue_r + i ), 2);
						}
						
						float normFactor;
						
						std::thread leftThread1( &GenericFilter<float,float, float, float>::exec, this->agcFilter_l, tmp_l.data(), dim_l , tmp_l.data() );
						std::thread rightThread1( &GenericFilter<float,float, float, float>::exec, this->agcFilter_r, tmp_r.data(), dim_r , tmp_r.data() );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
						
						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							tmp_l[ i ] = sqrt( tmp_l[ i ] ) + EPSILON;
							tmp_r[ i ] = sqrt( tmp_r[ i ] ) + EPSILON;
							normFactor = fmax ( tmp_l[ i ], tmp_r[ i ] );
							
							*(firstValue_l + i ) /= normFactor;
							*(firstValue_r + i ) /= normFactor;
						}
					}
					
					// 4- Level Scaling
					if ( map.get<unsigned short>("pp_bUnityComp") ) {
						
						double current_dboffset = 100; //dbspl(1);
						double dbVar = pow( 10 , ( current_dboffset - map.get<float>("pp_refSPLdB") ) / 20 );

						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							*(firstValue_l + i ) *= dbVar;
							*(firstValue_r + i ) *= dbVar;
						}
					}
					
					// 5- Middle Ear Filtering
					if ( map.get<unsigned short>("pp_bUnityComp") ) {
						
						if ( map["pp_bPreEmphasis"] == "jespen" )
							this->meFilterPeakdB = 55.9986;
						else if ( map["pp_bPreEmphasis"] == "lopezpoveda" )
							this->meFilterPeakdB = 66.2888;
						
					} else this->meFilterPeakdB = 0;
					
					if ( map.get<unsigned short>("pp_bMiddleEarFiltering") == 1 ) {
						//	TODO 
					}
													
					// Processed data is on PMZ					
			}
													
		public:
		
			/* PreProc */
			PreProc (const std::string nameArg, const uint32_t fs, const uint32_t bufferSize_s, std::shared_ptr<InputProc > upperProcPtr, apf::parameter_map& paramsArg) : TDSProcessor<float> (nameArg, fs, fs, bufferSize_s, _inputProc) {
				
				/* Setting the user's parameters */
				this->givenParameters ( paramsArg );

				this->upperProcPtr = upperProcPtr;
				
				this->verifyParameters();
				this->prepareForProcessing ();
			}
				
			~PreProc () {
				dcFilter_l.reset();
				dcFilter_r.reset();				
				
				preEmphFilter_l.reset();
				preEmphFilter_r.reset();
			
				agcFilter_l.reset();
				agcFilter_r.reset();

				midEarFilter_l.reset();
				midEarFilter_r.reset();
			}
			
			void processChunk () {
    					
					this->setNFR ( upperProcPtr->getNFR() ); /* for rosAFE */
											
					// Appending the chunk to process (the processing must be done on the PMZ)
					leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
					rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
	
					std::shared_ptr<twoCTypeBlock<float> > l_PMZ = leftPMZ->getLastChunkAccesor();
					std::shared_ptr<twoCTypeBlock<float> > r_PMZ = rightPMZ->getLastChunkAccesor();
					
					// 0- Initialization
					size_t dim1_l = l_PMZ->array1.second;
					size_t dim2_l = l_PMZ->array2.second;
					size_t dim1_r = r_PMZ->array1.second;
					size_t dim2_r = r_PMZ->array2.second;
							
					float* firstValue1_l = l_PMZ->array1.first;
					float* firstValue2_l = l_PMZ->array2.first;
					float* firstValue1_r = r_PMZ->array1.first;
					float* firstValue2_r = r_PMZ->array2.first;				
					
					if ( ( dim1_l > 0 ) && ( dim1_r > 0 ) )
						process ( firstValue1_l, dim1_l, firstValue1_r, dim1_r );
					if ( ( dim2_l > 0 ) && ( dim2_r > 0 ) )	
						process ( firstValue2_l, dim2_l, firstValue2_r, dim2_r );
			}
			
			void prepareForProcessing () {
						
				const apfMap map = this->getCurrentParameters();

				// Filter instantiation (if needed)	
				if ( map.get<unsigned short>("pp_bRemoveDC") ) {
					
					this->dcFilter_l.reset ( new bwFilter<float> ( this->getFsIn(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					this->dcFilter_r.reset ( new bwFilter<float> ( this->getFsIn(), 4 /* order */, map.get<float>("pp_cutoffHzDC"), (bwType)1 /* High */ ) );
					
				} else {
					// Deleting the filter objects
					this->dcFilter_l.reset();
					this->dcFilter_r.reset();
				}

				if ( map.get<unsigned short>("pp_bPreEmphasis") ) {
					
					std::vector<float> vectB (2,1);
					vectB[1] = -1 * abs( map.get<float>("pp_coefPreEmphasis") );
					std::vector<float> vectA (1,1);
					
					this->preEmphFilter_l.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					this->preEmphFilter_r.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->preEmphFilter_l.reset();
					this->preEmphFilter_r.reset();
				}
				
				if ( map.get<unsigned short>("pp_bNormalizeRMS") ) {

					std::vector<float> vectB (1,1);
					std::vector<float> vectA (2,1);
					
					vectA[1] = -1 * exp( -1 / ( map.get<float>("pp_intTimeSecRMS") * this->getFsIn() ) );
					vectB[0] = vectA[0] + vectA[1];

					this->agcFilter_l.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					this->agcFilter_r.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->agcFilter_l.reset();
					this->agcFilter_r.reset();
				}

				if ( map.get<unsigned short>("pp_bMiddleEarFiltering") ) {

					 //this->midEarFilter_l.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					 //this->midEarFilter_r.reset ( new GenericFilter<float,float, float, float> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->midEarFilter_l.reset();
					this->midEarFilter_r.reset();
				}			
			}
	
	}; /* class PreProc */
}; /* namespace openAFE */

#endif /* PREPROC_HPP */
