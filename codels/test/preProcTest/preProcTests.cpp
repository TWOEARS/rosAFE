#include "../testUtilities.hpp"

void printWholeBufers(InputProcPtr inputProcessor, PreProcPtr preProcessor) {
	inputProcessor->calcWholeBuffer();
	test::printNTwoCTypeBlock( (inputProcessor->getWholeBufferAccesor()) , "The whole buffer (input) : ");
			
	preProcessor->calcWholeBuffer();
	test::printNTwoCTypeBlock( (preProcessor->getWholeBufferAccesor()) , "The whole buffer (preProc) : ");
}

int main() {
	
	const uint64_t fsIn = 4;
	const uint64_t fsOut = 4;
	const uint64_t bufferSize_s = 2;
	apf::parameter_map params;

	int chunkSize = 5;
	uint64_t cmpL = 0, cmpR = 0;
	std::vector< T> leftChunk(chunkSize);
	std::vector< T> rightChunk(chunkSize);
	
	InputProcPtr inputProcessor (new InputProc<T>("inputTest", fsIn, fsOut, bufferSize_s) );

	PreProcPtr preProcessor (new PreProc<T>("preProcTest", fsIn, fsOut, bufferSize_s, params) );

	preProcessor->addInputProcessor ( inputProcessor );
    
	std::cout << "Go!" << std::endl;	
    char key = 'a';
    while (key != 'q') {

		key = std::cin.get();
		
		if ( key == 'w' ) {

			// std::cout << "FD before " << inputProcessor->getFreshDataSize() << std::endl;
			
			test::updateChunk(leftChunk.data(), leftChunk.size(), cmpL);
			test::updateChunk(rightChunk.data(), rightChunk.size(), cmpR);
			
			inputProcessor->processChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );
			inputProcessor->appendChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );

			// std::cout << "FD after " << inputProcessor->getFreshDataSize() << std::endl;
			
			printWholeBufers(inputProcessor, preProcessor);
		}
		
		/* exec : Process & append the whole fresh data of input to preProc */
		if ( key == 'x' ) {
			
			inputProcessor->calcOldData( );
			// test::printNTwoCTypeBlock( inputProcessor->getOldDataAccesor(), "The whole old data (input) : " );

			preProcessor->processChunk ();
			
			std::cout << "Processing" << std::endl;
		}

		/* release : append */		
		if ( key == 'c' ) {
			
			preProcessor->appendChunk ();
			printWholeBufers(inputProcessor, preProcessor);
		}
	
		if ( key == 'v' ) {
			std::cout << "Input : Fresh data size " << inputProcessor->getFreshDataSize() << std::endl;
			std::cout << "PreProc : Fresh data size " << preProcessor->getFreshDataSize() << std::endl;
		}

		/* Show the last chunks */					
		if ( key == 's' ) {
			inputProcessor->calcLastChunk();
			test::printNTwoCTypeBlock( inputProcessor->getLastChunkAccesor(), "The last chunk (input) : ");
			
			preProcessor->calcLastChunk();
			test::printNTwoCTypeBlock( preProcessor->getLastChunkAccesor(), "The last chunk (preProc) : ");
		}

		/* Show whole buffers */					
		if ( key == 'd' ) {
			
			printWholeBufers(inputProcessor, preProcessor);
		}
							
	} // While

    return 0;
}
