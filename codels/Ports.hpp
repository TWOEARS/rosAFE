#ifndef PORTS_HPP
#define PORTS_HPP

#include "acrosAFE.h"

#include "rosAFE_c_types.h"

#include "genom3_dataFiles.hpp"

#include <string>

namespace PORT {
	
	genom_event
	initInputPort ( const rosAFE_inputProcPort *inputProcPort, uint32_t sampleRate,
						uint32_t bufferSize_s, genom_context self );

	genom_event
	publishInputPort ( const rosAFE_inputProcPort *inputProcPort, inputProcAccessorVector data, uint32_t bytesPerFrame, int64_t nfr, genom_context self );

	genom_event
	initPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, uint32_t sampleRate,
						uint32_t bufferSize_s, genom_context self );

	genom_event
	publishPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, inputProcAccessorVector data, uint32_t bytesPerFrame, int64_t nfr, genom_context self );
													
	genom_event
	publishTDSPort (const char *name, const rosAFE_dataObj *dataObj, inputProcAccessorVector data, uint64_t frameIndex, uint32_t iii, genom_context self);

	/*
	genom_event
	deleteTDSPort (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self);
	*/
}
#endif /* PORTS_HPP */
