#include "../testUtilities.hpp"

int main()
{

	uint64_t cmp = 0;
    unsigned int chunkSize = 4;
	std::vector< T> inChunk(chunkSize);
	char key = 'a';

	containerPtr bufWithArg (new CircularContainer<T>( 100000 ) );
	
	std::cout << "Go!" << std::endl;
	
    while (key != 'q') {
		
		key = std::cin.get();
		
		/* Append a new chunk and show the whole buffer */
		if ( key == 'w' ) {
			
			test::updateChunk(inChunk.data(), inChunk.size(), cmp);
			bufWithArg->push_chunk( inChunk.data(), inChunk.size() );
			
			bufWithArg->calcWholeBuffer();
			test::printTwoCTypeBlock( bufWithArg->getWholeBufferAccesor( ), "The whole buffer : ");
		}

		/* Show the last chunk */
		if ( key == 'x' ) {
			bufWithArg->calcLastChunk();
			test::printTwoCTypeBlock( bufWithArg->getLastChunkAccesor( ), "The last chunk : ");		
		}

		/* Show the last 7 samples */		
		if ( key == 'c' ) {
			bufWithArg->calcLatestData( 7 );
			test::printTwoCTypeBlock( bufWithArg->getLastDataAccesor( ), "The 7 lastest data : ");				
		}

		/* Show the 7 oldest, non seen samples */		
		if ( key == 'v' ) {
			bufWithArg->calcOldData( 7 );
			test::printTwoCTypeBlock( bufWithArg->getOldDataAccesor( ), "The 7 old data : ");		
		}

		/* Show the all oldest, non seen samples */				
		if ( key == 'b' ) {
			bufWithArg->calcOldData( );
			test::printTwoCTypeBlock( bufWithArg->getOldDataAccesor( ), "All the old data : ");		
		}

		/* Append again the last chunk */	
		if ( key == 'n' ) {
			bufWithArg->calcLastChunk();
			bufWithArg->push_chunk( bufWithArg->getLastChunkAccesor( ) );
			
			bufWithArg->calcWholeBuffer();
			test::printTwoCTypeBlock( bufWithArg->getWholeBufferAccesor( ), "The whole buffer : ");
		}		
			
		if ( key == 's' ) {
			std::cout << "Fresh data size " << bufWithArg->getFreshDataSize() << std::endl;
		}
						 
	}

    return 0;
}
