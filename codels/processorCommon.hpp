#ifndef PROCESSORCOMMON_HPP
#define PROCESSORCOMMON_HPP

#include "acrosAFE.h"

#include "rosAFE_c_types.h"

#include "genom3_dataFiles.hpp"

#include <string>

namespace PC {
	
	genom_event
	execAnyProc( const char *nameProc, rosAFE_ids *ids, genom_context self );

}; /* namespace PC */

#endif /* PROCESSORCOMMON_HPP */
