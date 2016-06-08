#ifndef GENOM3_DATAFILES_HPP
#define GENOM3_DATAFILES_HPP

#include <string>
#include <memory>
#include <vector>

#include "src/Processors/inputProc.hpp"
#include "src/Processors/preProc.hpp"
#include "src/Processors/gammatoneProc.hpp"
#include "src/Processors/ihcProc.hpp"
#include "src/Processors/ildProc.hpp"

#include "src/Processors/ProcessorVector.hpp"

using namespace openAFE;

using twoCTypeBlockPtr = typename twoCTypeBlock<double>::twoCTypeBlockPtr;

struct rosAFE_inputProcessors {
	ProcessorVector< InputProc > processorsAccessor; 
};

struct rosAFE_preProcessors {
	ProcessorVector< PreProc > processorsAccessor; 
};

struct rosAFE_gammatoneProcessors {
	ProcessorVector< GammatoneProc > processorsAccessor; 
};

struct rosAFE_ihcProcessors {
	ProcessorVector< IHCProc > processorsAccessor; 
};

struct rosAFE_ildProcessors {
	ProcessorVector< ILDProc > processorsAccessor; 
};

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
