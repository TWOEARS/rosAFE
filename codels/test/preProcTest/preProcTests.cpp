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
	
	PreProcPtr inputProcessor (new PreProc<T>("input", fsIn, fsOut, bufferSize_s, params) );

	std::cout << "Go!" << std::endl;	
    char key = 'a';
    while (key != 'q') {

		key = std::cin.get();
				
		key = 'q';
				
	} // While

    return 0;
}
