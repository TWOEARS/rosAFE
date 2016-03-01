#ifndef GENOM3_DATAFILES_HPP
#define GENOM3_DATAFILES_HPP

#include "includeAPIFiles.hpp"

using namespace openAFE;

/* Start : The Input processor template definitions */
using inputT = double;
using inputProcessor = InputProc<inputT>;
using inputProcessorVector = ProcessorVector<inputProcessor>;

using inputProcPtr = typename inputProcessorVector::processorSharedPtr;
typedef std::shared_ptr< inputProcessorVector > inputProcessorVectorPtr;

struct rosAFE_inputProcessors {
	inputProcessorVectorPtr processorsAccessor; 
};

/* Start : The pre processor template definitions */
using preT = double;
using preProcessor = PreProc<inputT>;
using preProcessorVector = ProcessorVector<preProcessor>;

using preProcPtr = typename preProcessorVector::processorSharedPtr;
typedef std::shared_ptr< preProcessorVector > preProcessorVectorPtr;

struct rosAFE_preProcessors {
	preProcessorVectorPtr processorsAccessor; 
};
/* End : The pre processor template definitions */



struct flagSt
   {
	std::string upperDep;
	std::string lowerDep;
	bool waitFlag = false;	 
   };

typedef std::shared_ptr<flagSt > 			flagStPtr;
typedef std::vector<flagStPtr > 			flagStPtrVector;

typedef flagStPtrVector::iterator 			flagStIterator;
typedef flagStPtrVector::const_iterator 	flagStConstIterator;
   
struct rosAFE_flagMap
   {
	flagStPtrVector allFlags; 
   };

#endif /* GENOM3_DATAFILES_HPP */
