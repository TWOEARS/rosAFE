#ifndef GENOM3_DATAFILES_HPP
#define GENOM3_DATAFILES_HPP

#include "includeAPIFiles.hpp"

using namespace openAFE;

/* Define all templates here */

/* Start : The Input processor */
using inputT = double;
using inputProcessor = InputProc<inputT>;
typedef std::shared_ptr<inputProcessor > inputProcPtr;

using inputProcessorVector = ProcessorVector<inputProcessor>;
typedef std::shared_ptr< inputProcessorVector > inputProcessorVectorPtr;

struct rosAFE_inputProcessors {
	inputProcessorVectorPtr processorsAccessor; 
};
/* End : The Input processor */

struct rosAFE_flagMap {

};

struct rosAFE_newDataMap {

};

#endif /* GENOM3_DATAFILES_HPP */
