#include "stateMachine.hpp"

/* --- Function RemovePependencie --------------------------------------- */

/** Codel removePependencie of function RemovePependencie.
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
