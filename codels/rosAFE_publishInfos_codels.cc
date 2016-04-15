#include "processorCommon.hpp"

/* --- Task publishInfos ------------------------------------------------ */


/** Codel initPublish of task publishInfos.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_exec.
 * Throws rosAFE_e_noMemory.
 */
genom_event
initPublish(const rosAFE_runningProcessors *runningProcessors,
            genom_context self)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
	 
  runningProcessors->data(self)->header.seq = 0;
  runningProcessors->data(self)->header.stamp.sec = tv.tv_sec;
  runningProcessors->data(self)->header.stamp.usec = tv.tv_usec;
	    
  runningProcessors->write( self );
  	
  return rosAFE_exec;
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
  struct timeval tv;
  rosAFE_RunningProcessorsSt *data;

  data = runningProcessors->data( self );

  gettimeofday(&tv, NULL);
	 
  data->header.seq += 1;
  data->header.stamp.sec = tv.tv_sec;
  data->header.stamp.usec = tv.tv_usec;
  
/* ****************************  Input START  ************************************ */

  uint32_t sizeInputProc = ids->inputProcessorsSt->processorsAccessor.getSize();
  if ( data->paramsInputProc._length > sizeInputProc )
	data->paramsInputProc._length = 0;
  
  if ( data->paramsInputProc._length != sizeInputProc ) {
	data->paramsInputProc._length = sizeInputProc;
	if (genom_sequence_reserve(&(data->paramsInputProc), sizeInputProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < data->paramsInputProc._length ; ii++) {
	 
	inputProcPtr thisProcessor = ids->inputProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
	data->paramsInputProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
    
    thisProcessor.reset();
  }
  
/* *****************************  Input END  ************************************* */


  
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
    data->paramsPreProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );	
    data->paramsPreProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    data->paramsPreProc._buffer[ii].pp_bRemoveDC = thisParams.get<unsigned long>("pp_bRemoveDC");
    data->paramsPreProc._buffer[ii].pp_cutoffHzDC = thisParams.get<float>("pp_cutoffHzDC");
    data->paramsPreProc._buffer[ii].pp_bPreEmphasis = thisParams.get<unsigned long>("pp_bPreEmphasis");
    data->paramsPreProc._buffer[ii].pp_coefPreEmphasis = thisParams.get<float>("pp_coefPreEmphasis");
    data->paramsPreProc._buffer[ii].pp_bNormalizeRMS = thisParams.get<unsigned long>("pp_bNormalizeRMS");
    data->paramsPreProc._buffer[ii].pp_bBinauralRMS = thisParams.get<unsigned long>("pp_bBinauralRMS");
    data->paramsPreProc._buffer[ii].pp_intTimeSecRMS = thisParams.get<float>("pp_intTimeSecRMS");
    data->paramsPreProc._buffer[ii].pp_bLevelScaling = thisParams.get<unsigned long>("pp_bLevelScaling");
    data->paramsPreProc._buffer[ii].pp_refSPLdB = thisParams.get<float>("pp_refSPLdB");
    data->paramsPreProc._buffer[ii].pp_bMiddleEarFiltering = thisParams.get<unsigned long>("pp_bMiddleEarFiltering");    
    data->paramsPreProc._buffer[ii].pp_middleEarModel = strdup( thisParams["pp_middleEarModel"].c_str() );
    data->paramsPreProc._buffer[ii].pp_bUnityComp = thisParams.get<float>("pp_bUnityComp");
    
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
    data->paramsGammatoneProc._buffer[ii].fb_type = strdup( thisParams["fb_type"].c_str() );
    data->paramsGammatoneProc._buffer[ii].fb_lowFreqHz = thisParams.get<float>("fb_lowFreqHz");
    data->paramsGammatoneProc._buffer[ii].fb_highFreqHz = thisParams.get<float>("fb_highFreqHz");
    data->paramsGammatoneProc._buffer[ii].fb_nERBs = thisParams.get<float>("fb_nERBs");
    data->paramsGammatoneProc._buffer[ii].fb_nChannels = thisParams.get<unsigned long>("fb_nChannels");
    data->paramsGammatoneProc._buffer[ii].fb_cfHz = thisParams.get<unsigned long>("fb_cfHz");
    data->paramsGammatoneProc._buffer[ii].fb_nGamma = thisParams.get<unsigned long>("fb_nGamma");
    data->paramsGammatoneProc._buffer[ii].fb_bwERBs = thisParams.get<float>("fb_bwERBs");
    
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
    data->paramsIhcProc._buffer[ii].ihc_method = strdup( thisParams["ihc_method"].c_str() );
    
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
