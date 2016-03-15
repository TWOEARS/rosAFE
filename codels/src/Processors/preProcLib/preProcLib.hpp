#ifndef PREPROCLIB_HPP
#define PREPROCLIB_HPP


namespace openAFE {
	
	namespace preProcLib {

		template<typename T>
		void multiply( T* firstValue, uint64_t dim, uint64_t multiple ) {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue + i ) = *( firstValue + i ) * multiple;			
		}
		
	};
};



#endif /* PREPROCLIB_HPP */

