#include "Ports.hpp"


/* Input Port ----------------------------------------------------------------- */

	genom_event
	PORT::initInputPort ( const rosAFE_inputProcPort *inputProcPort, uint32_t sampleRate,
					uint32_t bufferSize_s, genom_context self ) {
						
		  uint32_t fop =  sampleRate * bufferSize_s; // total amount of Frames On the Port 

		  inputProcPort->data( self )->left.data._length = fop;
		  inputProcPort->data( self )->right.data._length = fop;
				
		  if (genom_sequence_reserve(&(inputProcPort->data( self )->left.data), fop) ||
			  genom_sequence_reserve(&(inputProcPort->data( self )->right.data), fop))
		  return rosAFE_e_noMemory( self );

		  for ( uint32_t ii = 0; ii < fop; ++ii ) {
			inputProcPort->data( self )->left.data._buffer[ii] = 0;
			inputProcPort->data( self )->right.data._buffer[ii] = 0;
		  }

		  inputProcPort->data( self )->sampleRate = sampleRate;
		  inputProcPort->data( self )->framesOnPort = fop;
		  inputProcPort->data( self )->lastFrameIndex = 0;
		  
		  inputProcPort->write( self );
		  
		  return genom_ok;
	}

	genom_event
	PORT::publishInputPort ( const rosAFE_inputProcPort *inputProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {	

			rosAFE_TimeDomainSignalPortStruct *data;

			data = inputProcPort->data( self );
					
			uint32_t dim1 = left->array1.second;
			uint32_t dim2 = left->array2.second;

			uint32_t fpc = dim1 + dim2; 		// amount of Frames On this Chunk
			uint32_t fop = data->framesOnPort; 	// total amount of Frames On the Port

			memmove(data->left.data._buffer, data->left.data._buffer + fpc, (fop - fpc)*bytesPerFrame);
			memmove(data->right.data._buffer, data->right.data._buffer + fpc, (fop - fpc)*bytesPerFrame);

			uint32_t pos, ii;
			if (dim2 == 0) {	
				for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
					data->left.data._buffer[pos] = *(left->array1.first + ii);
					data->right.data._buffer[pos] = *(right->array1.first + ii);
				}
			} else if (dim1 == 0) {
					for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
						data->left.data._buffer[pos] = *(left->array2.first + ii);
						data->right.data._buffer[pos] = *(right->array2.first + ii);
					}
			} else {
					for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ii++, pos++) {
						data->left.data._buffer[pos] = *(left->array1.first + ii);
						data->right.data._buffer[pos] = *(right->array1.first + ii);
					}
					
					for (ii = 0, pos = fop - dim2; pos < fop ; ii++, pos++) {
						data->left.data._buffer[pos] = *(left->array2.first + ii);
						data->right.data._buffer[pos] = *(right->array2.first + ii);
					}			
				}
				
			data->lastFrameIndex = nfr;
			inputProcPort->write( self );
			
			return genom_ok;
	}

/* Pre Proc Port ----------------------------------------------------------------- */

	genom_event
	PORT::initPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, uint32_t sampleRate,
							uint32_t bufferSize_s, genom_context self ) {
								
	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  preProcPort->open( name, self );
		
	  if (genom_sequence_reserve(&(preProcPort->data( name, self )->left.data), fop) ||
		  genom_sequence_reserve(&(preProcPort->data( name, self )->right.data), fop))
	  return rosAFE_e_noMemory( self );

	  preProcPort->data( name, self )->left.data._length = fop;
	  preProcPort->data( name, self )->right.data._length = fop;

	  for (uint32_t ii = 0; ii < fop; ii++) {
		preProcPort->data( name, self )->left.data._buffer[ii] = 0;
		preProcPort->data( name, self )->right.data._buffer[ii] = 0;
	  }

	  preProcPort->data( name, self )->sampleRate = sampleRate;
	  preProcPort->data( name, self )->framesOnPort = fop;
	  preProcPort->data( name, self )->lastFrameIndex = 0;
	  
	  preProcPort->write( name, self );
	  
	  return genom_ok;
	}

	genom_event
	PORT::publishPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {		
	   
		rosAFE_TimeDomainSignalPortStruct *data;

		data = preProcPort->data( name, self );
		
		uint32_t dim1 = left->array1.second;
		uint32_t dim2 = left->array2.second;
			
		uint32_t fpc = dim1 + dim2; // amount of Frames On this Chunk
		uint32_t fop = data->framesOnPort; // total amount of Frames On the Port
		
		memmove(data->left.data._buffer, data->left.data._buffer + fpc, (fop - fpc)*bytesPerFrame);
		memmove(data->right.data._buffer, data->right.data._buffer + fpc, (fop - fpc)*bytesPerFrame);

		uint32_t pos, ii;
		if (dim2 == 0) {	
			for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
				data->left.data._buffer[pos] = *(left->array1.first + ii);
				data->right.data._buffer[pos] = *(right->array1.first + ii);
			}
		} else if (dim1 == 0) {
				for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
					data->left.data._buffer[pos] = *(left->array2.first + ii);
					data->right.data._buffer[pos] = *(right->array2.first + ii);
				}
		} else {
				for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ii++, pos++) {
					data->left.data._buffer[pos] = *(left->array1.first + ii);
					data->right.data._buffer[pos] = *(right->array1.first + ii);
				}
				
				for (ii = 0, pos = fop - dim2; pos < fop ; ii++, pos++) {
					data->left.data._buffer[pos] = *(left->array2.first + ii);
					data->right.data._buffer[pos] = *(right->array2.first + ii);
				}			
			}
			
		data->lastFrameIndex = nfr;
		preProcPort->write( name, self );	

		return genom_ok;
	}

/* Gammatone Port ----------------------------------------------------------------- */

	genom_event
	PORT::initGammatonePort ( const char *name, const rosAFE_gammatonePort *gammatonePort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self ) {
							
	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  gammatonePort->open( name, self );
		
	  if (genom_sequence_reserve(&(gammatonePort->data( name, self )->left.dataN), nChannels) ||
		  genom_sequence_reserve(&(gammatonePort->data( name, self )->right.dataN), nChannels))
	  return rosAFE_e_noMemory( self );

	  gammatonePort->data( name, self )->left.dataN._length = nChannels;
	  gammatonePort->data( name, self )->right.dataN._length = nChannels;
	  
	  for ( size_t ii = 0 ; ii < nChannels ; ++ii ) {
		if (genom_sequence_reserve(&(gammatonePort->data( name, self )->left.dataN._buffer[ii].data), fop) ||
			genom_sequence_reserve(&(gammatonePort->data( name, self )->right.dataN._buffer[ii].data), fop))
		return rosAFE_e_noMemory( self );
	  
	  	gammatonePort->data( name, self )->left.dataN._buffer[ii].data._length = fop;
		gammatonePort->data( name, self )->right.dataN._buffer[ii].data._length = fop;
	  
		for (uint32_t iii = 0; iii < fop; iii++) {
			gammatonePort->data( name, self )->left.dataN._buffer[ii].data._buffer[iii] = 0;
			gammatonePort->data( name, self )->right.dataN._buffer[ii].data._buffer[iii] = 0;
		}	  
	  
	  }

	  gammatonePort->data( name, self )->sampleRate = sampleRate;
	  gammatonePort->data( name, self )->framesOnPort = fop;	  
	  gammatonePort->data( name, self )->numberOfChannels = nChannels;
	  gammatonePort->data( name, self )->lastFrameIndex = 0;
	  
	  gammatonePort->write( name, self );
	  
	  return genom_ok;
	}							
				

	genom_event
	PORT::publishGammatonePort ( const char *name, const rosAFE_gammatonePort *gammatonePort, std::vector<twoCTypeBlockPtr > left,
						std::vector<twoCTypeBlockPtr > right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {
											
		rosAFE_TimeFrequencySignalPortStruct *thisPort;
					
		thisPort = gammatonePort->data( name, self );
		
		for ( size_t ii = 0 ; ii < thisPort->numberOfChannels ; ++ii ) {
			
			uint32_t dim1 = left[ii]->array1.second;
			uint32_t dim2 = left[ii]->array2.second;
			
			uint32_t fpc = dim1 + dim2; 			// amount of Frames On this Chunk
			uint32_t fop = thisPort->framesOnPort; 	// total amount of Frames On the Port
		
			memmove(thisPort->left.dataN._buffer[ii].data._buffer, thisPort->left.dataN._buffer[ii].data._buffer + fpc, (fop - fpc) * bytesPerFrame);
			memmove(thisPort->right.dataN._buffer[ii].data._buffer, thisPort->right.dataN._buffer[ii].data._buffer + fpc, (fop - fpc) * bytesPerFrame);

			uint32_t pos, iii;
			if (dim2 == 0) {	
				for (iii = 0, pos = fop - fpc; pos < fop ; iii++, pos++) {
					thisPort->left.dataN._buffer[ii].data._buffer[pos] = *(left[ii]->array1.first + iii);
					thisPort->right.dataN._buffer[ii].data._buffer[pos] = *(right[ii]->array1.first + iii);
				}
			} else if (dim1 == 0) {
					for (iii = 0, pos = fop - fpc; pos < fop ; iii++, pos++) {
						thisPort->left.dataN._buffer[ii].data._buffer[pos] = *(left[ii]->array2.first + iii);
						thisPort->right.dataN._buffer[ii].data._buffer[pos] = *(right[ii]->array2.first + iii);
					}
			} else {
					for (iii = 0, pos = fop - fpc; pos < fop - dim2 ; iii++, pos++) {
						thisPort->left.dataN._buffer[ii].data._buffer[pos] = *(left[ii]->array1.first + iii);
						thisPort->right.dataN._buffer[ii].data._buffer[pos] = *(right[ii]->array1.first + iii);
					}
					
					for (iii = 0, pos = fop - dim2; pos < fop ; iii++, pos++) {
						thisPort->left.dataN._buffer[ii].data._buffer[pos] = *(left[ii]->array2.first + iii);
						thisPort->right.dataN._buffer[ii].data._buffer[pos] = *(right[ii]->array2.first + iii);
					}			
				}
		}
			
		thisPort->lastFrameIndex = nfr;
		gammatonePort->write( name, self );			
							
		return genom_ok;							
	}
							
/*
genom_event
deleteTDSPort   (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self) {
	
  TDSPorts->close( name, self );
  return genom_ok;
}
*/
