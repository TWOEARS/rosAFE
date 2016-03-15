#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include "acrosAFE.h"

#include "rosAFE_c_types.h"

#include "genom3_dataFiles.hpp"

#include <string>

namespace SM {
	
	void addFlag(char **name, char **upperDep, rosAFE_flagMap **flagMapSt, genom_context& self );
	
	unsigned int checkFlag ( char **name, char **upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  );
	
	bool checkFlag ( char **name, rosAFE_flagMap **newDataMapSt, genom_context& self  );

	void fallFlag ( char **name, char **upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  );

	void riseFlag ( char **name, rosAFE_flagMap **newDataMapSt, genom_context& self  );

}; /* namespace SM */

#endif /* STATEMACHINE_HPP */
