#include "Ports.hpp"


	genom_event
	PORT::iniTDS_port ( sequence_double *signal, uint32_t fop, bool initToZero, genom_context self ) {

		signal->_length = fop;
		
		if ( genom_sequence_reserve( signal, fop ) )
		return rosAFE_e_noMemory( self );	

		if ( initToZero ) {
			for ( size_t ii = 0; ii < fop; ++ii )
				signal->_buffer[ii] = 0;
		}
		return genom_ok;	
	}

	genom_event
	PORT::publishTDS_port ( sequence_double *signal, twoCTypeBlockPtr chunk, uint32_t fop, uint32_t bytesPerFrame, genom_context self ) {

		uint32_t dim1 = chunk->array1.second;
		uint32_t dim2 = chunk->array2.second;

		uint32_t fpc = dim1 + dim2; 		// amount of Frames On this Chunk
		
		if ( bytesPerFrame > 0 )
			memmove(signal->_buffer, signal->_buffer + fpc, (fop - fpc) * bytesPerFrame);		
		
		uint32_t pos, ii;
			if (dim2 == 0) {	
				for (ii = 0, pos = fop - fpc; pos < fop ; ++ii, ++pos ) {
					signal->_buffer[pos] = *(chunk->array1.first + ii);
				}
			} else if (dim1 == 0) {
					for (ii = 0, pos = fop - fpc; pos < fop ; ++ii, ++pos ) {
						signal->_buffer[pos] = *(chunk->array2.first + ii);
					}
			} else {
					for (ii = 0, pos = fop - fpc; pos < fop - dim2 ; ++ii, ++pos ) {
						signal->_buffer[pos] = *(chunk->array1.first + ii);
					}
					
					for (ii = 0, pos = fop - dim2; pos < fop ; ++ii, ++pos ) {
						signal->_buffer[pos] = *(chunk->array2.first + ii);
					}			
				}		
		return genom_ok;
	}

	genom_event
	PORT::iniTFS_port ( sequence_rosAFE_signalOneD *signal, uint32_t nChannels, uint32_t fop, bool initToZero, genom_context self ) {

	  if ( genom_sequence_reserve( signal, nChannels) )
		return rosAFE_e_noMemory( self );

	  signal->_length = nChannels;
	  
	  for ( size_t ii = 0 ; ii < nChannels ; ++ii ) {
		if ( genom_sequence_reserve(&(signal->_buffer[ii].data), fop) )
		return rosAFE_e_noMemory( self );
	  
	  	signal->_buffer[ii].data._length = fop;

		if ( initToZero ) {	  
			for ( size_t iii = 0; iii < fop; ++iii)
				signal->_buffer[ii].data._buffer[iii] = 0;
		}
	  }

	  return genom_ok;		
	}

	genom_event
	PORT::publishTFS_port ( sequence_rosAFE_signalOneD *signal, std::vector<twoCTypeBlockPtr >& chunk, uint32_t nChannels, uint32_t fop, uint32_t bytesPerFrame, genom_context self ) {

		for ( size_t ii = 0 ; ii < nChannels ; ++ii ) {
			
			uint32_t dim1 = chunk[ii]->array1.second;
			uint32_t dim2 = chunk[ii]->array2.second;
			
			uint32_t fpc = dim1 + dim2; 			// amount of Frames On this Chunk

			if ( bytesPerFrame > 0 )		
				memmove(signal->_buffer[ii].data._buffer, signal->_buffer[ii].data._buffer + fpc, (fop - fpc) * bytesPerFrame);

			uint32_t pos, iii;
			if (dim2 == 0) {	
				for (iii = 0, pos = fop - fpc; pos < fop ; ++iii, ++pos)
					signal->_buffer[ii].data._buffer[pos] = *(chunk[ii]->array1.first + iii);
			} else if (dim1 == 0) {
					for (iii = 0, pos = fop - fpc; pos < fop ; ++iii, ++pos)
						signal->_buffer[ii].data._buffer[pos] = *(chunk[ii]->array2.first + iii);
			} else {
					for (iii = 0, pos = fop - fpc; pos < fop - dim2 ; ++iii, ++pos)
						signal->_buffer[ii].data._buffer[pos] = *(chunk[ii]->array1.first + iii);
					for (iii = 0, pos = fop - dim2; pos < fop ; ++iii, ++pos)
						signal->_buffer[ii].data._buffer[pos] = *(chunk[ii]->array2.first + iii);	
				}
		}

	  return genom_ok;	
	}
			
/* Input Port ----------------------------------------------------------------- */

	genom_event
	PORT::initInputPort ( const rosAFE_inputProcPort *inputProcPort, uint32_t sampleRate,
					uint32_t bufferSize_s, genom_context self ) {
						
		  uint32_t fop =  sampleRate * bufferSize_s; // total amount of Frames On the Port 
	
		  iniTDS_port(&(inputProcPort->data( self )->left.data), fop, true, self );
		  iniTDS_port(&(inputProcPort->data( self )->right.data), fop, true, self );

		  inputProcPort->data( self )->sampleRate = sampleRate;
		  inputProcPort->data( self )->framesOnPort = fop;
		  inputProcPort->data( self )->lastFrameIndex = 0;
		  
		  inputProcPort->write( self );
		  
		  return genom_ok;
	}

	genom_event
	PORT::publishInputPort ( const rosAFE_inputProcPort *inputProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {	

			uint32_t fop = inputProcPort->data( self )->framesOnPort; 	// total amount of Frames On the Port

			publishTDS_port ( &(inputProcPort->data( self )->left.data), left, fop, bytesPerFrame, self );
			publishTDS_port ( &(inputProcPort->data( self )->right.data), right, fop, bytesPerFrame, self );
			
			inputProcPort->data( self )->lastFrameIndex = nfr;
			inputProcPort->write( self );
			
			return genom_ok;
	}

/* Pre Proc Port ----------------------------------------------------------------- */

	genom_event
	PORT::initPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, uint32_t sampleRate,
							uint32_t bufferSize_s, genom_context self ) {
								
	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  preProcPort->open( name, self );
		
	  iniTDS_port(&(preProcPort->data( name, self )->left.data), fop, true, self );
	  iniTDS_port(&(preProcPort->data( name, self )->right.data), fop, true, self );
		  
	  preProcPort->data( name, self )->sampleRate = sampleRate;
	  preProcPort->data( name, self )->framesOnPort = fop;
	  preProcPort->data( name, self )->lastFrameIndex = 0;
	  
	  preProcPort->write( name, self );
	  
	  return genom_ok;
	}

	genom_event
	PORT::publishPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {		
	   	
		uint32_t fop = preProcPort->data( name, self )->framesOnPort; // total amount of Frames On the Port

		publishTDS_port ( &(preProcPort->data( name, self )->left.data), left, fop, bytesPerFrame, self );
		publishTDS_port ( &(preProcPort->data( name, self )->right.data), right, fop, bytesPerFrame, self );
							
		preProcPort->data( name, self )->lastFrameIndex = nfr;
		preProcPort->write( name, self );	

		return genom_ok;
	}

	genom_event
	PORT::deletePreProcPort   ( const char *name, const rosAFE_preProcPort *preProcPort, genom_context self ) {
		preProcPort->close( name, self );
		return genom_ok;
	}
	
/* Gammatone Port ----------------------------------------------------------------- */

	genom_event
	PORT::initGammatonePort ( const char *name, const rosAFE_gammatonePort *gammatonePort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self ) {
							
	  gammatonePort->open( name, self );
							
	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  iniTFS_port ( &(gammatonePort->data( name, self )->left.dataN), nChannels,  fop, true, self );
	  iniTFS_port ( &(gammatonePort->data( name, self )->right.dataN), nChannels,  fop, true, self );
		
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

		publishTFS_port ( &(thisPort->left.dataN), left, thisPort->numberOfChannels, thisPort->framesOnPort, bytesPerFrame, self );
		publishTFS_port ( &(thisPort->right.dataN), right, thisPort->numberOfChannels, thisPort->framesOnPort, bytesPerFrame, self );
		
		thisPort->lastFrameIndex = nfr;
		gammatonePort->write( name, self );			
							
		return genom_ok;							
	}

	genom_event
	PORT::deleteGammatonePort   ( const char *name, const rosAFE_gammatonePort *gammatonePort, genom_context self ) {
		gammatonePort->close( name, self );
		return genom_ok;		
	}
	
/* IHC Port ----------------------------------------------------------------- */

	genom_event
	PORT::initIHCPort ( const char *name, const rosAFE_ihcPort *ihcPort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self ) {
	
	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  ihcPort->open( name, self );

	  iniTFS_port ( &(ihcPort->data( name, self )->left.dataN), nChannels,  fop, true, self );
	  iniTFS_port ( &(ihcPort->data( name, self )->right.dataN), nChannels,  fop, true, self );

	  ihcPort->data( name, self )->sampleRate = sampleRate;
	  ihcPort->data( name, self )->framesOnPort = fop;	  
	  ihcPort->data( name, self )->numberOfChannels = nChannels;
	  ihcPort->data( name, self )->lastFrameIndex = 0;
	  
	  ihcPort->write( name, self );
	  
	  return genom_ok;
	}
	
	genom_event
	PORT::publishIHCPort ( const char *name, const rosAFE_ihcPort *ihcPort, std::vector<twoCTypeBlockPtr > left,
						std::vector<twoCTypeBlockPtr > right, uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {	

		rosAFE_TimeFrequencySignalPortStruct *thisPort;
										
		thisPort = ihcPort->data( name, self );

		publishTFS_port ( &(thisPort->left.dataN), left, thisPort->numberOfChannels, thisPort->framesOnPort, bytesPerFrame, self );
		publishTFS_port ( &(thisPort->right.dataN), right, thisPort->numberOfChannels, thisPort->framesOnPort, bytesPerFrame, self );
		
		thisPort->lastFrameIndex = nfr;
		ihcPort->write( name, self );			
							
		return genom_ok;							
	}

	genom_event
	PORT::deleteIHCPort   ( const char *name, const rosAFE_ihcPort *ihcPort, genom_context self ) {
		ihcPort->close( name, self );
		return genom_ok;		
	}

/* ILD Port ----------------------------------------------------------------- */

	genom_event
	PORT::initILDPort ( const char *name, const rosAFE_ildPort *ildPort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self ) {

	  uint32_t fop =  sampleRate * bufferSize_s; /* total amount of Frames On the Port */
	  
	  ildPort->open( name, self );

	  iniTFS_port ( &(ildPort->data( name, self )->left.dataN), nChannels,  fop, true, self );

	  ildPort->data( name, self )->sampleRate = sampleRate;
	  ildPort->data( name, self )->framesOnPort = fop;	  
	  ildPort->data( name, self )->numberOfChannels = nChannels;
	  ildPort->data( name, self )->lastFrameIndex = 0;
	  
	  ildPort->write( name, self );
	  
	  return genom_ok;					
	}

	genom_event
	PORT::publishILDPort ( const char *name, const rosAFE_ildPort *ildPort, std::vector<twoCTypeBlockPtr > left,
						uint32_t bytesPerFrame, int64_t nfr, genom_context self ) {

		rosAFE_TimeFrequencySignalPortStruct *thisPort;
										
		thisPort = ildPort->data( name, self );

		publishTFS_port ( &(thisPort->left.dataN), left, thisPort->numberOfChannels, thisPort->framesOnPort, bytesPerFrame, self );
		
		thisPort->lastFrameIndex = nfr;
		ildPort->write( name, self );

	  return genom_ok;							
	}

	genom_event
	PORT::deleteILDPort   ( const char *name, const rosAFE_ildPort *ildPort, genom_context self ) {
		ildPort->close( name, self );
		return genom_ok;		
	}
