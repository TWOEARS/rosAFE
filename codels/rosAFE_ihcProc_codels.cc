#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task ihcProc ----------------------------------------------------- */


/* --- Activity IhcProc ------------------------------------------------- */

/** Codel startIhcProc of activity IhcProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
startIhcProc(const char *name, const char *upperDepName,
             uint32_t fsOut, rosAFE_ihcProcessors **ihcProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             rosAFE_gammatoneProcessors **gammatoneProcessorsSt,
             const rosAFE_infos *infos, const char *ihc_method,
             genom_context self)
{
  gammatoneProcPtr upperDepProc = ((*gammatoneProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("ihc_method", ihc_method);
  
  ihcProcPtr ihcProcessor (new IHCProc<ihcT>( name, upperDepProc->getFsOut(), fsOut, infos->innerBufferSize_s, params) );
  ihcProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*ihcProcessorsSt)->processorsAccessor).addProcessor( ihcProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  ihcProcessor.reset();
  return rosAFE_waitExec;
}

/** Codel waitExec of activity IhcProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel exec of activity IhcProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel waitRelease of activity IhcProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service InputProc */


/** Codel release of activity IhcProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel deleteIhcProc of activity IhcProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
deleteIhcProc(const char *name, rosAFE_ihcProcessors **ihcProcessorsSt,
              genom_context self)
{
  ((*ihcProcessorsSt)->processorsAccessor).removeProcessor( name );
  return rosAFE_ether;
}

/** Codel stopIhcProc of activity IhcProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
stopIhcProc(rosAFE_ihcProcessors **ihcProcessorsSt,
            genom_context self)
{
  ((*ihcProcessorsSt)->processorsAccessor).clear();
  
  delete (*ihcProcessorsSt);
  
  return rosAFE_ether;
}
