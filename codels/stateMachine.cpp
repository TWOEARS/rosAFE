#include "stateMachine.hpp"

	void SM::addFlag(char **name, char **upperDep, rosAFE_flagMap **flagMapSt, genom_context& self ) {

	  flagStPtr flag ( new flagSt() );
	  flag->upperDep = (*upperDep);
	  flag->lowerDep = (*name);
	  flag->waitFlag = true;
	  
	  // And we store that flag into the flagMap
	  (*flagMapSt)->allFlags.push_back( std::move ( flag ) );
	}
	
	unsigned int SM::checkFlag ( char **name, char **upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {

    std::string nameS = *name;
    std::string fatherS = *upperDep;
  		
	for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( ( (*it)->upperDep == fatherS ) and ( (*it)->lowerDep ==  nameS ) )
			 if ( (*it)->waitFlag == false )
					return 0;
			 else return 1;
	return -1;
	}
	
	bool SM::checkFlag ( char **name, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {

    std::string nameS = *name;
      		
	for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( (*it)->upperDep ==  nameS )
			 if ( (*it)->waitFlag == true )
					return false;
	return true;
	}	

	void SM::fallFlag ( char **name, char **upperDep, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {
		
      std::string nameS = *name;
      std::string fatherS = *upperDep;
    
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( ( (*it)->upperDep == fatherS ) and ( (*it)->lowerDep ==  nameS ) )
			 (*it)->waitFlag = false;	 		
	}

	void SM::riseFlag ( char **name, rosAFE_flagMap **newDataMapSt, genom_context& self  ) {
		
      std::string nameS = *name;
          
	  for ( flagStIterator it = (*newDataMapSt)->allFlags.begin() ; it != (*newDataMapSt)->allFlags.end() ; ++it)
		if ( (*it)->upperDep ==  nameS ) 
			 (*it)->waitFlag = true;	 		
	}
/*
	void SM::deleteFlags ( char **name, rosAFE_flagMap **flagMapSt, genom_context& self  ) {
	
		  // Transformation to string
		  std::string nameS = *name;

		  for ( flagStIterator it = (*flagMapSt)->allFlags.begin() ; it != (*flagMapSt)->allFlags.end() ; ++it)
			 if ((*it)->upperDep == nameS )
					((*it)->lowerDep == lowerDepS ) )
				(*flagMapSt)->allFlags.erase( it );
		  
	  // return genom_ok;
	}*/	
