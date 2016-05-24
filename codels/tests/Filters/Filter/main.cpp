#include "Filter.hpp"
#include <memory>
#include <iostream>
#include <complex>

#define N 10

using namespace openAFE;
using T = std::complex<double>;
using T_ab = double;

int main () {
	
	std::shared_ptr<Filter<T, T_ab> > newFilter;
	std::vector<T_ab > a(2);
	a[0] = 1;
	a[1] = 5;
	
	std::vector<T_ab > b(3);
	b[0] = 1;
	b[1] = 4;
	b[2] = 9;

		
	std::vector<T> x(N);
	for ( unsigned int i = 0 ; i < x.size() ; ++i )
		x[i] = i;
					
	newFilter.reset( new Filter<T, T_ab>( b.data(), b.size(), a.data(), a.size() ) ); 
	
	newFilter->exec( x.data(), x.size(), x.data() );

	for ( unsigned int i = 0 ; i < x.size() ; ++i )
		std::cout << x[i] << " ";
	std::cout << std::endl; 
	
	
	return 0;
}
