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
	publishInputPort ( const rosAFE_inputProcPort *inputProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self );

	genom_event
	initPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, uint32_t sampleRate,
						uint32_t bufferSize_s, genom_context self );

	genom_event
	publishPreProcPort ( const char *name, const rosAFE_preProcPort *preProcPort, twoCTypeBlockPtr left, twoCTypeBlockPtr right, uint32_t bytesPerFrame, int64_t nfr, genom_context self );

	genom_event
	initGammatonePort ( const char *name, const rosAFE_gammatonePort *gammatonePort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self );
						
	genom_event
	publishGammatonePort ( const char *name, const rosAFE_gammatonePort *gammatonePort, std::vector<twoCTypeBlockPtr > left,
						std::vector<twoCTypeBlockPtr > right, uint32_t bytesPerFrame, int64_t nfr, genom_context self );

	genom_event
	initIHCPort ( const char *name, const rosAFE_ihcPort *ihcPort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self );
						
	genom_event
	publishIHCPort ( const char *name, const rosAFE_ihcPort *ihcPort, std::vector<twoCTypeBlockPtr > left,
						std::vector<twoCTypeBlockPtr > right, uint32_t bytesPerFrame, int64_t nfr, genom_context self );

	genom_event
	initILDPort ( const char *name, const rosAFE_ildPort *ildPort, uint32_t sampleRate,
						uint32_t bufferSize_s, uint32_t nChannels, genom_context self );
						
	genom_event
	publishILDPort ( const char *name, const rosAFE_ildPort *ildPort, std::vector<twoCTypeBlockPtr > left,
						uint32_t bytesPerFrame, int64_t nfr, genom_context self );
																		
	/*
	genom_event
	deleteTDSPort (const char *name, const rosAFE_TDSPorts *TDSPorts, genom_context self);
	*/
}
#endif /* PORTS_HPP */
