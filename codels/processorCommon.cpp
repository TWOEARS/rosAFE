#include "processorCommon.hpp"

    genom_event
    PC::execAnyProc( const char *nameProc, rosAFE_ids *ids, genom_context self ) {
		
	  if ( ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
		ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->processChunk( );
		return genom_ok;
	  }
	  if ( ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
		ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc )->processChunk( );
		return genom_ok;
	  }
	  if ( ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
		ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc )->processChunk( );
		return genom_ok;
	  }
	  if ( ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
		ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc )->processChunk( );
		return genom_ok;
	  }		
		
	}
