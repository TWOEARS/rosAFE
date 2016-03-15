#include "stateMachine.hpp"

using namespace openAFE;

/* --- Task preProc ----------------------------------------------------- */


/* --- Activity PreProc ------------------------------------------------- */

/** Codel startPreProc of activity PreProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
startPreProc(char **name, char **upperDepName, uint32_t *fsIn,
             uint32_t *fsOut, rosAFE_preProcessors **preProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             const rosAFE_inputProcessors *inputProcessorsSt,
             genom_context self)
{
  std::string nameS = *name;
  std::string upperDepS = *upperDepName;
  
  inputProcPtr upperDepProc = inputProcessorsSt->processorsAccessor->getProcessor( upperDepS );
  
  apf::parameter_map params;
  preProcPtr preProcessor (new PreProc<preT>( nameS , *fsIn, *fsOut, 10, params) ); // test

  preProcessor->addInputProcessor ( upperDepProc );
  
  /* Adding this procesor to the ids */
  (*preProcessorsSt)->processorsAccessor->addProcessor( preProcessor );

  SM::addFlag( name, upperDepName, flagMapSt, self );
  SM::addFlag( name, upperDepName, newDataMapSt, self );
	      
  return rosAFE_waitExec;
}

/** Codel waitExecPreProc of activity PreProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
waitExecPreProc(char **name, char **upperDepName,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{   
  // If there is no new data, we will wait
  unsigned int check = SM::checkFlag( name, upperDepName, newDataMapSt, self);
  
  if (check == 0)
	return rosAFE_pause_waitExec;
  if (check == -1)
	return rosAFE_stop;  // FIXME : or ether ?
  /* Nothing here */

  // That data is now old
  SM::fallFlag ( name, upperDepName, newDataMapSt, self);
  return rosAFE_exec;

}

/** Codel execPreProc of activity PreProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
execPreProc(char **name, char **upperDepName,
            rosAFE_preProcessors **preProcessorsSt,
            rosAFE_flagMap **flagMapSt, genom_context self)
{
  std::string nameS = *name;
  std::cout << "              " << *name << std::endl;

  (*preProcessorsSt)->processorsAccessor->getProcessor( nameS )->processChunk( ); 
    
    
  // Finished with this data. The upperDep can overwite it.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
  
  return rosAFE_waitRelease;
}

/** Codel waitReleasePreProc of activity PreProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
waitReleasePreProc(char **name, rosAFE_flagMap **flagMapSt,
                   genom_context self)
{
  /* Waiting for all childs */
  if ( ! SM::checkFlag( name, flagMapSt, self) )
	return rosAFE_pause_waitRelease;  

  /* Rising the flag (if any) */
  SM::riseFlag ( name, flagMapSt, self);

  return rosAFE_release;
}

/** Codel releasePreProc of activity PreProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
releasePreProc(char **name, rosAFE_preProcessors **preProcessorsSt,
               rosAFE_flagMap **newDataMapSt, genom_context self)
{
  std::string nameS = *name;
  (*preProcessorsSt)->processorsAccessor->getProcessor( nameS )->appendChunk( ); 
  SM::riseFlag ( name, newDataMapSt, self);
  
  return rosAFE_pause_waitExec;
}

/** Codel stopPreProc of activity PreProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noData.
 */
genom_event
stopPreProc(char **name, rosAFE_preProcessors **preProcessorsSt,
            genom_context self)
{
  std::string nameS = *name;
  //(*preProcessorsSt)->processorsAccessor->removeProcessor( nameS );

  return rosAFE_ether;
}
