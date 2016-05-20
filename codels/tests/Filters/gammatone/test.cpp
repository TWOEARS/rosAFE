#include "gammatoneTools.hpp"
#include <iostream>
#include <complex>

using namespace openAFE;
using namespace GTT;
	
int main() {
	
	std::vector<std::complex<double> > a( 3,1 ); a[0].real( 1.5 ); a[0].imag( 3.14 );
	
	std::vector<std::complex<double> > b( 3,1 ); // b[0] = 1; b[1] = 1; b[2] = 2;	
	
	std::vector<std::complex<double> > out = conv<std::complex<double>> ( a.data(), b.data(), a.size(), b.size() );
	
	for ( unsigned int i = 0 ; i < out.size() ; i++ )
		std::cout << out[i] << " ";
	std::cout << std::endl;	
	
	return 0;
}
