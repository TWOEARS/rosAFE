#include <pthread.h>

#include <memory>

#include "stateMachine.hpp"
#include "processorCommon.hpp"
#include "genom3_dataFiles.hpp"

bool checkExists ( const char *name, const rosAFE_ids *ids ) {
	if ( ( (ids->inputProcessorsSt->processorsAccessor).existsProcessorName ( name ) ) or
	     ( (ids->preProcessorsSt->processorsAccessor).existsProcessorName ( name ) ) or
	     ( (ids->gammatoneProcessorsSt->processorsAccessor).existsProcessorName ( name ) ) or
	     ( (ids->ihcProcessorsSt->processorsAccessor).existsProcessorName ( name ) ) or
	     ( (ids->ildProcessorsSt->processorsAccessor).existsProcessorName ( name ) )      	     
	   )
	     return true;
	return false;
}

/* --- Activity PreProc ------------------------------------------------- */

/** Validation codel existsAlready of activity PreProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
genom_event
existsAlready(const char *name, const char *upperDepName,
              const rosAFE_ids *ids, genom_context self)
{
  if ( ! checkExists ( upperDepName, ids ) ) {
	return rosAFE_e_noUpperDependencie( self );
  } else if ( checkExists ( name, ids ) ) {
	return rosAFE_e_existsAlready( self );
  } else return genom_ok;
}


/* --- Activity GammatoneProc ------------------------------------------- */

/** Validation codel existsAlready of activity GammatoneProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Activity IhcProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IhcProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Activity IldProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IldProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Function getSignals ---------------------------------------------- */

/** Codel getSignal of function getSignals.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory, rosAFE_e_badIndexParam.
 */
genom_event
getSignal(uint64_t *startIndex, uint64_t endIndex,
          rosAFE_dataObjSt *signals, const rosAFE_ids *ids,
          genom_context self)
{
  if (endIndex < *startIndex)
	return rosAFE_e_badIndexParam( self );
  
  return genom_ok;
}


/* --- Function getParameters ------------------------------------------- */

/** Codel getParameters of function getParameters.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory.
 */
genom_event
getParameters(const rosAFE_ids *ids, rosAFE_parameters *parameters,
              genom_context self)
{
	
/* ****************************  Input START  ************************************ */

  size_t sizeInputProc = ids->inputProcessorsSt->processorsAccessor.getSize();
  parameters->input._length = sizeInputProc;
  
  if (genom_sequence_reserve(&(parameters->input), sizeInputProc))
	return rosAFE_e_noMemory( self );

  for ( size_t ii = 0 ; ii < sizeInputProc ; ++ii ) {
	 
    std::shared_ptr < InputProc > thisProcessor = ids->inputProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
	parameters->input._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
	parameters->input._buffer[ii].in_doNormalize = thisProcessor->get_in_doNormalize();
	parameters->input._buffer[ii].in_normalizeValue = thisProcessor->get_in_normalizeValue();
    
    thisProcessor.reset();
  }
  
/* ****************************  PREPR START  ************************************ */
  size_t sizePreProc = ids->preProcessorsSt->processorsAccessor.getSize();

  parameters->preProc._length = sizePreProc;  
  if ( genom_sequence_reserve(&(parameters->preProc), sizePreProc) )
	return rosAFE_e_noMemory( self );

 for ( size_t ii = 0 ; ii < parameters->preProc._length ; ++ii ) {
	 
    std::shared_ptr < PreProc > thisProcessor = ids->preProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
    parameters->preProc._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
    // parameters->preProc._buffer[ii].upperDepName = strdup( thisProcessor->upperProcPtr->getName().c_str() );
    	
    parameters->preProc._buffer[ii].pp_bRemoveDC = thisProcessor->get_pp_bRemoveDC();
    parameters->preProc._buffer[ii].pp_cutoffHzDC = thisProcessor->get_pp_cutoffHzDC();
    parameters->preProc._buffer[ii].pp_bPreEmphasis = thisProcessor->get_pp_bPreEmphasis();
    parameters->preProc._buffer[ii].pp_coefPreEmphasis = thisProcessor->get_pp_coefPreEmphasis();
    parameters->preProc._buffer[ii].pp_bNormalizeRMS = thisProcessor->get_pp_bNormalizeRMS();
    parameters->preProc._buffer[ii].pp_intTimeSecRMS = thisProcessor->get_pp_intTimeSecRMS();
    parameters->preProc._buffer[ii].pp_bLevelScaling = thisProcessor->get_pp_bLevelScaling();
    parameters->preProc._buffer[ii].pp_refSPLdB = thisProcessor->get_pp_refSPLdB();
    parameters->preProc._buffer[ii].pp_bMiddleEarFiltering = thisProcessor->get_pp_bMiddleEarFiltering();   
    parameters->preProc._buffer[ii].pp_middleEarModel = strdup( thisProcessor->get_pp_middleEarModel().c_str() );
    parameters->preProc._buffer[ii].pp_bUnityComp = thisProcessor->get_pp_bUnityComp();
    
    thisProcessor.reset();
  }

/* *****************************  PREPR END  ************************************* */


/* **************************  GAMMATONE START  ********************************** */
 /* 
  uint32_t sizeGammatoneProc = ids->gammatoneProcessorsSt->processorsAccessor.getSize();
  if ( parameters->paramsGammatoneProc._length > sizeGammatoneProc )
	parameters->paramsGammatoneProc._length = 0;
  
  if ( parameters->paramsGammatoneProc._length != sizeGammatoneProc ) {	
	if (genom_sequence_reserve(&(parameters->paramsGammatoneProc), sizeGammatoneProc))
		return rosAFE_e_noMemory( self );
	parameters->paramsGammatoneProc._length = sizeGammatoneProc;	
  }

 for (uint32_t ii = 0 ; ii < parameters->paramsGammatoneProc._length ; ii++) {
	 
	gammatoneProcPtr thisProcessor = ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
    parameters->paramsGammatoneProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
    parameters->paramsGammatoneProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    parameters->paramsGammatoneProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    parameters->paramsGammatoneProc._buffer[ii].fb_type = strdup( thisParams["fb_type"].c_str() );
    parameters->paramsGammatoneProc._buffer[ii].fb_lowFreqHz = thisParams.get<float>("fb_lowFreqHz");
    parameters->paramsGammatoneProc._buffer[ii].fb_highFreqHz = thisParams.get<float>("fb_highFreqHz");
    parameters->paramsGammatoneProc._buffer[ii].fb_nERBs = thisParams.get<float>("fb_nERBs");
    parameters->paramsGammatoneProc._buffer[ii].fb_nChannels = thisParams.get<unsigned long>("fb_nChannels");
    parameters->paramsGammatoneProc._buffer[ii].fb_cfHz = thisParams.get<unsigned long>("fb_cfHz");
    parameters->paramsGammatoneProc._buffer[ii].fb_nGamma = thisParams.get<unsigned long>("fb_nGamma");
    parameters->paramsGammatoneProc._buffer[ii].fb_bwERBs = thisParams.get<float>("fb_bwERBs");
    
    thisProcessor.reset();
  }*/
/* ***************************  GAMMATONE END  *********************************** */


/* *****************************  IHC START  ************************************* */
/*
  uint32_t sizeIhcProc = ids->ihcProcessorsSt->processorsAccessor.getSize();
  if ( parameters->paramsIhcProc._length > sizeIhcProc )
	parameters->paramsIhcProc._length = 0;
  
  if ( parameters->paramsIhcProc._length != sizeIhcProc ) {
	parameters->paramsIhcProc._length = sizeIhcProc;
	if (genom_sequence_reserve(&(parameters->paramsIhcProc), sizeIhcProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < parameters->paramsIhcProc._length ; ii++) {
	 
	ihcProcPtr thisProcessor = ids->ihcProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	parameters->paramsIhcProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
	parameters->paramsIhcProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    parameters->paramsIhcProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    parameters->paramsIhcProc._buffer[ii].ihc_method = strdup( thisParams["ihc_method"].c_str() );
    
    thisProcessor.reset();
  }
  */
/* ******************************  IHC END  ************************************** */

/* *****************************  ILD START  ************************************* */
  /*
  uint32_t sizeIldProc = ids->ildProcessorsSt->processorsAccessor.getSize();
  if ( parameters->paramsIldProc._length > sizeIldProc )
	parameters->paramsIldProc._length = 0;
    
  if ( parameters->paramsIldProc._length != sizeIldProc ) {	
	parameters->paramsIldProc._length = sizeIldProc;
	if (genom_sequence_reserve(&(parameters->paramsIldProc), sizeIldProc))
		return rosAFE_e_noMemory( self );
  }

 for (uint32_t ii = 0 ; ii < parameters->paramsIldProc._length ; ii++) {
	 
	ildProcPtr thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( ii );
	apfMap thisParams = thisProcessor->getCurrentParameters();
		  
	parameters->paramsIldProc._buffer[ii].name = strdup( thisProcessor->getProcessorInfo().name.c_str() );
	parameters->paramsIldProc._buffer[ii].upperDepName = strdup( thisProcessor->getInProcessorInfo(0).name.c_str() );
    parameters->paramsIldProc._buffer[ii].fsOut = thisProcessor->getFsOut();
    parameters->paramsIldProc._buffer[ii].ild_wname = strdup( thisParams["ild_wname"].c_str() );
    parameters->paramsIldProc._buffer[ii].ild_wSizeSec = thisParams.get<float>("ild_wSizeSec");
    parameters->paramsIldProc._buffer[ii].ild_hSizeSec = thisParams.get<float>("ild_hSizeSec");
        
    thisProcessor.reset();
  }*/
/* ******************************  ILD END  ************************************** */
	
	
  return genom_ok;
}


/* --- Function modifyParameter ----------------------------------------- */

/** Codel modifyParameter of function modifyParameter.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory, rosAFE_e_noSuchProcessor,
 *        rosAFE_e_noSuchParameter.
 */
genom_event
modifyParameter(const char *nameProc, const char *nameParam,
                const char *newValue, const rosAFE_ids *ids,
                genom_context self)
{
  return rosAFE_e_noSuchProcessor( self );
}


/* --- Function removeProcessor ----------------------------------------- */

/** Codel removeProcessor of function removeProcessor.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noSuchProcessor.
 */
genom_event
removeProcessor(const char *name, rosAFE_flagMap **flagMapSt,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{
  SM::removeFlag( name, newDataMapSt, self );
  if ( SM::removeFlag( name, flagMapSt, self ) ) {
	// sleep(1);
	return genom_ok;  
  }
  else
	return rosAFE_e_noSuchProcessor( self );
}
