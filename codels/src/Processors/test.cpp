#include "inputProc.hpp"
#include "preProc.hpp"
#include "gammatoneProc.hpp"
#include "ihcProc.hpp"

#define LENGTH 10

using namespace openAFE;


	
int main() {
	int ii = 0;
	char c = 'a';
	std::vector <double> left(LENGTH), right(LENGTH);
	
	uint32_t fsIn = 16000;
	uint32_t fsOut = 16000;
	uint32_t bufferSize_s = 2;
	
	std::shared_ptr < InputProc > inProc( new InputProc( "inP1", 20, bufferSize_s, false ) );	
	std::shared_ptr < PreProc > pProc( new PreProc( "pp1", fsOut, bufferSize_s, inProc, false, 20, false, 0.97, false, 0.5, false, 100, false, "jepsen", true ) );
	std::shared_ptr < GammatoneProc > gttProc( new GammatoneProc( "fb1", fsIn, fsOut, bufferSize_s, pProc, _gammatoneFilterBank, 80, 8000, 1, 0, nullptr, 0, 4, 1.018 ) );
	std::shared_ptr < IHCProc > ihcP( new IHCProc( "ihc1", fsIn, fsOut, bufferSize_s, gttProc, _none ) );
		
		for ( unsigned int i = 0 ; i < LENGTH ; i++ ) {
			left[i] = i; right[i] = i;
		}
		
//	while ( c != 'q' ) {
	while ( ii < 1000 ) {
		
		inProc->processChunk( left.data(), left.size(), right.data(), right.size() );
		inProc->setNFR( ii );
		inProc->releaseChunk( );
		pProc->processChunk( );
		pProc->releaseChunk( );
		gttProc->processChunk( );
		gttProc->releaseChunk( );
		ihcP->processChunk( );
		ihcP->releaseChunk( );
				
		ii++;

/*		std::shared_ptr< twoCTypeBlock<double> > infopp = pProc->getLeftLastChunkAccessor();

			for ( unsigned int i = 0 ; i < infopp->array1.second ; i++ )
				std::cout << *( infopp->array1.first + i ) << " ";
			for ( unsigned int i = 0 ; i < infopp->array2.second ; i++ )
				std::cout << *( infopp->array2.first + i ) << " ";
			std::cout << std::endl;


		std::vector<std::shared_ptr< twoCTypeBlock<double> > > info = ihcP->getLeftLastChunkAccessor();
	
		for ( size_t iii = 0 ; iii < info.size() ; ++iii) {
			std::cout << "Left Channel : " << iii <<  std::endl;	
			for ( unsigned int i = 0 ; i < info[iii]->array1.second ; i++ )
				std::cout << *( info[iii]->array1.first + i ) << " ";
			for ( unsigned int i = 0 ; i < info[iii]->array2.second ; i++ )
				std::cout << *( info[iii]->array2.first + i ) << " ";
			std::cout << std::endl;
		}


		 c = getchar();	
*/	}

		std::vector<std::shared_ptr< twoCTypeBlock<double> > > info = ihcP->getLeftLastChunkAccessor();
	
		for ( size_t iii = 0 ; iii < info.size() ; ++iii) {
			std::cout << "Left Channel : " << iii <<  std::endl;	
			for ( unsigned int i = 0 ; i < info[iii]->array1.second ; i++ )
				std::cout << *( info[iii]->array1.first + i ) << " ";
			for ( unsigned int i = 0 ; i < info[iii]->array2.second ; i++ )
				std::cout << *( info[iii]->array2.first + i ) << " ";
			std::cout << std::endl;
		}
			
	return 0;
}
