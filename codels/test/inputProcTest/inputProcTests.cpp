#include "../testUtilities.hpp"

int main() {
	
	const uint64_t fsIn = 7;
	const uint64_t fsOut = 7;
	const uint64_t bufferSize_s = 2;
	apf::parameter_map params;

	int chunkSize = 5;
	uint64_t cmpL = 0, cmpR = 0;
	std::vector< T> leftChunk(chunkSize);
	std::vector< T> rightChunk(chunkSize);
	
	InputProcPtr inputProcessor (new InputProc<T>("input", fsIn, fsOut, bufferSize_s) );

	std::cout << "Go!" << std::endl;	
    char key = 'a';
    while (key != 'q') {

		key = std::cin.get();
				
		/* Get a new chunk, process it and the append. Finally see the whole buffer */
		if ( key == 'w' ) {
			
			test::updateChunk(leftChunk.data(), leftChunk.size(), cmpL);
			test::updateChunk(rightChunk.data(), rightChunk.size(), cmpR);
			
			inputProcessor->processChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );
			inputProcessor->appendChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );
			
			inputProcessor->calcWholeBuffer();
			test::printNTwoCTypeBlock( (inputProcessor->getWholeBufferAccesor()) , "The whole buffer : ");
		}
		
		/* Show the last chunk */					
		if ( key == 'x' ) {
			inputProcessor->calcLastChunk();
			test::printNTwoCTypeBlock( inputProcessor->getLastChunkAccesor(), "The last chunk : ");
		}		

		/* Show the last 7 samples */
		if ( key == 'c' ) {
			inputProcessor->calcLastData( 7 );
			test::printNTwoCTypeBlock( inputProcessor->getLastDataAccesor(), "The latest 7 samples : " );
		}
		
		/* Show the 7 oldest, non seen samples. */		
		if ( key == 'v' ) {
			inputProcessor->calcOldData( 7 );
			test::printNTwoCTypeBlock( inputProcessor->getOldDataAccesor(), "The oldest 7 samples : " );
		}
		
		/* Show the all oldest, non seen samples. */				
		if ( key == 'b' ) {	
			inputProcessor->calcOldData( );
			test::printNTwoCTypeBlock( inputProcessor->getOldDataAccesor(), "The whole old data : " );
		}

		/* Print the fresh data size. */		
		if ( key == 's' ) {
			std::cout << "Fresh data size " << inputProcessor->getFreshDataSize() << std::endl;
		}
		
		/* Print Name etc. */
		if ( key == 'd' ) { 
			inputProcessor->printSignals();	
		}
				
	} // While

    return 0;
}
