#include "bwCoef.hpp"
#include "bwIIR.hpp"
#include "bwFilter.hpp"

using T = float;
using bwFilterPtr = openAFE::bwFilter<T>::bwFilterPtr;

int main() {
	
	bwFilterPtr newFilter;
	
	newFilter.reset( new openAFE::bwFilter<T> (44100, 4, 5000, (openAFE::bwType)1) );

	std::vector<T> a(12, 10);

	for ( unsigned int i = 0 ; i < a.size() ; i++ )
		std::cout << a[i] << " ";
	std::cout << std::endl;
		
	newFilter->filterChunk( a.data(), a.data() + a.size(), a.data() );
	
	for ( unsigned int i = 0 ; i < a.size() ; i++ )
		std::cout << a[i] << " ";
	std::cout << std::endl;
		
		
	return 0;
}
