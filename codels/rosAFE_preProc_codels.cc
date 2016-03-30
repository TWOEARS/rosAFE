#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task preProc ----------------------------------------------------- */


/* --- Activity PreProc ------------------------------------------------- */

/** Codel startPreProc of activity PreProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
startPreProc(const char *name, const char *upperDepName,
             uint32_t fsOut, rosAFE_preProcessors **preProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             rosAFE_inputProcessors **inputProcessorsSt,
             const rosAFE_TDSPorts *TDSPorts,
             const rosAFE_infos *infos, uint16_t bRemoveDC,
             float cutoffHzDC, uint16_t bPreEmphasis,
             float coefPreEmphasis, uint16_t bNormalizeRMS,
             uint16_t bBinauralRMS, float intTimeSecRMS,
             uint16_t bLevelScaling, float refSPLdB,
             uint16_t bMiddleEarFiltering, const char *middleEarModel,
             float bUnityComp, genom_context self)
{
  inputProcPtr upperDepProc = ((*inputProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("bRemoveDC", bRemoveDC);
  params.set("cutoffHzDC", cutoffHzDC);
  params.set("bPreEmphasis", bPreEmphasis);
  params.set("coefPreEmphasis", coefPreEmphasis);
  params.set("bNormalizeRMS", bNormalizeRMS);
  params.set("bBinauralRMS", bBinauralRMS);
  params.set("intTimeSecRMS", intTimeSecRMS);
  params.set("bLevelScaling", bLevelScaling);
  params.set("bPreEmphasis", bPreEmphasis);
  params.set("refSPLdB", refSPLdB);
  params.set("bMiddleEarFiltering", bMiddleEarFiltering);
  params.set("middleEarModel", middleEarModel);
  params.set("bUnityComp", bUnityComp);
	  
  preProcPtr preProcessor (new PreProc<preT>( name, upperDepProc->getFsOut(), fsOut, infos->innerBufferSize_s, params) );
  preProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*preProcessorsSt)->processorsAccessor).addProcessor( preProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  preProcessor.reset();
  
  /* Initialization of the output port */
  initTDSPort( name, TDSPorts, fsOut, infos->bufferSize_s, sizeof(preT), self );
  	      
  return rosAFE_waitExec;
}

/** Codel waitExec of activity PreProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
waitExec (const char *name, const char *upperDepName,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{   
  // If there is no new data, we will wait
  int check = SM::checkFlag( name, upperDepName, newDataMapSt, self);
  
  if (check == 0)
	return rosAFE_pause_waitExec;
  if (check == 2) {
	return rosAFE_delete;
  }
  /* Nothing here */

  // That data is now old
  SM::fallFlag ( name, upperDepName, newDataMapSt, self);
  return rosAFE_exec;

}

/** Codel execPreProc of activity PreProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
execPreProc(const char *name, const char *upperDepName,
            rosAFE_preProcessors **preProcessorsSt,
            rosAFE_flagMap **flagMapSt, genom_context self)
{
  std::cout << "              " << name << std::endl;

  (((*preProcessorsSt)->processorsAccessor).getProcessor( name ))->processChunk( );
      
  // Finished with this data. The upperDep can overwite it.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
  
  return rosAFE_waitRelease;
}

/** Codel waitRelease of activity PreProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service InputProc */


/** Codel releasePreProc of activity PreProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
releasePreProc(const char *name,
               rosAFE_preProcessors **preProcessorsSt,
               rosAFE_flagMap **newDataMapSt,
               const rosAFE_TDSPorts *TDSPorts, genom_context self)
{
  preProcPtr thisProcessor = ((*preProcessorsSt)->processorsAccessor).getProcessor( name );
	
  thisProcessor->appendChunk( );
  thisProcessor->calcLastChunk( );
  
  /* Publishing on the output port */
  publishTDSPort( name, TDSPorts, thisProcessor->getLastChunkAccesor(), self );
  
  SM::riseFlag ( name, newDataMapSt, self);
  
  thisProcessor.reset();
  return rosAFE_pause_waitExec;
}

/** Codel deletePreProc of activity PreProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
deletePreProc(const char *name, rosAFE_preProcessors **preProcessorsSt,
              const rosAFE_TDSPorts *TDSPorts, genom_context self)
{
  /* Delting the port out */
  deleteTDSPort( name, TDSPorts, self );

  /* Delting the processor */
  ((*preProcessorsSt)->processorsAccessor).removeProcessor( name );
  return rosAFE_ether;
}

/** Codel stopPreProc of activity PreProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
stopPreProc(rosAFE_preProcessors **preProcessorsSt,
            genom_context self)
{
  ((*preProcessorsSt)->processorsAccessor).clear();
  
  delete (*preProcessorsSt);
  
  return rosAFE_ether;
}
