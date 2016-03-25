#include "../../testUtilities.hpp"		
 		
 int main () {		
 			
 	uint32_t fs = 4, bufferSize_s = 2;		
	uint32_t cmp1 = 0, cmp2 = 0;

    unsigned int chunkSize = 4;
	std::vector< T> inChunk1(chunkSize);
	std::vector< T> inChunk2(chunkSize);

	twoCTypeBlockPtr inChunk1P (new twoCTypeBlock<T>() );
	inChunk1P->first->firstValue = inChunk1.data();
	inChunk1P->first->dim = inChunk1.size();

	twoCTypeBlockPtr inChunk2P (new twoCTypeBlock<T>() );
	inChunk2P->first->firstValue = inChunk2.data();
	inChunk2P->first->dim = inChunk2.size();
		
	nTwoCTypeBlockAccessorPtr inChunk (new nTwoCTypeBlockAccessor<T>(2) );
	inChunk->setData ( 0, inChunk1P );
	inChunk->setData ( 1, inChunk2P );
	 	
 	doubleVector cfHz(2,1); 			
    TimeFrequencySignalPtr leftSig ( new TimeFrequencySignal<T>(fs, bufferSize_s, cfHz) );
 
 	std::cout << "Go!" << std::endl;	
	char key = 'a';
    while (key != 'q') {

		key = std::cin.get();

		/* Append a new chunk and show the whole buffer */
		if ( key == 'w' ) {
			
			test::updateChunk(inChunk1.data(), inChunk1.size(), cmp1);
			test::updateChunk(inChunk2.data(), inChunk2.size(), cmp2);								
			
			leftSig->appendChunk(inChunk);
			leftSig->calcWholeBuffer();		
			test::printNTwoCTypeBlock( leftSig->getWholeBufferAccesor() , "The whole buffer : ");
		}
		
		/* Show the last chunk */					
		if ( key == 'x' ) {
			leftSig->calcLastChunk();
			test::printNTwoCTypeBlock( leftSig->getLastChunkAccesor(), "The last chunk : ");
		}		

		/* Show the last 7 samples */
		if ( key == 'c' ) {
			leftSig->calcLastData( 7 );
			test::printNTwoCTypeBlock( leftSig->getLastDataAccesor(), "The latest 7 samples : " );
		}
		
		/* Show the 7 oldest, non seen samples */		
		if ( key == 'v' ) {
			leftSig->calcOldData( 7 );
			test::printNTwoCTypeBlock( leftSig->getOldDataAccesor(), "The oldest 7 samples : " );
		}
		
		/* Show the all oldest, non seen samples */				
		if ( key == 'b' ) {	
			leftSig->calcOldData( );
			test::printNTwoCTypeBlock( leftSig->getOldDataAccesor(), "The whole old data : " );
		}
		
		/* Append the last chunk again and show the whole buffer */
		if ( key == 'n' ) {		

			leftSig->calcLastChunk();
			leftSig->appendChunk( leftSig->getLastChunkAccesor() );
			
			leftSig->calcWholeBuffer();		
			test::printNTwoCTypeBlock( leftSig->getWholeBufferAccesor() , "The whole buffer : ");
		}		

		if ( key == 's' ) {
			std::cout << "Fresh data size " << leftSig->getFreshDataSize() << std::endl;
		}
					
	}
 
 			
 	return 0;		
 }
