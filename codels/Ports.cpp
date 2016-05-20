#include "Ports.hpp"


/* initPort ----------------------------------------------------------------- */

genom_event
PORT::initInputPort ( const rosAFE_inputProcPort *inputProcPort, uint32_t sampleRate,
                uint32_t bufferSize_s, genom_context self ) {
					
	  uint32_t fop =  sampleRate * bufferSize_s; // total amount of Frames On the Port 
		
	  if (genom_sequence_reserve(&(inputProcPort->data( self )->left.data), fop) ||
		  genom_sequence_reserve(&(inputProcPort->data( self )->right.data), fop))
	  return rosAFE_e_noMemory( self );

	  inputProcPort->data( self )->left.data._length = fop;
	  inputProcPort->data( self )->right.data._length = fop;

	  for (uint32_t ii = 0; ii < fop; ii++) {
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
PORT::publishInputPort ( const rosAFE_inputProcPort *inputProcPort, inputProcAccessorVector inChunk, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {
	
		assert ( inChunk.size() == 2 );

		rosAFE_TimeDomainSignalPortStruct *data;

		data = inputProcPort->data( self );
    			
		uint32_t dim1 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->dim;
		uint32_t dim2 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->dim;
			
		uint32_t fpc = dim1 + dim2; 		// amount of Frames On this Chunk
		uint32_t fop = data->framesOnPort; 	// total amount of Frames On the Port

		memmove(data->left.data._buffer, data->left.data._buffer + fpc, (fop - fpc)*bytesPerFrame);
		memmove(data->right.data._buffer, data->right.data._buffer + fpc, (fop - fpc)*bytesPerFrame);

		uint32_t pos, ii;
		if (dim2 == 0) {	
			for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
				data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
				data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
			}
		} else if (dim1 == 0) {
				for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
					data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
					data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				}
		} else {
				for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ii++, pos++) {
					data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
					data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
				}
				
				for (ii = 0, pos = fop - dim2; pos < fop ; ii++, pos++) {
					data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
					data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				}			
			}
			
		data->lastFrameIndex = nfr;
		inputProcPort->write( self );
	
}


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
PORT::publishPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, inputProcAccessorVector inChunk, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {
	
	assert ( inChunk.size() == 2 );
		
    rosAFE_TimeDomainSignalPortStruct *data;

    data = preProcPort->data( name, self );
    
    uint32_t dim1 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->dim;
    uint32_t dim2 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->dim;
        
 	uint32_t fpc = dim1 + dim2; /* amount of Frames On this Chunk */
	uint32_t fop = data->framesOnPort; /* total amount of Frames On the Port */
	
    memmove(data->left.data._buffer, data->left.data._buffer + fpc, (fop - fpc)*bytesPerFrame);
    memmove(data->right.data._buffer, data->right.data._buffer + fpc, (fop - fpc)*bytesPerFrame);

	uint32_t pos, ii;
	if (dim2 == 0) {	
		for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
			data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
			data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
		}
	} else if (dim1 == 0) {
			for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
				data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
			}
	} else {
			for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ii++, pos++) {
				data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
				data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
			}
			
			for (ii = 0, pos = fop - dim2; pos < fop ; ii++, pos++) {
				data->left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				data->right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
			}			
		}
 	 	
    data->lastFrameIndex = nfr;
    preProcPort->write( name, self );	

    return genom_ok;
}














	
/* publishPort ------------------------------------------------------------- */

genom_event
PORT::publishTDSPort(const char *name, const rosAFE_dataObj *dataObj, inputProcAccessorVector inChunk, uint64_t frameIndex, uint32_t iii, genom_context self) {
/*	
		assert ( inChunk.size() == 2 );
			
		uint32_t dim1 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->dim;
		uint32_t dim2 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->dim;
			
		uint32_t fpc = dim1 + dim2; // amount of Frames On this Chunk
		
		dataObj->data( self )->allTDSPorts._buffer[iii].left.data._length =  fpc;
		dataObj->data( self )->allTDSPorts._buffer[iii].right.data._length =  fpc; 		
		if ( (genom_sequence_reserve(&(dataObj->data( self )->allTDSPorts._buffer[iii].left.data), fpc)) || (genom_sequence_reserve(&(dataObj->data( self )->allTDSPorts._buffer[iii].right.data), fpc)) )
			return rosAFE_e_noMemory( self );

		uint32_t pos;
		for (pos = 0 ; pos < dim1 ; pos++) {
			dataObj->data( self )->allTDSPorts._buffer[iii].left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + pos);
			dataObj->data( self )->allTDSPorts._buffer[iii].right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + pos);
		}

		for (pos = dim1 ; pos < fpc ; pos++) {
			dataObj->data( self )->allTDSPorts._buffer[iii].left.data._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + pos);
			dataObj->data( self )->allTDSPorts._buffer[iii].right.data._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + pos);
		}
		
		dataObj->data( self )->allTDSPorts._buffer[iii].lostData = frameIndex - ( dataObj->data( self )->allTDSPorts._buffer[iii].lastFrameIndex + fpc );	
		dataObj->data( self )->allTDSPorts._buffer[iii].lastFrameIndex = frameIndex;
		dataObj->data( self )->allTDSPorts._buffer[iii].framesOnPort = fpc;

		strcpy(dataObj->data( self )->allTDSPorts._buffer[iii].name, name);
		dataObj->write( self );
*/		
		return genom_ok;
	}


/*
genom_event
deleteTDSPort   (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self) {
	
  TDSPorts->close( name, self );
  return genom_ok;
}
*/
