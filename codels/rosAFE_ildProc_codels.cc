#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task ildProc ----------------------------------------------------- */


/* --- Activity IldProc ------------------------------------------------- */

/** Codel startIldProc of activity IldProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
startIldProc(const char *name, const char *upperDepName,
             uint32_t fsOut, rosAFE_ildProcessors **ildProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             rosAFE_ihcProcessors **ihcProcessorsSt,
             const rosAFE_infos *infos, double dummy,
             genom_context self)
{
  ihcProcPtr upperDepProc = ((*ihcProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  apf::parameter_map params;
  params.set("dummy", dummy);
  
  ildProcPtr ildProcessor (new ILDProc<ildT>( name, upperDepProc->getFsOut(), fsOut, infos->innerBufferSize_s, params) );
  ildProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  ((*ildProcessorsSt)->processorsAccessor).addProcessor( ildProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );

  upperDepProc.reset();
  ildProcessor.reset();
  return rosAFE_waitExec;
}

/** Codel waitExec of activity IldProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_ether,
 *           rosAFE_delete.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service PreProc */


/** Codel execIldProc of activity IldProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
execIldProc(const char *name, const char *upperDepName,
            const rosAFE_ildProcessors *ildProcessorsSt,
            rosAFE_flagMap **flagMapSt, genom_context self)
{
  std::cout << "                                                        " << name << std::endl;  
    
  // Finished with this data. The upperDep can overwite it.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
  
  return rosAFE_waitRelease;
}

/** Codel waitRelease of activity IldProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service InputProc */


/** Codel releaseIldProc of activity IldProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
releaseIldProc(const char *name,
               rosAFE_ildProcessors **ildProcessorsSt,
               rosAFE_flagMap **newDataMapSt, genom_context self)
{
  
  SM::riseFlag ( name, newDataMapSt, self);
  return rosAFE_pause_waitExec;
}

/** Codel deleteIldProc of activity IldProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
deleteIldProc(const char *name, rosAFE_ildProcessors **ildProcessorsSt,
              genom_context self)
{

  ((*ildProcessorsSt)->processorsAccessor).removeProcessor( name );
  return rosAFE_ether;
}

/** Codel stopIldProc of activity IldProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
genom_event
stopIldProc(rosAFE_ildProcessors **ildProcessorsSt,
            genom_context self)
{
  ((*ildProcessorsSt)->processorsAccessor).clear();
  
  delete (*ildProcessorsSt);

  return rosAFE_ether;
}
