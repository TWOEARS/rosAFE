#include "stateMachine.hpp"

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
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
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
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service PreProc validation */



/* --- Activity IhcProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IhcProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service PreProc validation */



/* --- Activity IldProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IldProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready.
 */
/* already defined in service PreProc validation */



/* --- Function ModifyParameter ----------------------------------------- */

/** Codel modifyParameter of function ModifyParameter.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noSuchProcessor, rosAFE_e_noSuchParameter.
 */
genom_event
modifyParameter(const char *nameProc, const char *nameParam,
                const char *newValue, const rosAFE_ids *ids,
                genom_context self)
{
  if ( ids->inputProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
	if ( ids->inputProcessorsSt->processorsAccessor.getProcessor ( nameProc )->modifyParameter( nameParam, newValue) )
		return genom_ok;
	else
		return rosAFE_e_noSuchParameter(self);
  }
  if ( ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
	if (ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->modifyParameter( nameParam, newValue ))
		return genom_ok;
	else
		return rosAFE_e_noSuchParameter(self);
  }
  if ( ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
	if (ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc )->modifyParameter( nameParam, newValue ))
		return genom_ok;
	else
		return rosAFE_e_noSuchParameter(self);
  }
  if ( ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
	if (ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc )->modifyParameter( nameParam, newValue ))
		return genom_ok;
	else
		return rosAFE_e_noSuchParameter(self);
  }
  if ( ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc ) ) {
	if (ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc )->modifyParameter( nameParam, newValue ))
		return genom_ok;
	else
		return rosAFE_e_noSuchParameter(self);
  }
  
  return rosAFE_e_noSuchProcessor( self );
}


/* --- Function RemoveProcessor ----------------------------------------- */

/** Codel removeProcessor of function RemoveProcessor.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noSuchProcessor.
 */
genom_event
removeProcessor(const char *name, rosAFE_flagMap **flagMapSt,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{
  SM::removeFlag( name, newDataMapSt, self );
  if ( SM::removeFlag( name, flagMapSt, self ) )
	return genom_ok;
  else
	return rosAFE_e_noSuchProcessor( self );
}
