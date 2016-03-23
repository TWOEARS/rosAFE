#ifndef PREPROCLIB_HPP
#define PREPROCLIB_HPP

namespace openAFE {
	
	namespace preProcLib {

		template<typename T>
		void multiply( T* firstValue, uint32_t dim, apfMap& map ) {
			uint32_t val = map.get<int>("bRemoveDC");
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue + i ) = *( firstValue + i ) * val;			
		}
		
	};
};



#endif /* PREPROCLIB_HPP */

