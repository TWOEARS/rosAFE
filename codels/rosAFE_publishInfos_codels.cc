#include "acrosAFE.h"

#include "rosAFE_c_types.h"
#include "genom3_dataFiles.hpp"

/* --- Task publishInfos ------------------------------------------------ */


/** Codel initPublish of task publishInfos.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_exec.
 * Throws rosAFE_e_noMemory.
 */
genom_event
initPublish(genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_exec;
}


/** Codel publish of task publishInfos.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_pause_exec, rosAFE_ether.
 * Throws rosAFE_e_noMemory.
 */
genom_event
publish(const rosAFE_ids *ids,
        const rosAFE_runningProcessors *runningProcessors,
        genom_context self)
{
  rosAFE_RunningProcessorsSt *data;

  data = runningProcessors->data( self );
  
/* ****************************  PREPR START  ************************************ */
  uint32_t sizePreProc = ids->preProcessorsSt->processorsAccessor.getSize();
  if ( data->paramsPreProc._length > sizePreProc )
	data->paramsPreProc._length = 0;
  
  if ( data->paramsPreProc._length < sizePreProc ) {	
	data->paramsPreProc._length = sizePreProc;
	if (genom_sequence_reserve(&(data->paramsPreProc), sizePreProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < data->paramsPreProc._length ; ii++) {
	 
	preProcPtr thisProcessor = ids->preProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	data->paramsPreProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );  	
    data->paramsPreProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    data->paramsPreProc._buffer[ii].bRemoveDC = thisParams.get<unsigned long>("bRemoveDC");
    data->paramsPreProc._buffer[ii].cutoffHzDC = thisParams.get<float>("cutoffHzDC");
    data->paramsPreProc._buffer[ii].bPreEmphasis = thisParams.get<unsigned long>("bPreEmphasis");
    data->paramsPreProc._buffer[ii].coefPreEmphasis = thisParams.get<float>("coefPreEmphasis");
    data->paramsPreProc._buffer[ii].bNormalizeRMS = thisParams.get<unsigned long>("bNormalizeRMS");
    data->paramsPreProc._buffer[ii].bBinauralRMS = thisParams.get<unsigned long>("bBinauralRMS");
    data->paramsPreProc._buffer[ii].intTimeSecRMS = thisParams.get<float>("intTimeSecRMS");
    data->paramsPreProc._buffer[ii].bLevelScaling = thisParams.get<unsigned long>("bLevelScaling");
    data->paramsPreProc._buffer[ii].refSPLdB = thisParams.get<float>("refSPLdB");
    data->paramsPreProc._buffer[ii].bMiddleEarFiltering = thisParams.get<unsigned long>("bMiddleEarFiltering");    
	data->paramsPreProc._buffer[ii].middleEarModel = strdup( thisParams["middleEarModel"].c_str() );
    
    data->paramsPreProc._buffer[ii].bUnityComp = thisParams.get<float>("bUnityComp");
    
    thisProcessor.reset();
  }

/* *****************************  PREPR END  ************************************* */


/* **************************  GAMMATONE START  ********************************** */
  
  uint32_t sizeGammatoneProc = ids->gammatoneProcessorsSt->processorsAccessor.getSize();
  if ( data->paramsGammatoneProc._length > sizeGammatoneProc )
	data->paramsGammatoneProc._length = 0;
  
  if ( data->paramsGammatoneProc._length != sizeGammatoneProc ) {	
	if (genom_sequence_reserve(&(data->paramsGammatoneProc), sizeGammatoneProc))
		return rosAFE_e_noMemory( self );
	data->paramsGammatoneProc._length = sizeGammatoneProc;	
  }

 for (uint32_t ii = 0 ; ii < data->paramsGammatoneProc._length ; ii++) {
	 
	gammatoneProcPtr thisProcessor = ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	data->paramsGammatoneProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
	data->paramsGammatoneProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    data->paramsGammatoneProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    data->paramsGammatoneProc._buffer[ii].type = strdup( thisParams["type"].c_str() );
    data->paramsGammatoneProc._buffer[ii].lowFreqHz = thisParams.get<float>("lowFreqHz");
    data->paramsGammatoneProc._buffer[ii].highFreqHz = thisParams.get<float>("highFreqHz");
    data->paramsGammatoneProc._buffer[ii].nERBs = thisParams.get<float>("nERBs");
    data->paramsGammatoneProc._buffer[ii].nChannels = thisParams.get<unsigned long>("nChannels");
    data->paramsGammatoneProc._buffer[ii].cfHz = thisParams.get<unsigned long>("cfHz");
    data->paramsGammatoneProc._buffer[ii].nGamma = thisParams.get<unsigned long>("nGamma");
    data->paramsGammatoneProc._buffer[ii].bwERBs = thisParams.get<float>("bwERBs");
    
    thisProcessor.reset();
  }
/* ***************************  GAMMATONE END  *********************************** */


/* *****************************  IHC START  ************************************* */

  uint32_t sizeIhcProc = ids->ihcProcessorsSt->processorsAccessor.getSize();
  if ( data->paramsIhcProc._length > sizeIhcProc )
	data->paramsIhcProc._length = 0;
  
  if ( data->paramsIhcProc._length != sizeIhcProc ) {
	data->paramsIhcProc._length = sizeIhcProc;
	if (genom_sequence_reserve(&(data->paramsIhcProc), sizeIhcProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < data->paramsIhcProc._length ; ii++) {
	 
	ihcProcPtr thisProcessor = ids->ihcProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	data->paramsIhcProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
	data->paramsIhcProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    data->paramsIhcProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    data->paramsIhcProc._buffer[ii].method = strdup( thisParams["method"].c_str() );
    
    thisProcessor.reset();
  }
  
/* ******************************  IHC END  ************************************** */

/* *****************************  ILD START  ************************************* */
  
  uint32_t sizeIldProc = ids->ildProcessorsSt->processorsAccessor.getSize();
  if ( data->paramsIldProc._length > sizeIldProc )
	data->paramsIldProc._length = 0;
    
  if ( data->paramsIldProc._length != sizeIldProc ) {	
	data->paramsIldProc._length = sizeIldProc;
	if (genom_sequence_reserve(&(data->paramsIldProc), sizeIldProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < data->paramsIldProc._length ; ii++) {
	 
	ildProcPtr thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	data->paramsIldProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
	data->paramsIldProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    data->paramsIldProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    data->paramsIldProc._buffer[ii].ild_wname = strdup( thisParams["ild_wname"].c_str() );
    data->paramsIldProc._buffer[ii].ild_wSizeSec = thisParams.get<float>("ild_wSizeSec");
    data->paramsIldProc._buffer[ii].ild_hSizeSec = thisParams.get<float>("ild_hSizeSec");
        
    thisProcessor.reset();
  }
/* ******************************  ILD END  ************************************** */
  
  runningProcessors->write( self );

  return rosAFE_pause_exec;
}


/** Codel stopPublish of task publishInfos.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noMemory.
 */
genom_event
stopPublish(const rosAFE_runningProcessors *runningProcessors,
            genom_context self)
{
  rosAFE_RunningProcessorsSt *data;

  data = runningProcessors->data( self );	
	
  data->paramsPreProc._release( data->paramsPreProc._buffer );
  data->paramsGammatoneProc._release( data->paramsGammatoneProc._buffer );
  data->paramsIhcProc._release( data->paramsIhcProc._buffer );
  data->paramsIldProc._release( data->paramsIldProc._buffer );

  return rosAFE_ether;
}
