#include "acrosAFE.h"

#include "rosAFE_c_types.h"
#include "genom3_dataFiles.hpp"

/* --- Function RemoveDependencie --------------------------------------- */

/** Codel removeDependencie of function RemoveDependencie.
 *
 * Returns genom_ok.
 */
genom_event
removeDependencie(char **name, rosAFE_flagMap **flagMapSt,
                  rosAFE_flagMap **newDataMapSt, genom_context self)
{
  // Transformation to string
  std::string nameS = *name;

  for ( flagStIterator it = (*flagMapSt)->allFlags.begin() ; it != (*flagMapSt)->allFlags.end() ; ++it)
	 if  ((*it)->upperDep == nameS )
		(*flagMapSt)->allFlags.erase( it );

  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
	 if ((*it)->upperDep == nameS )
		(*newDataMapSt)->allFlags.erase( it );
				  
  return genom_ok;
}
