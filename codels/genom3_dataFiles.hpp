#ifndef GENOM3_DATAFILES_HPP
#define GENOM3_DATAFILES_HPP

#include "includeAPIFiles.hpp"

using namespace openAFE;

/* Start : The Input processor template definitions */
using inputT = float;
using inputProcessor = InputProc<inputT>;
using inputProcessorVector = ProcessorVector<inputProcessor>;

using inputProcAccessorVector = typename inputProcessor::outT_nTwoCTypeBlockAccessorPtrVector;
using inputProcAccessorVectorIter = typename inputProcessor::outT_AccessorIter;


using inputProcPtr = typename inputProcessorVector::processorSharedPtr;

struct rosAFE_inputProcessors {
	inputProcessorVector processorsAccessor; 
};
/* End : The Input processor template definitions */

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

/* Start : The IHC template definitions */
using ihcT = gammatoneT;
using ihcProcessor = IHCProc<ihcT>;
using ihcProcVector = ProcessorVector<ihcProcessor>;

using ihcProcPtr = typename ihcProcVector::processorSharedPtr;

struct rosAFE_ihcProcessors {
	ihcProcVector processorsAccessor; 
};
/* End : The IHC template definitions */

/* Start : The ILD template definitions */
using ildT = ihcT;
using ildProcessor = ILDProc<ildT>;
using ildProcVector = ProcessorVector<ildProcessor>;

using ildProcPtr = typename ildProcVector::processorSharedPtr;

struct rosAFE_ildProcessors {
	ildProcVector processorsAccessor; 
};
/* End : The ILD template definitions */

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
