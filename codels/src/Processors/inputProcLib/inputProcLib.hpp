#ifndef INPUTPROCLIB_HPP
#define INPUTPROCLIB_HPP

#define MAXCODABLEVALUE 2147483647;

namespace openAFE {
	
	namespace inputProcLib {

		/* This function divades all the datas of the incoming chunk
		 * with the MAXCODABLEVALUE.
		 * */
		template<typename T>
		void normaliseData(T* firstValue, uint32_t dim) {
			for ( unsigned int i = 0 ; i < dim ; ++i )
				*( firstValue + i ) = *( firstValue + i ) / MAXCODABLEVALUE;
		}
		
	};
};



#endif /* INPUTPROCLIB_HPP */

