#include "processorCommon.hpp"
#include "Ports.hpp"
#include "stateMachine.hpp"

/* --- Task sendToMatlab ------------------------------------------------ */


/** Codel initSendToMatlabTask of task sendToMatlab.
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


/* --- Activity SendToMatlab -------------------------------------------- */

/** Codel initSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noMemory.
 */
genom_event
initSendToMatlab(const rosAFE_dataObj *dataObj, const rosAFE_ids *ids,
                 const rosAFE_runningProcessors *runningProcessors,
                 rosAFE_flagMap **flagMapSt, genom_context self)
{
  struct timeval tv;
  rosAFE_dataObjSt *data;
  data = dataObj->data( self );

  rosAFE_RunningProcessorsSt *running;
  running = runningProcessors->data( self );

  uint32_t numTDS = running->paramsInputProc._length + running->paramsPreProc._length;

  data->allTDSPorts._length = numTDS;
  if (genom_sequence_reserve(&(data->allTDSPorts), numTDS))
	return rosAFE_e_noMemory( self );

  for (uint32_t ii = 0 ; ii < running->paramsInputProc._length ; ii++) {   
	inputProcPtr thisProc = ids->inputProcessorsSt->processorsAccessor.getProcessor ( ii );
	
	SM::addFlag( "SendInput", thisProc->getProcessorInfo().name.c_str(), flagMapSt, self );
	SM::riseFlag ( "SendInput", flagMapSt, self);
	    
	thisProc->calcOldData();
	PORT::publishTDSPort ( thisProc->getProcessorInfo().name.c_str(), dataObj, thisProc->getOldDataAccesor(), thisProc->getFrameIndex(), ii, self );
	
	SM::removeFlag ( "SendInput", flagMapSt, self );
	thisProc.reset();
  }

  for (uint32_t ii = 0 ; ii < running->paramsPreProc._length ; ii++) {
	preProcPtr thisProc = ids->preProcessorsSt->processorsAccessor.getProcessor ( ii );

	SM::addFlag( "SendPreProc", thisProc->getProcessorInfo().name.c_str(), flagMapSt, self );
	
	thisProc->calcOldData();
	PORT::publishTDSPort ( thisProc->getProcessorInfo().name.c_str(), dataObj, thisProc->getOldDataAccesor(), thisProc->getFrameIndex(), running->paramsInputProc._length + ii, self );

	SM::removeFlag ( "SendPreProc", flagMapSt, self );
	
	thisProc.reset();
  }

/*	
  uint32_t numTFS = running->paramsGammatoneProc._length + running->paramsIhcProc._length + running->paramsIldProc._length;
	
  data->allTFSPorts._length = numTFS;
  if (genom_sequence_reserve(&(data->allTDSPorts), numTFS))
	return rosAFE_e_noMemory( self );
*/

  gettimeofday(&tv, NULL);
	 
  data->header.seq += 1;
  data->header.stamp.sec = tv.tv_sec;
  data->header.stamp.usec = tv.tv_usec;
  
  dataObj->write( self );
  return rosAFE_ether;
}

/** Codel stopSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noMemory.
 */
genom_event
stopSendToMatlab(genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_ether;
}
