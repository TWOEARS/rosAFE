#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task gammatoneProc ----------------------------------------------- */


/* --- Activity GammatoneProc ------------------------------------------- */

/** Codel startGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
startGammatoneProc(const char *name, const char *upperDepName,
                   uint32_t fsOut,
                   rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                   rosAFE_flagMap **flagMapSt,
                   rosAFE_flagMap **newDataMapSt,
                   rosAFE_preProcessors **preProcessorsSt,
                   const rosAFE_infos *infos, const char *type,
                   float lowFreqHz, float highFreqHz, float nERBs,
                   uint32_t nChannels, uint32_t cfHz, uint32_t nGamma,
                   float bwERBs, genom_context self)
{
  preProcPtr upperDepProc = ((*preProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("type", type);
  params.set("lowFreqHz", lowFreqHz);
  params.set("highFreqHz", highFreqHz);
  params.set("nERBs", nERBs);
  params.set("nChannels", nChannels);
  params.set("cfHz", cfHz);
  params.set("nGamma", nGamma);
  params.set("bwERBs", bwERBs);
	  
  gammatoneProcPtr gammatoneProcessor (new GamamtoneProc<gammatoneT>( name, upperDepProc->getFsOut(), fsOut, infos->innerBufferSize_s, params) );
  gammatoneProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*gammatoneProcessorsSt)->processorsAccessor).addProcessor( gammatoneProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  gammatoneProcessor.reset();
  /* Initialization of the output port */
  	      
  return rosAFE_waitExec;
}

/** Codel waitExec of activity GammatoneProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service PreProc */


/** Codel execGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
execGammatoneProc(const char *name, const char *upperDepName,
                  const rosAFE_gammatoneProcessors *gammatoneProcessorsSt,
                  rosAFE_flagMap **flagMapSt, genom_context self)
{
  std::cout << "                            " << name << std::endl;    
    
    
  // Finished with this data. The upperDep can overwite it.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
  
  return rosAFE_waitRelease;
}

/** Codel waitRelease of activity GammatoneProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service InputProc */


/** Codel releaseGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
releaseGammatoneProc(const char *name,
                     rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                     rosAFE_flagMap **newDataMapSt,
                     genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_pause_waitExec;
}

/** Codel deleteGammatoneProc of activity GammatoneProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
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
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
stopGammatoneProc(rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
                  genom_context self)
{
  ((*gammatoneProcessorsSt)->processorsAccessor).clear();

  delete (*gammatoneProcessorsSt);
  
  return rosAFE_ether;
}
