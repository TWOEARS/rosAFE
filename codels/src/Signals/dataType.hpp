#ifndef DATATYPE_HPP
#define DATATYPE_HPP

#include <vector>
#include <memory>
#include <assert.h>
#include <iostream>
/*
 * This file contains the pre-defined macros for the openAFE.
 */
 
namespace openAFE {
	// Alias definitions
	typedef std::vector<unsigned int> uintVector;
	typedef std::vector<uint32_t> uint32Vector;
	typedef std::vector<double> doubleVector;
	
	
	typedef std::vector<std::string> stringVector;
	typedef std::shared_ptr<std::string> stringP;
			
	/* twoCTypeBlock : This is needed to save the
	 * first and the second parts of the last chunk from a 
	 * circular buffer
	 * */
	template<typename T>
	struct twoCTypeBlock {		
		public :
			typedef std::shared_ptr<twoCTypeBlock<T> > twoCTypeBlockPtr;
			std::pair<T*, size_t> array1, array2;

			uint32_t getSize () {
				return array1.second + array2.second;
			}
			
			void setData( std::shared_ptr<twoCTypeBlock<T> > toCopy) {
				this->array1.first = toCopy->array1.first;
				this->array1.second = toCopy->array1.second;				
				this->array2.first = toCopy->array2.first;
				this->array2.second = toCopy->array2.second;
			}
	};
};

#endif /* DATATYPE_HPP */
