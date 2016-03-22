#include "Ports.hpp"


/* initPort ----------------------------------------------------------------- */

genom_event
initInputPort(const char *name, const rosAFE_inputProcessorOutput *inputProcessorOutput, uint32_t sampleRate,
                  uint32_t bufferSize_s, uint32_t bytesPerFrame, genom_context self)
{
  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
  
  inputProcessorOutput->open( name, self );
    
  if (genom_sequence_reserve(&(inputProcessorOutput->data( name, self )->left), fop) ||
      genom_sequence_reserve(&(inputProcessorOutput->data( name, self )->right), fop))
  return rosAFE_e_noMemory( self );

  inputProcessorOutput->data( name, self )->left._length = fop;
  inputProcessorOutput->data( name, self )->right._length = fop;

  for (uint32_t ii = 0; ii < fop; ii++) {
    inputProcessorOutput->data( name, self )->left._buffer[ii] = 0;
    inputProcessorOutput->data( name, self )->right._buffer[ii] = 0;
  }

  inputProcessorOutput->data( name, self )->sampleRate = sampleRate;
  inputProcessorOutput->data( name, self )->framesOnPort = fop;
  inputProcessorOutput->data( name, self )->bytesPerFrame = bytesPerFrame;
  inputProcessorOutput->data( name, self )->lastFrameIndex = 0;
  
  inputProcessorOutput->write( name, self );
  
  return genom_ok;
}

/* publishPort ------------------------------------------------------------- */

genom_event
publishInputPort(const char *name, const rosAFE_inputProcessorOutput *inputProcessorOutput, inputProcAccessorVector inChunk, genom_context self)
{
	assert ( inChunk.size() == 2 );
		
    rosAFE_TimeDomainSignalPortStruct *data;

    data = inputProcessorOutput->data( name, self );
    
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
    inputProcessorOutput->write( name, self );	
	
    return genom_ok;
}


