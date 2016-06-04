#ifndef IHCPROC_HPP
#define IHCPROC_HPP
	
#include "TFSProcessor.hpp"
#include "gammatoneProc.hpp"

#include "../Filters/bwFilter.hpp"

#include "../tools/mathTools.hpp"

/* 
 * Inner Hair Cell Proc :
 * 
 * 
 * */
 
namespace openAFE {

	enum ihcMethod {
        _none,
        _halfwave,
        _fullwave,
        _square,
        _hilbert,
        _joergensen,
        _dau,
        _breebart,
        _bernstein
	};

	class IHCProc : public TFSProcessor<float > {

		private:

			typedef std::shared_ptr< bwFilter<float> > bwFilterPtr;
            typedef std::vector <bwFilterPtr > filterPtrVector;
			
			ihcMethod method;
			filterPtrVector ihcFilter_l, ihcFilter_r;			
			std::shared_ptr<GammatoneProc > upperProcPtr;

			uint32_t fb_nChannels;

			void populateFilters( filterPtrVector& filters ) {

                 switch ( this->method ) {

                     case _joergensen:
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
						 
                         break;

                     case _dau:
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     case _breebart:
                         // First order butterworth filter @ 2000Hz cascaded 5
                         // times
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     case _bernstein:
                         // Second order butterworth filter @ 425Hz
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     default:
                         this->ihcFilter_l.clear();
                         this->ihcFilter_r.clear();
                         break;

				}	

			}

			void processFilteringChannel ( bwFilterPtr filter, std::shared_ptr<twoCTypeBlock<float> > oneChannel ) {
				// 0- Initialization
				size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				float* firstValue1 = oneChannel->array1.first;
				float* firstValue2 = oneChannel->array2.first;

                 switch ( this->method ) {

                     case _joergensen:
						
						break;
					 case _dau:

					 	break;
					 	
					 case _breebart:
					 
					 	break;
					 	
					 case _bernstein:

					 	break;
					 	
					 default:
					 
						break;
				}					
			}
			
			void processChannel ( std::shared_ptr<twoCTypeBlock<float> > oneChannel ) {
				// 0- Initialization
				size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				float* firstValue1 = oneChannel->array1.first;
				float* firstValue2 = oneChannel->array2.first;

                 switch ( this->method ) {

                     case _none:
						// Nothing
						break;
					 case _halfwave:

						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = fmax( *( firstValue1 + ii ), 0 );
						if ( dim2 > 0 )	
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = fmax( *( firstValue2 + ii ), 0 );
					 	break;
					 	
					 case _fullwave:
					 
						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = fabs( *( firstValue1 + ii ) );
						if ( dim2 > 0 )	
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = fabs( *( firstValue2 + ii ) );	 
					 	break;
					 	
					 case _square:

						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = pow( fabs( *( firstValue1 + ii ) ), 2 );
						if ( dim2 > 0 )	
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = pow( fabs( *( firstValue2 + ii ) ), 2 );						 
					 	break;
					 	
					 default:
						break;
				}					
			}
			
			void processLR ( filterPtrVector& filters, std::vector<std::shared_ptr<twoCTypeBlock<float> > > PMZ ) {
				std::vector<std::thread> threads;
				  for ( size_t ii = 0 ; ii < this->fb_nChannels ; ++ii ) {
					  if ( ( ( this->method == _joergensen ) or ( this->method == _dau ) or ( this->method == _breebart ) or ( this->method == _bernstein ) ) and ( filters.size() > 0 ) )
						threads.push_back(std::thread( &IHCProc::processFilteringChannel, this, filters[ii], PMZ[ii] ));
					  else threads.push_back(std::thread( &IHCProc::processChannel, this, PMZ[ii] ));
				   }

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();
			}
														
		public:
		
			/* PreProc */
			IHCProc (const std::string nameArg, const uint32_t fsIn, const uint32_t fsOut, const uint32_t bufferSize_s, std::shared_ptr<GammatoneProc > upperProcPtr,
				ihcMethod method = _dau  ) : TFSProcessor<float > (nameArg, fsIn, fsOut, bufferSize_s, upperProcPtr->get_fb_nChannels(), "magnitude", _ihc) {
					
				this->fb_nChannels = upperProcPtr->get_fb_nChannels();
				this->upperProcPtr = upperProcPtr;
				this->method = method;
				this->prepareForProcessing();																							 
			}
			
			~IHCProc () {
				
				ihcFilter_l.clear();
				ihcFilter_r.clear();
			}
			
			void prepareForProcessing() {
				this->populateFilters( this->ihcFilter_l );
				this->populateFilters( this->ihcFilter_r );
			}
			
			void processChunk ( ) {
				this->setNFR ( this->upperProcPtr->getNFR() );

				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				std::vector<std::shared_ptr<twoCTypeBlock<float> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<float> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
				this->processLR( ihcFilter_l, l_PMZ );
				this->processLR( ihcFilter_r, r_PMZ );				
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool operator==( IHCProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( this->get_ihc_method() == toCompare.get_ihc_method() )				     		     			     
						return true;
				return false;
			}

			// getters
			const ihcMethod get_ihc_method() {return this->method;}

			// setters			
			void set_ihc_method(const ihcMethod arg) {this->method=arg; this->prepareForProcessing ();}

	}; /* class IHCProc */
}; /* namespace openAFE */

#endif /* IHCPROC_HPP */
