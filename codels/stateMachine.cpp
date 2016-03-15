#include "stateMachine.hpp"

	void SM::addFlag(const char *name, const char *upperDep, rosAFE_flagMap **flagMapSt, genom_context& self ) {

	  flagStPtr flag ( new flagSt() );
	  flag->upperDep = upperDep;
	  flag->lowerDep = name;
	  flag->waitFlag = true;
	  
	  // And we store that flag into the flagMap
	  (*flagMapSt)->allFlags.push_back( std::move ( flag ) );
	}
	
	int SM::checkFlag ( const char *name, const char *upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {

		for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it) {
			if ( ( (*it)->upperDep == upperDep ) and ( (*it)->lowerDep ==  name ) ) {
				 if ( (*it)->waitFlag == false ) {
						return 0;
				 } else return 1;
			}
		}
		std::cout << "Going to send 2 : " << name << std::endl;
		return 2;
	}
	
	bool SM::checkFlag ( const char *name, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {
      		
	for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( (*it)->upperDep ==  name )
			 if ( (*it)->waitFlag == true )
					return false;
	return true;
	}	

	void SM::fallFlag ( const char *name, const char *upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {
    
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( ( (*it)->upperDep == upperDep ) and ( (*it)->lowerDep ==  name ) )
			 (*it)->waitFlag = false;	 		
	}

	void SM::riseFlag ( const char *name, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {
		          
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( (*it)->upperDep ==  name ) 
			 (*it)->waitFlag = true;	 		
	}

    void SM::removeFlag ( const char *name, rosAFE_flagMap **newDataMapSt, genom_context& self ) {
		
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it) {
		 if ( (*it)->upperDep == name ) {
			 SM::removeFlag( ((*it)->lowerDep).c_str(), newDataMapSt, self);
		 }
	  }
		 
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it) {
		 if ( (*it)->lowerDep == name ) {
			 // (*newDataMapSt)->allFlags.erase( it );
			 std::cout << "Erased : " << name << std::endl;
		 }
	  }
	}
