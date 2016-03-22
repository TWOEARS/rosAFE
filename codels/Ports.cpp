#include "Ports.hpp"


/* initPort ----------------------------------------------------------------- */

genom_event
initTDSPort(const char *name, const rosAFE_TDSPorts *TDSPorts, uint32_t sampleRate,
                  uint32_t bufferSize_s, uint32_t bytesPerFrame, genom_context self)
{
  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
  
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

/* publishPort ------------------------------------------------------------- */

genom_event
publishTDSPort(const char *name, const rosAFE_TDSPorts *TDSPorts, inputProcAccessorVector inChunk, genom_context self)
{
	assert ( inChunk.size() == 2 );
		
    rosAFE_TimeDomainSignalPortStruct *data;

    data = TDSPorts->data( name, self );
    
    uint32_t dim1 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->dim;
    uint32_t dim2 = (inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->dim;
        
 	uint32_t fpc = dim1 + dim2; /* amount of Frames On this Chunk */
	uint32_t fop = data->framesOnPort; /* total amount of Frames On the Port */
	uint32_t bpf = data->bytesPerFrame;
	
    memmove(data->left._buffer, data->left._buffer + fpc, (fop - fpc)*bpf);
    memmove(data->right._buffer, data->right._buffer + fpc, (fop - fpc)*bpf);

	uint32_t pos, ii;
	if (dim2 == 0) {	
		for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
			data->left._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
			data->right._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
		}
	} else if (dim1 == 0) {
			for (ii = 0, pos = fop - fpc; pos < fop ; ii++, pos++) {
				data->left._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				data->right._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
			}
	} else {
			for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ii++, pos++) {
				data->left._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
				data->right._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->first->firstValue + ii);
			}
			
			for (ii = 0, pos = fop - dim2; pos < fop ; ii++, pos++) {
				data->left._buffer[pos] = *((inChunk[0])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
				data->right._buffer[pos] = *((inChunk[1])->getTwoCTypeBlockAccessor( 0 )->second->firstValue + ii);
			}			
		}
 	 	
    data->lastFrameIndex += fpc;
    TDSPorts->write( name, self );
	
    return genom_ok;
}

genom_event
deleteTDSPort   (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self) {
	
  TDSPorts->close( name, self );
  return genom_ok;
}

