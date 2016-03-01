#include "../testUtilities.hpp"

int main() {
	
	const uint64_t fsIn = 7;
	const uint64_t fsOut = 7;
	const uint64_t bufferSize_s = 2;

	int chunkSize = 5;
	uint64_t cmpL = 0, cmpR = 0;
	std::vector< T> leftChunk(chunkSize);
	std::vector< T> rightChunk(chunkSize);
	
	ProcessorVector<InputProc<T> > procVector;
	InputProcPtr inputProcessor (new InputProc<T>("input", fsIn, fsOut, bufferSize_s) );

	procVector.addProcessor( inputProcessor );
	
	std::cout << "Go!" << std::endl;	
    char key = 'a';
    while (key != 'q') {

		key = std::cin.get();
				
		/* Get a new chunk, process it and the append. Finally see the whole buffer */
		if ( key == 'w' ) {
			
			test::updateChunk(leftChunk.data(), leftChunk.size(), cmpL);
			test::updateChunk(rightChunk.data(), rightChunk.size(), cmpR);
			
			procVector.getProcessor("input")->processChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );
			procVector.getProcessor("input")->appendChunk( leftChunk.data(), leftChunk.size(), rightChunk.data(), rightChunk.size() );
			
			procVector.getProcessor("input")->calcWholeBuffer();
			test::printNTwoCTypeBlock( (procVector.getProcessor("input")->getWholeBufferAccesor()) , "The whole buffer : ");
		}
		
		/* Show the last chunk */					
		if ( key == 'x' ) {
			procVector.getProcessor("input")->calcLastChunk();
			test::printNTwoCTypeBlock( procVector.getProcessor("input")->getLastChunkAccesor(), "The last chunk : ");
		}		

		/* Show the last 7 samples */
		if ( key == 'c' ) {
			procVector.getProcessor("input")->calcLastData( 7 );
			test::printNTwoCTypeBlock( procVector.getProcessor("input")->getLastDataAccesor(), "The latest 7 samples : " );
		}
		
		/* Show the 7 oldest, non seen samples. */		
		if ( key == 'v' ) {
			procVector.getProcessor("input")->calcOldData( 7 );
			test::printNTwoCTypeBlock( procVector.getProcessor("input")->getOldDataAccesor(), "The oldest 7 samples : " );
		}
		
		/* Show the all oldest, non seen samples. */				
		if ( key == 'b' ) {	
			procVector.getProcessor("input")->calcOldData( );
			test::printNTwoCTypeBlock( procVector.getProcessor("input")->getOldDataAccesor(), "The whole old data : " );
		}

		/* Print the fresh data size. */		
		if ( key == 's' ) {
			std::cout << "Fresh data size " << inputProcessor->getFreshDataSize() << std::endl;
		}
		
		/* Print Name etc. */
		if ( key == 'd' ) { 
			procVector.getProcessor("input")->printSignals();	
		}
				
	} // While

    return 0;
}
