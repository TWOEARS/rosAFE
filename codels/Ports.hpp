#ifndef PORTS_HPP
#define PORTS_HPP

#include "acrosAFE.h"

#include "rosAFE_c_types.h"

#include "genom3_dataFiles.hpp"

#include <string>

int initInputPort(const rosAFE_inputProcessorOutput *inputProcessorOutput, uint32_t sampleRate,
				  uint32_t bufferSize_s, uint32_t bytesPerFrame, genom_context self);
int publishInputPort (const rosAFE_inputProcessorOutput *inputProcessorOutput, inputProcAccessorVector data, genom_context self);

#endif /* PORTS_HPP */
