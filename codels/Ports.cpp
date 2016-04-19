#include "Ports.hpp"


/* initPort ----------------------------------------------------------------- */
/*
genom_event
initTDSPort(const char *name, const rosAFE_TDSPorts *TDSPorts, uint32_t sampleRate,
                  uint32_t bufferSize_s, uint32_t bytesPerFrame, genom_context self)
{
  uint32_t fop =  sampleRate * bufferSize_s; // total amount of Frames On the Port 
  
  TDSPorts->open( name, self );
    
  if (genom_sequence_reserve(&(TDSPorts->data( name, self )->left), fop) ||
      genom_sequence_reserve(&(TDSPorts->data( name, self )->right), fop))
  return rosAFE_e_noMemory( self );

  TDSPorts->data( name, self )->left._length = fop;
  TDSPorts->data( name, self )->right._length = fop;

  for (uint32_t ii = 0; ii < fop; ii++) {
    TDSPorts->data( name, self )->left._buffer[ii] = 0;
    TDSPorts->data( name, self )->right._buffer[ii] = 0;
  }

  TDSPorts->data( name, self )->sampleRate = sampleRate;
  TDSPorts->data( name, self )->framesOnPort = fop;
  TDSPorts->data( name, self )->bytesPerFrame = bytesPerFrame;
  TDSPorts->data( name, self )->lastFrameIndex = 0;
  
  TDSPorts->write( name, self );
  
  return genom_ok;
}
*/
/* publishPort ------------------------------------------------------------- */

genom_event
PORT::publishTDSPort(const char *name, const rosAFE_dataObj *dataObj, inputProcAccessorVector inChunk, uint32_t iii, genom_context self)
{
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
			
    dataObj->data( self )->allTDSPorts._buffer[iii].lastFrameIndex += fpc;
    dataObj->data( self )->allTDSPorts._buffer[iii].framesOnPort = fpc;    

    strcpy(dataObj->data( self )->allTDSPorts._buffer[iii].name, name);
    dataObj->write( self );
	
    return genom_ok;
}


/*
genom_event
deleteTDSPort   (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self) {
	
  TDSPorts->close( name, self );
  return genom_ok;
}
*/
