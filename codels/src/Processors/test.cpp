#include "inputProc.hpp"
#include "preProc.hpp"
#include "gammatoneProc.hpp"

using namespace openAFE;
	
int main() {
	int ii = 0;
	char c = 'a';
	std::vector <float> left(10), right(10);
	
	uint32_t fsIn = 20;
	uint32_t fsOut = 20;
	uint32_t bufferSize_s = 1;
	
	std::shared_ptr < InputProc > inProc( new InputProc( "P1", 20, bufferSize_s ) );	
	std::shared_ptr < PreProc > pProc( new PreProc( "P1", fsOut, bufferSize_s, inProc, false, 20, false, 0.97, false, 0.5, false, 100, false, "jepsen", true ) );
	std::shared_ptr < GammatoneProc > gttProc( new GammatoneProc( "GTT1", fsIn, fsOut, bufferSize_s, pProc, _gammatoneFilterBank, 80, 8000, 1, 0, nullptr, 0, 4, 1.018 ) );
	
	while ( c != 'q' ) {

		for ( unsigned int i = 0 ; i < left.size() ; i++ ) {
				left[i] = i * ii; right[i] = i*ii;
		}
		
		inProc->processChunk( left.data(), left.size(), right.data(), right.size() );
		inProc->releaseChunk( );
		pProc->processChunk( );
		pProc->releaseChunk( );
		gttProc->processChunk( );
		gttProc->releaseChunk( );
		
		std::vector<std::shared_ptr< twoCTypeBlock<float> > > info = gttProc->getLeftLastChunkAccessor();
	
		for ( size_t iii = 0 ; iii < info.size() ; ++iii) {
			std::cout << "Channel : " << iii <<  std::endl;	
			for ( unsigned int i = 0 ; i < info[iii]->array1.second ; i++ ) {
				std::cout << *( info[0]->array1.first + i ) << std::endl;
			}
			for ( unsigned int i = 0 ; i < info[iii]->array2.second ; i++ ) {
				std::cout << *( info[0]->array2.first + i ) << std::endl;
			}		
		}
		std::cout << ii++ << " : ------------------------" << std::endl;
	
		c = getchar();	
	}

	return 0;
}
