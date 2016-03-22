#include "stateMachine.hpp"

/** Codel removePependencie of function RemovePependencie.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
existsAlreadyInput(const char *name,
                   const rosAFE_inputProcessors *inputProcessorsSt,
                   genom_context self)
{
  if ( inputProcessorsSt->processorsAccessor->existsProcessorName ( name ) )
	return rosAFE_e_existsAlready( self );
  return genom_ok;
}


/* --- Activity PreProc ------------------------------------------------- */

/** Validation codel existsAlreadyPreProc of activity PreProc.
 *
 * Returns genom_ok.
 * Throws .
 */
genom_event
existsAlreadyPreProc(const char *name,
                     const rosAFE_preProcessors *preProcessorsSt,
                     genom_context self)
{
  if ( preProcessorsSt->processorsAccessor->existsProcessorName ( name ) )
	return rosAFE_e_existsAlready( self );
  return genom_ok;
}


/* --- Function RemoveProcessor ----------------------------------------- */

/** Codel removeProcessor of function RemoveProcessor.
 *
 * Returns genom_ok.
 */
genom_event
removeProcessor(const char *name, rosAFE_flagMap **flagMapSt,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{
  SM::removeFlag( name, newDataMapSt, self );  
  SM::removeFlag( name, flagMapSt, self );
  
  return genom_ok;
}
