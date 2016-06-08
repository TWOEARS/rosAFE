#include "acrosAFE.h"
#include "rosAFE_c_types.h"

#include <memory>

#include "genom3_dataFiles.hpp"
#include "stateMachine.hpp"
#include "Ports.hpp"


/* --- Task ildProc ----------------------------------------------------- */


/* --- Activity IldProc ------------------------------------------------- */

/** Codel startIldProc of activity IldProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
startIldProc(const char *name, const char *upperDepName,
             rosAFE_ildProcessors **ildProcessorsSt,
             rosAFE_flagMap **flagMapSt, rosAFE_flagMap **newDataMapSt,
             rosAFE_ihcProcessors **ihcProcessorsSt,
             const rosAFE_infos *infos, const rosAFE_ildPort *ildPort,
             const char *ild_wname, double ild_wSizeSec,
             double ild_hSizeSec, genom_context self)
{
  std::cout << "                    start of ild" << std::endl;
	
  std::shared_ptr < IHCProc > upperDepProc = ((*ihcProcessorsSt)->processorsAccessor).getProcessor( upperDepName );
  
  std::shared_ptr < ILDProc > ildProcessor ( new ILDProc( name, upperDepProc, ild_wSizeSec, ild_hSizeSec, _hann ) );

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
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel exec of activity IldProc.
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
  ids->ildProcessorsSt->processorsAccessor.getProcessor ( name )->processChunk( );
  std::cout << "                    exec of ild" << std::endl;

  // At the end of this codel, the upperDep will be able to overwite the data.
  SM::fallFlag ( name, upperDepName, flagMapSt, self);
  return rosAFE_waitRelease;
}

/** Codel waitRelease of activity IldProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc */


/** Codel release of activity IldProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
release(const char *name, rosAFE_ids *ids,
        rosAFE_flagMap **newDataMapSt, const rosAFE_ildPort *ildPort,
        genom_context self)
{
  std::shared_ptr < ILDProc > thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( name );
  thisProcessor->releaseChunk( );
  
  // PORT::publishGammatonePort ( name, gammatonePort, thisProcessor->getLeftLastChunkAccessor(), thisProcessor->getRightLastChunkAccessor(), sizeof(double), thisProcessor->getNFR(), self );

		std::vector<std::shared_ptr< twoCTypeBlock<double> > > info = thisProcessor->getLeftLastChunkAccessor();
	
		for ( size_t iii = 0 ; iii < info.size() ; ++iii) {
			std::cout << "Channel : " << iii <<  std::endl;	
			for ( unsigned int i = 0 ; i < info[iii]->array1.second ; i++ )
				std::cout << *( info[iii]->array1.first + i ) << " ";
			std::cout << std::endl;
		}

  // Informing all the potential childs to say that this is a new chunk.
  SM::riseFlag ( name, newDataMapSt, self );
    
  thisProcessor.reset();
  return rosAFE_pause_waitExec;
}

/** Codel deleteIldProc of activity IldProc.
 *
 * Triggered by rosAFE_delete.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
deleteIldProc(const char *name, rosAFE_ildProcessors **ildProcessorsSt,
              genom_context self)
{
  /* Delting the processor */
  ((*ildProcessorsSt)->processorsAccessor).removeProcessor( name );
  return rosAFE_ether;
}

/** Codel stopIldProc of activity IldProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 *        rosAFE_e_noSuchProcessor.
 */
genom_event
stopIldProc(rosAFE_ildProcessors **ildProcessorsSt,
            genom_context self)
{
  ((*ildProcessorsSt)->processorsAccessor).clear();
  
  delete (*ildProcessorsSt);
  
  return rosAFE_ether;
}
