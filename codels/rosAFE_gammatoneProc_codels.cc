#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task gammatoneProc ----------------------------------------------- */


/* --- Activity GammatoneProc ------------------------------------------- */

/** Codel startGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
startGammatoneProc(const char *name, const char *upperDepName,
                   uint32_t fsOut,
                   rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                   rosAFE_flagMap **flagMapSt,
                   rosAFE_flagMap **newDataMapSt,
                   rosAFE_preProcessors **preProcessorsSt,
                   const rosAFE_infos *infos, const char *fb_type,
                   float fb_lowFreqHz, float fb_highFreqHz,
                   float fb_nERBs, uint32_t fb_nChannels,
                   uint32_t fb_cfHz, uint32_t fb_nGamma,
                   float fb_bwERBs, genom_context self)
{
  preProcPtr upperDepProc = ((*preProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("fb_type", fb_type);
  params.set("fb_lowFreqHz", fb_lowFreqHz);
  params.set("fb_highFreqHz", fb_highFreqHz);
  params.set("fb_nERBs", fb_nERBs);
  params.set("fb_nChannels", fb_nChannels);
  params.set("fb_cfHz", fb_cfHz);
  params.set("fb_nGamma", fb_nGamma);
  params.set("fb_bwERBs", fb_bwERBs);
	  
  gammatoneProcPtr gammatoneProcessor (new GamamtoneProc<gammatoneT>( name, upperDepProc->getFsOut(), fsOut, infos->bufferSize_s, params) );
  gammatoneProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*gammatoneProcessorsSt)->processorsAccessor).addProcessor( gammatoneProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  gammatoneProcessor.reset();
  return rosAFE_waitExec;
}

/** Codel waitExec of activity GammatoneProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel exec of activity GammatoneProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel waitRelease of activity GammatoneProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service InputProc */


/** Codel release of activity GammatoneProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel deleteGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
deleteGammatoneProc(const char *name,
                    rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                    genom_context self)
{
  /* Deleting the processor */
  ((*gammatoneProcessorsSt)->processorsAccessor).removeProcessor( name );
  
  return rosAFE_ether;
}

/** Codel stopGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
stopGammatoneProc(rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                  genom_context self)
{
  ((*gammatoneProcessorsSt)->processorsAccessor).clear();

  delete (*gammatoneProcessorsSt);
  
  return rosAFE_ether;
}
