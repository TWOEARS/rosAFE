#include "../../testUtilities.hpp"


	const uint32_t fsIn = 7;
	const uint32_t fsOut = 7;
	const uint32_t bufferSize_s = 2;
	
void makeProcssors( ProcessorVector<InputProc<T> >& inputProcVector, ProcessorVector<PreProc<T> >& preProcVector, apf::parameter_map& params) {
	
	InputProcPtr inputProcessor (new InputProc<T>("input", fsIn, fsOut, bufferSize_s) );
	std::cout << inputProcessor.use_count() << std::endl;
	inputProcVector.addProcessor( inputProcessor );
	
/*	PreProcPtr preProcessor (new PreProc<T>("preProcTest", fsIn, fsOut, bufferSize_s, params) );
	preProcessor->addInputProcessor ( inputProcVector.getProcessor("input") );
	preProcVector.addProcessor( preProcessor );

	PreProcPtr preProcessor2 (new PreProc<T>("preProcTest2", fsIn, fsOut, bufferSize_s, params) );
	preProcessor2->addInputProcessor ( inputProcVector.getProcessor("input") );	
	preProcVector.addProcessor( preProcessor2 );*/
		std::cout << inputProcessor.use_count() << std::endl;	

	inputProcVector.clear();
	std::cout << inputProcessor.use_count() << std::endl;	
}


int main() {
	


	int chunkSize = 5;
	uint32_t cmpL = 0, cmpR = 0;
	std::vector< T> leftChunk(chunkSize);
	std::vector< T> rightChunk(chunkSize);
	apf::parameter_map params;
	params.set("bRemoveDC",1);
		
	ProcessorVector<InputProc<T> > inputProcVector;
	ProcessorVector<PreProc<T> > preProcVector;
		
	makeProcssors( inputProcVector, preProcVector, params );
	
	inputProcVector.clear();
	std::cout << inputProcVector.getProcessor("input").use_count() << std::endl;

	
		
    return 0;
}
