#ifndef TESTUTILITIES_HPP
#define TESTUTILITIES_HPP

#include "../includeAPIFiles.hpp"

using T = double;

using namespace openAFE;

using twoCTypeBlockPtr = typename twoCTypeBlock<T>::twoCTypeBlockPtr;
using nTwoCTypeBlockAccessorPtr = typename nTwoCTypeBlockAccessor<T>::nTwoCTypeBlockAccessorPtr;
using nTwoCTypeBlockAccessorPtrVector = typename nTwoCTypeBlockAccessor<T>::nTwoCTypeBlockAccessorPtrVector;

using containerPtr = typename CircularContainer<T>::containerPtr;
using TimeDomainSignalPtr = TimeDomainSignal<T>::signalSharedPtr;
using TimeFrequencySignalPtr = TimeFrequencySignal<T>::signalSharedPtr;
	

using InputProcPtr = typename InputProc<T>::processorSharedPtr;
using PreProcPtr = typename ProcessorVector<PreProc<T> >::processorSharedPtr;
using GammatoneProcPtr = typename ProcessorVector<GamamtoneProc<T> >::processorSharedPtr;


namespace test {
	
	void printChunk (T* start, uint32_t size) {
		
		if (size > 0 ) {
			for (uint32_t i = 0 ; i < size ; i++)
				std::cout << "(" << (*(start + i)) << "; " << &(*(start + i)) << ") ";
			std::cout << std::endl;
		}
	}

	void updateChunk (T* start, uint32_t size, uint32_t& cmp) {
		
		for (unsigned int j = 0 ; j < size ; ++j)
			*( start + j ) = j + cmp;		
		cmp += size;
	}

	void printTwoCTypeBlock (twoCTypeBlockPtr data, std::string info = ""){
			std::cout << info;
			printChunk(data->first->firstValue, data->first->dim);
			printChunk(data->second->firstValue, data->second->dim);
			std::cout << std::endl;			
	}
	
	void printNTwoCTypeBlock (nTwoCTypeBlockAccessorPtr data, std::string info = "" ){
			std::cout << info;
			for (unsigned int i = 0 ; i < data->getDimOfSignal() ; ++i)
				printTwoCTypeBlock( data->getTwoCTypeBlockAccessor(i) );
			std::cout << std::endl;
	}
	
	void printNTwoCTypeBlock (nTwoCTypeBlockAccessorPtrVector& data, std::string info){
			std::cout << info << std::endl;
			for (unsigned int i = 0 ; i < data.size() ; ++i)
				printNTwoCTypeBlock( data[i] );
			std::cout << std::endl;
	}

};
#endif /* TESTUTILITIES_HPP */
