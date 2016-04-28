#include "processorCommon.hpp"
#include "stateMachine.hpp"

/* --- Task preProc ----------------------------------------------------- */


/* --- Activity PreProc ------------------------------------------------- */

/** Codel startPreProc of activity PreProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
startPreProc(const char *name, const char *upperDepName,
             uint32_t fsOut, rosAFE_preProcessors **preProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             rosAFE_inputProcessors **inputProcessorsSt,
             const rosAFE_infos *infos, uint16_t pp_bRemoveDC,
             float pp_cutoffHzDC, uint16_t pp_bPreEmphasis,
             float pp_coefPreEmphasis, uint16_t pp_bNormalizeRMS,
             uint16_t pp_bBinauralRMS, float pp_intTimeSecRMS,
             uint16_t pp_bLevelScaling, float pp_refSPLdB,
             uint16_t pp_bMiddleEarFiltering,
             const char *pp_middleEarModel, float pp_bUnityComp,
             genom_context self)
{
  inputProcPtr upperDepProc = ((*inputProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("pp_bRemoveDC", pp_bRemoveDC);
  params.set("pp_cutoffHzDC", pp_cutoffHzDC);
  params.set("pp_bPreEmphasis", pp_bPreEmphasis);
  params.set("pp_coefPreEmphasis", pp_coefPreEmphasis);
  params.set("pp_bNormalizeRMS", pp_bNormalizeRMS);
  params.set("pp_bBinauralRMS", pp_bBinauralRMS);
  params.set("pp_intTimeSecRMS", pp_intTimeSecRMS);
  params.set("pp_bLevelScaling", pp_bLevelScaling);
  params.set("pp_bPreEmphasis", pp_bPreEmphasis);
  params.set("pp_refSPLdB", pp_refSPLdB);
  params.set("pp_bMiddleEarFiltering", pp_bMiddleEarFiltering);
  params.set("pp_middleEarModel", pp_middleEarModel);
  params.set("pp_bUnityComp", pp_bUnityComp);
	  
  preProcPtr preProcessor (new PreProc<preT>( name, upperDepProc->getFsOut(), fsOut, infos->bufferSize_s, params) );
  preProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*preProcessorsSt)->processorsAccessor).addProcessor( preProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  preProcessor.reset();
  return rosAFE_waitExec;
}

/** Codel waitExec of activity PreProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
waitExec(const char *name, const char *upperDepName,
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

/** Codel exec of activity PreProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
exec(const char *name, const char *upperDepName, rosAFE_ids *ids,
     rosAFE_flagMap **flagMapSt, genom_context self)
{
  PC::execAnyProc( name, ids, self );
  
  // At the end of this codel, the upperDep will be able to overwite the data.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
	  
  return rosAFE_waitRelease;
}

/** Codel waitRelease of activity PreProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service InputProc */


/** Codel release of activity PreProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
release(const char *name, rosAFE_ids *ids,
        rosAFE_flagMap **newDataMapSt, genom_context self)
{
  PC::releaseAnyProc( name, ids, self);
  
  SM::riseFlag ( name, newDataMapSt, self);

  return rosAFE_pause_waitExec;
}

/** Codel deletePreProc of activity PreProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
deletePreProc(const char *name, rosAFE_preProcessors **preProcessorsSt,
              genom_context self)
{
  /* Delting the processor */
  ((*preProcessorsSt)->processorsAccessor).removeProcessor( name );
  return rosAFE_ether;
}

/** Codel stopPreProc of activity PreProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
stopPreProc(rosAFE_preProcessors **preProcessorsSt,
            genom_context self)
{
  ((*preProcessorsSt)->processorsAccessor).clear();
  
  delete (*preProcessorsSt);
  
  return rosAFE_ether;
}
