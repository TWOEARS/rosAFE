#ifndef GENOM3_DATAFILES_HPP
#define GENOM3_DATAFILES_HPP

#include "includeAPIFiles.hpp"

using namespace openAFE;

/* Start : The Input processor template definitions */
using inputT = float_t;
using inputProcessor = InputProc<inputT>;
using inputProcessorVector = ProcessorVector<inputProcessor>;

using inputProcAccessorVector = typename inputProcessor::outT_nTwoCTypeBlockAccessorPtrVector;
using inputProcAccessorVectorIter = typename inputProcessor::outT_AccessorIter;


using inputProcPtr = typename inputProcessorVector::processorSharedPtr;

struct rosAFE_inputProcessors {
	inputProcessorVector processorsAccessor; 
};

/* Start : The pre processor template definitions */
using preT = inputT;
using preProcessor = PreProc<preT>;
using preProcessorVector = ProcessorVector<preProcessor>;

using preProcPtr = typename preProcessorVector::processorSharedPtr;

struct rosAFE_preProcessors {
	preProcessorVector processorsAccessor; 
};
/* End : The pre processor template definitions */


/* Start : The Gammatone template definitions */
using gammatoneT = preT;
using gammatoneProcessor = GamamtoneProc<gammatoneT>;
using gammatoneProcVector = ProcessorVector<gammatoneProcessor>;

using gammatoneProcPtr = typename gammatoneProcVector::processorSharedPtr;

struct rosAFE_gammatoneProcessors {
	gammatoneProcVector processorsAccessor; 
};
/* End : The Gammatone template definitions */

struct flagSt {
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
