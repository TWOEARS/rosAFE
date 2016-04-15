#include "processorCommon.hpp"

/* --- Task sendToMatlab ------------------------------------------------ */


/* --- Activity SendToMatlab -------------------------------------------- */

/** Codel initSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_ether.
 */
genom_event
initSendToMatlabTask(const rosAFE_dataObj *dataObj,
                     const rosAFE_runningProcessors *runningProcessors,
                     genom_context self)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
	 
  dataObj->data( self )->header.seq = 0;
  dataObj->data( self )->header.stamp.sec = tv.tv_sec;
  dataObj->data( self )->header.stamp.usec = tv.tv_usec;
	    
  dataObj->write( self );
  	
  return rosAFE_ether;
}

/** Codel send of activity SendToMatlab.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_ether.
 */
genom_event
initSendToMatlab(const rosAFE_dataObj *dataObj, const rosAFE_ids *ids,
                 const rosAFE_runningProcessors *runningProcessors,
                 genom_context self)
{
	
  struct timeval tv;
  rosAFE_dataObjSt *data;
  data = dataObj->data( self );

  rosAFE_RunningProcessorsSt *running;  
  running = runningProcessors->data( self );

  gettimeofday(&tv, NULL);
	 
  data->header.seq += 1;
  data->header.stamp.sec = tv.tv_sec;
  data->header.stamp.usec = tv.tv_usec;
	
  uint32_t numTDS = running->paramsInputProc._length + running->paramsPreProc._length; 

  if ( data->allTDSPorts._length > numTDS )
	data->allTDSPorts._length = 0;
  
  if ( data->allTDSPorts._length != numTDS ) {
	data->allTDSPorts._length = numTDS;
	if (genom_sequence_reserve(&(data->allTDSPorts), numTDS))
		return rosAFE_e_noMemory( self );
  }
  
  for (uint32_t ii = 0 ; ii < data->allTDSPorts._length ; ii++) {
	 
	// ids->
  }  
 
  dataObj->write( self );
  return rosAFE_ether;
}

/** Codel stopSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 */
genom_event
stopSendToMatlab(genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_ether;
}
