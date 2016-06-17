#include "acrosAFE.h"
#include "rosAFE_c_types.h"

#include <pthread.h>

#include <memory>

#include "stateMachine.hpp"
#include "processorCommon.hpp"
#include "genom3_dataFiles.hpp"
#include "Ports.hpp"

#include <apf/parameter_map.h>

openAFE::procType findType ( const char *name, const rosAFE_ids *ids )
{
	if ( (ids->inputProcessorsSt->processorsAccessor).existsProcessorName ( name ) )
		return _inputProc;
	else if ( (ids->preProcessorsSt->processorsAccessor).existsProcessorName ( name ) )
		return _preProc;
	else if ( (ids->gammatoneProcessorsSt->processorsAccessor).existsProcessorName ( name ) )
		return _gammatone;
	else if ( (ids->ihcProcessorsSt->processorsAccessor).existsProcessorName ( name ) )
		return _ihc;
	else if ( (ids->ildProcessorsSt->processorsAccessor).existsProcessorName ( name ) )
	    return _ild;
	else return _unknow;
}

bool checkExists ( const char *name, const rosAFE_ids *ids )
{
	if ( findType(name, ids) == _unknow )
	   return false;
	else return true;
}

/* --- Activity PreProc ------------------------------------------------- */

/** Validation codel existsAlready of activity PreProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
genom_event
existsAlready(const char *name, const char *upperDepName,
              const rosAFE_ids *ids, genom_context self)
{
  if ( ! checkExists ( upperDepName, ids ) ) {
	return rosAFE_e_noUpperDependencie( self );
  } else if ( checkExists ( name, ids ) ) {
	return rosAFE_e_existsAlready( self );
  } else return genom_ok;
}


/* --- Activity GammatoneProc ------------------------------------------- */

/** Validation codel existsAlready of activity GammatoneProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Activity IhcProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IhcProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Activity IldProc ------------------------------------------------- */

/** Validation codel existsAlready of activity IldProc.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noUpperDependencie, rosAFE_e_existsAlready,
 * rosAFE_e_noSuchProcessor.
 */
/* already defined in service PreProc validation */



/* --- Function getSignals ---------------------------------------------- */

/** Codel getSignal of function getSignals.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory, rosAFE_e_badIndexParam.
 */
genom_event
getSignal(rosAFE_dataObjSt *signals, const rosAFE_ids *ids,
          genom_context self)
{
/* ****************************  Input START  ************************************ */
  size_t sizeInputProc = ids->inputProcessorsSt->processorsAccessor.getSize();

  signals->input._length = sizeInputProc;

  if (genom_sequence_reserve(&(signals->input), sizeInputProc))
	return rosAFE_e_noMemory( self );

  for ( size_t ii = 0 ; ii < sizeInputProc ; ++ii ) {
    std::shared_ptr < InputProc > thisProcessor = ids->inputProcessorsSt->processorsAccessor.getProcessor ( ii );
    
	twoCTypeBlockPtr left = thisProcessor->getLeftOldDataAccessor();
	twoCTypeBlockPtr right = thisProcessor->getRightOldDataAccessor();

	uint32_t dim1 = left->array1.second;
	uint32_t dim2 = left->array2.second;

 	uint32_t fpc = dim1 + dim2; // amount of Frames On this Chunk
 	 	 	 	 	 	 	
	signals->input._buffer[ii].sampleRate = thisProcessor->getFsOut();
	signals->input._buffer[ii].framesOnPort = fpc;
	signals->input._buffer[ii].lastFrameIndex = thisProcessor->getNFR();

    PORT::iniTDS_port ( &(signals->input._buffer[ii].left.data), fpc, false, self );
    PORT::iniTDS_port ( &(signals->input._buffer[ii].right.data), fpc, false, self );
        
	PORT::publishTDS_port ( &(signals->input._buffer[ii].left.data), left, fpc, 0, self );
	PORT::publishTDS_port ( &(signals->input._buffer[ii].right.data), right, fpc, 0, self );
	
  }

/* ****************************  PreProc START  ************************************ */
  size_t sizePreProc = ids->preProcessorsSt->processorsAccessor.getSize();

  signals->preProc._length = sizePreProc;

  if (genom_sequence_reserve(&(signals->preProc), sizePreProc))
	return rosAFE_e_noMemory( self );

  for ( size_t ii = 0 ; ii < sizePreProc ; ++ii ) {
    std::shared_ptr < PreProc > thisProcessor = ids->preProcessorsSt->processorsAccessor.getProcessor ( ii );
 
	twoCTypeBlockPtr left = thisProcessor->getLeftOldDataAccessor();
	twoCTypeBlockPtr right = thisProcessor->getRightOldDataAccessor();

	uint32_t dim1 = left->array1.second;
	uint32_t dim2 = left->array2.second;

 	uint32_t fpc = dim1 + dim2; // amount of Frames On this Chunk
 	
	signals->preProc._buffer[ii].sampleRate = thisProcessor->getFsOut();
	signals->preProc._buffer[ii].framesOnPort = fpc;
	signals->preProc._buffer[ii].lastFrameIndex = thisProcessor->getNFR();

    PORT::iniTDS_port ( &(signals->preProc._buffer[ii].left.data), fpc, false, self );
    PORT::iniTDS_port ( &(signals->preProc._buffer[ii].right.data), fpc, false, self );
        
	PORT::publishTDS_port ( &(signals->preProc._buffer[ii].left.data), left, fpc, 0, self );
	PORT::publishTDS_port ( &(signals->preProc._buffer[ii].right.data), right, fpc, 0, self );
												  
	left.reset();
	right.reset();
    thisProcessor.reset();
  }

/* ****************************  Gammatone START  ************************************ */
  size_t sizeGammatone = ids->gammatoneProcessorsSt->processorsAccessor.getSize();
  
  signals->gammatone._length = sizeGammatone;
  if (genom_sequence_reserve(&(signals->gammatone), sizeGammatone))
	return rosAFE_e_noMemory( self );
	
  for ( size_t ii = 0 ; ii < sizeGammatone ; ++ii ) {
	  std::shared_ptr < GammatoneProc > thisProcessor = ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( ii );
	
	  std::vector< twoCTypeBlockPtr > left = thisProcessor->getLeftOldDataAccessor();
	  std::vector< twoCTypeBlockPtr > right = thisProcessor->getRightOldDataAccessor();

	  uint32_t dim1 = left[0]->array1.second;
	  uint32_t dim2 = left[0]->array2.second;
			
	  uint32_t fpc = dim1 + dim2; 			// amount of Frames On this Chunk
	  uint32_t nChannels = thisProcessor->get_fb_nChannels();
	  		
	  PORT::iniTFS_port ( &(signals->gammatone._buffer[ii].left.dataN), nChannels,  fpc, false, self );
	  PORT::iniTFS_port ( &(signals->gammatone._buffer[ii].right.dataN), nChannels,  fpc, false, self );
	  
	  PORT::publishTFS_port ( &(signals->gammatone._buffer[ii].left.dataN), left, nChannels, fpc, 0, self );
	  PORT::publishTFS_port ( &(signals->gammatone._buffer[ii].right.dataN), right, nChannels, fpc, 0, self );
	  
	  signals->gammatone._buffer[ii].framesOnPort = fpc;	  
	  signals->gammatone._buffer[ii].sampleRate = thisProcessor->getFsOut();
	  signals->gammatone._buffer[ii].numberOfChannels = nChannels;
	  signals->gammatone._buffer[ii].lastFrameIndex = thisProcessor->getNFR();	  
								  
  }

/* ****************************  IHC START  ************************************ */
  size_t sizeIhcProc = ids->ihcProcessorsSt->processorsAccessor.getSize();
  signals->ihc._length = sizeIhcProc;
  if (genom_sequence_reserve(&(signals->ihc), sizeIhcProc))
	return rosAFE_e_noMemory( self );
	
  for ( size_t ii = 0 ; ii < sizeIhcProc ; ++ii ) {
      std::shared_ptr < IHCProc > thisProcessor = ids->ihcProcessorsSt->processorsAccessor.getProcessor ( ii );
	
	  std::vector< twoCTypeBlockPtr > left = thisProcessor->getLeftOldDataAccessor();
	  std::vector< twoCTypeBlockPtr > right = thisProcessor->getRightOldDataAccessor();

	  uint32_t dim1 = left[0]->array1.second;
	  uint32_t dim2 = left[0]->array2.second;
			
	  uint32_t fpc = dim1 + dim2; 			// amount of Frames On this Chunk
	  uint32_t nChannels = thisProcessor->get_ihc_nChannels();
	  		
	  PORT::iniTFS_port ( &(signals->ihc._buffer[ii].left.dataN), nChannels,  fpc, false, self );
	  PORT::iniTFS_port ( &(signals->ihc._buffer[ii].right.dataN), nChannels,  fpc, false, self );
	  
	  PORT::publishTFS_port ( &(signals->ihc._buffer[ii].left.dataN), left, nChannels, fpc, 0, self );
	  PORT::publishTFS_port ( &(signals->ihc._buffer[ii].right.dataN), right, nChannels, fpc, 0, self );
	  
	  signals->ihc._buffer[ii].framesOnPort = fpc;	  
	  signals->ihc._buffer[ii].sampleRate = thisProcessor->getFsOut();
	  signals->ihc._buffer[ii].numberOfChannels = nChannels;
	  signals->ihc._buffer[ii].lastFrameIndex = thisProcessor->getNFR();
	  
  }	    

/* ****************************  ILD START  ************************************ */
  size_t sizeIldProc = ids->ildProcessorsSt->processorsAccessor.getSize();
  signals->ild._length = sizeIldProc;
  if (genom_sequence_reserve(&(signals->ild), sizeIldProc))
	return rosAFE_e_noMemory( self );
	 
  for ( size_t ii = 0 ; ii < sizeIldProc ; ++ii ) {
      std::shared_ptr < ILDProc > thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( ii );
	
	  std::vector< twoCTypeBlockPtr > left = thisProcessor->getLeftOldDataAccessor();

	  uint32_t dim1 = left[0]->array1.second;
	  uint32_t dim2 = left[0]->array2.second;
			
	  uint32_t fpc = dim1 + dim2; 			// amount of Frames On this Chunk
	  uint32_t nChannels = thisProcessor->get_ild_nChannels();
	  		
	  PORT::iniTFS_port ( &(signals->ild._buffer[ii].left.dataN), nChannels,  fpc, false, self );
	  
	  PORT::publishTFS_port ( &(signals->ild._buffer[ii].left.dataN), left, nChannels, fpc, 0, self );
	  
	  signals->ild._buffer[ii].framesOnPort = fpc;	  
	  signals->ild._buffer[ii].sampleRate = thisProcessor->getFsOut();
	  signals->ild._buffer[ii].numberOfChannels = nChannels;
	  signals->ild._buffer[ii].lastFrameIndex = thisProcessor->getNFR();
	  
  }
  	  
  return genom_ok;
}


/* --- Function getDependencies ----------------------------------------- */

/** Codel getDependencie of function getDependencies.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory, rosAFE_e_noSuchProcessor.
 */
genom_event
getDependencie(const char *nameProc, sequence_string *dependencies,
               const rosAFE_ids *ids, genom_context self)
{ 
  openAFE::procType type = findType(nameProc, ids);
  if ( type == _unknow ) 
	return rosAFE_e_noSuchProcessor( self );
	
  // Reserving genom_sequence only once
  if ( dependencies->_length == 0 ) {
	  
	  dependencies->_length = type ;
	  if ( genom_sequence_reserve(dependencies, type) )
		return rosAFE_e_noMemory( self );
  }
  
	if ( type == _inputProc ) {
		std::shared_ptr < InputProc > thisProcessor = ids->inputProcessorsSt->processorsAccessor.getProcessor ( nameProc );
		dependencies->_buffer[ type - 1 ] = strdup( thisProcessor->getName().c_str() );
	} else if ( type == _preProc ) {
		std::shared_ptr < PreProc > thisProcessor = ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc );
		dependencies->_buffer[ type - 1 ] = strdup( thisProcessor->getName().c_str() );
		getDependencie( thisProcessor->get_upperProcName().c_str(), dependencies, ids, self );
	} else if ( type == _gammatone ) {
		std::shared_ptr < GammatoneProc > thisProcessor = ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc );
		dependencies->_buffer[ type - 1 ] = strdup( thisProcessor->getName().c_str() );
		getDependencie( thisProcessor->get_upperProcName().c_str(), dependencies, ids, self );
	} else if ( type == _ihc ) {
		std::shared_ptr < IHCProc > thisProcessor = ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc );
		dependencies->_buffer[ type - 1 ] = strdup( thisProcessor->getName().c_str() );
		getDependencie( thisProcessor->get_upperProcName().c_str(), dependencies, ids, self );
	} else if ( type == _ild ) {
		std::shared_ptr < ILDProc > thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc );
		dependencies->_buffer[ type - 1 ] = strdup( thisProcessor->getName().c_str() );
		getDependencie( thisProcessor->get_upperProcName().c_str(), dependencies, ids, self );
	}
  
  return genom_ok;
}


/* --- Function getParameters ------------------------------------------- */

/** Codel getParameters of function getParameters.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory.
 */
genom_event
getParameters(const rosAFE_ids *ids, rosAFE_parameters *parameters,
              genom_context self)
{
	
/* ****************************  Input START  ************************************ */

  size_t sizeInputProc = ids->inputProcessorsSt->processorsAccessor.getSize();
  parameters->input._length = sizeInputProc;
  
  if (genom_sequence_reserve(&(parameters->input), sizeInputProc))
	return rosAFE_e_noMemory( self );

  for ( size_t ii = 0 ; ii < sizeInputProc ; ++ii ) {
	 
    std::shared_ptr < InputProc > thisProcessor = ids->inputProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
	parameters->input._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
	parameters->input._buffer[ii].in_doNormalize = thisProcessor->get_in_doNormalize();
	parameters->input._buffer[ii].in_normalizeValue = thisProcessor->get_in_normalizeValue();
    
    thisProcessor.reset();
  }
  
/* ****************************  PREPROC START  ************************************ */
  size_t sizePreProc = ids->preProcessorsSt->processorsAccessor.getSize();

  parameters->preProc._length = sizePreProc;  
  if ( genom_sequence_reserve(&(parameters->preProc), sizePreProc) )
	return rosAFE_e_noMemory( self );

 for ( size_t ii = 0 ; ii < parameters->preProc._length ; ++ii ) {
	 
    std::shared_ptr < PreProc > thisProcessor = ids->preProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
    parameters->preProc._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
    parameters->preProc._buffer[ii].pp_bRemoveDC = thisProcessor->get_pp_bRemoveDC();
    parameters->preProc._buffer[ii].pp_cutoffHzDC = thisProcessor->get_pp_cutoffHzDC();
    parameters->preProc._buffer[ii].pp_bPreEmphasis = thisProcessor->get_pp_bPreEmphasis();
    parameters->preProc._buffer[ii].pp_coefPreEmphasis = thisProcessor->get_pp_coefPreEmphasis();
    parameters->preProc._buffer[ii].pp_bNormalizeRMS = thisProcessor->get_pp_bNormalizeRMS();
    parameters->preProc._buffer[ii].pp_intTimeSecRMS = thisProcessor->get_pp_intTimeSecRMS();
    parameters->preProc._buffer[ii].pp_bLevelScaling = thisProcessor->get_pp_bLevelScaling();
    parameters->preProc._buffer[ii].pp_refSPLdB = thisProcessor->get_pp_refSPLdB();
    parameters->preProc._buffer[ii].pp_bMiddleEarFiltering = thisProcessor->get_pp_bMiddleEarFiltering(); 
    
	switch ( thisProcessor->get_pp_middleEarModel() ) {
		case _jepsen:
			parameters->preProc._buffer[ii].pp_middleEarModel = strdup ( "jepsen" );
			break;
		case _lopezpoveda:
			parameters->preProc._buffer[ii].pp_middleEarModel = strdup ( "lopezpoveda" );
			break;
		default:
			parameters->preProc._buffer[ii].pp_middleEarModel = strdup ( "unknow" );
			break;
	}
	
    parameters->preProc._buffer[ii].pp_bUnityComp = thisProcessor->get_pp_bUnityComp();
    
    thisProcessor.reset();
  }


/* **************************  FILTERBANK START  ********************************** */
  
  size_t sizeGammatone = ids->gammatoneProcessorsSt->processorsAccessor.getSize();

  parameters->gammatone._length = sizeGammatone;  
  if ( genom_sequence_reserve(&(parameters->gammatone), sizeGammatone) )
	return rosAFE_e_noMemory( self );

 for ( size_t ii = 0 ; ii < sizeGammatone ; ++ii ) {
	 
    std::shared_ptr < GammatoneProc > thisProcessor = ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
    parameters->gammatone._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
    
    switch ( thisProcessor->get_fb_type() ) {
		case _gammatoneFilterBank:
			parameters->gammatone._buffer[ii].fb_type = strdup("gammatone");
			break;
		case _drnlFilterBank:
			parameters->gammatone._buffer[ii].fb_type = strdup("drnl");
			break;
		default:
			parameters->gammatone._buffer[ii].fb_type = strdup("unknow");
			break;
	}
	
    parameters->gammatone._buffer[ii].fb_lowFreqHz = thisProcessor->get_fb_lowFreqHz();
    parameters->gammatone._buffer[ii].fb_highFreqHz = thisProcessor->get_fb_highFreqHz();
    parameters->gammatone._buffer[ii].fb_nERBs = thisProcessor->get_fb_nERBs();
    parameters->gammatone._buffer[ii].fb_nChannels = thisProcessor->get_fb_nChannels();
    parameters->gammatone._buffer[ii].fb_nGamma = thisProcessor->get_fb_nGamma();
    parameters->gammatone._buffer[ii].fb_bwERBs = thisProcessor->get_fb_bwERBs();

    parameters->gammatone._buffer[ii].fb_cfHz._length = thisProcessor->get_fb_nChannels();
	if ( genom_sequence_reserve(&(parameters->gammatone._buffer[ii].fb_cfHz), thisProcessor->get_fb_nChannels() ) )
		return rosAFE_e_noMemory( self );
	for ( size_t jj = 0 ; jj < thisProcessor->get_fb_nChannels() ; ++jj )
		parameters->gammatone._buffer[ii].fb_cfHz._buffer[jj] = *( thisProcessor->get_fb_cfHz() + jj );
    
    thisProcessor.reset(); 
  }

/* *****************************  IHC START  ************************************* */

  size_t sizeIhcProc = ids->ihcProcessorsSt->processorsAccessor.getSize();
  
  parameters->ihc._length = sizeIhcProc;
  if (genom_sequence_reserve(&(parameters->ihc), sizeIhcProc))
    return rosAFE_e_noMemory( self );

 for ( size_t ii = 0 ; ii < sizeIhcProc ; ++ii ) {
	 
    std::shared_ptr < IHCProc > thisProcessor = ids->ihcProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
	parameters->ihc._buffer[ii].name = strdup( thisProcessor->getName().c_str() );
    
    switch ( thisProcessor->get_ihc_method() ) {
		case _none:
			parameters->ihc._buffer[ii].ihc_method = strdup("none");
			break;
		case _halfwave:
			parameters->ihc._buffer[ii].ihc_method = strdup("halfwave");
			break;
		case _fullwave:
			parameters->ihc._buffer[ii].ihc_method = strdup("fullwave");
			break;
		case _square:
			parameters->ihc._buffer[ii].ihc_method = strdup("square");
			break;
		case _hilbert:
			parameters->ihc._buffer[ii].ihc_method = strdup("hilbert");
			break;
		case _joergensen:
			parameters->ihc._buffer[ii].ihc_method = strdup("joergensen");
			break;
		case _dau:
			parameters->ihc._buffer[ii].ihc_method = strdup("dau");
			break;
		case _breebart:
			parameters->ihc._buffer[ii].ihc_method = strdup("breebart");
			break;
		case _bernstein:
			parameters->ihc._buffer[ii].ihc_method = strdup("bernstein");
			break;
		default:
			parameters->ihc._buffer[ii].ihc_method = strdup("unknown");
			break;
	}
	
    thisProcessor.reset();
  }
  

/* *****************************  ILD START  ************************************* */
  
  size_t sizeIldProc = ids->ildProcessorsSt->processorsAccessor.getSize();

  parameters->ild._length = sizeIldProc;
  if (genom_sequence_reserve(&(parameters->ild), sizeIldProc))
    return rosAFE_e_noMemory( self );

 for ( size_t ii = 0 ; ii < sizeIldProc ; ++ii ) {
	 
    std::shared_ptr < ILDProc > thisProcessor = ids->ildProcessorsSt->processorsAccessor.getProcessor ( ii );
		  
	parameters->ild._buffer[ii].name = strdup( thisProcessor->getName().c_str() );	

    switch ( thisProcessor->get_ild_wname() ) {
		case _hamming:
			parameters->ild._buffer[ii].ild_wname = strdup("hamming");
			break;
		case _hanning:
			parameters->ild._buffer[ii].ild_wname = strdup("hanning");
			break;
		case _hann:
			parameters->ild._buffer[ii].ild_wname = strdup("hann");
			break;
		case _blackman:
			parameters->ild._buffer[ii].ild_wname = strdup("blackman");
			break;
		case _triang:
			parameters->ild._buffer[ii].ild_wname = strdup("triang");
			break;
		case _sqrt_win:
			parameters->ild._buffer[ii].ild_wname = strdup("sqrt_win");
			break;
		default:
			parameters->ild._buffer[ii].ild_wname = strdup("unknown");
			break;
	}

    parameters->ild._buffer[ii].ild_wSizeSec = thisProcessor->get_ild_wSizeSec();
    parameters->ild._buffer[ii].ild_hSizeSec = thisProcessor->get_ild_hSizeSec();
        
    thisProcessor.reset();
  }
/* ******************************  ILD END  ************************************** */
	
	
  return genom_ok;
}


/* --- Function modifyParameter ----------------------------------------- */

/** Codel modifyParameter of function modifyParameter.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noMemory, rosAFE_e_noSuchProcessor,
 *        rosAFE_e_noSuchParameter, rosAFE_e_badNewValue.
 */
genom_event
modifyParameter(const char *nameProc, const char *nameParam,
                const char *newValue, const rosAFE_ids *ids,
                genom_context self)
{
  apf::parameter_map params;
  std::string newValueSt = strdup( newValue );
  params.set("newValue", newValueSt);

  openAFE::procType type = findType(nameProc, ids);
  if ( type == _unknow ) 
	return rosAFE_e_noSuchProcessor( self );
  	
  try {
	  if ( type == _inputProc ) {
		if ( strcmp( nameParam, "in_doNormalize" ) == 0 ) {
			ids->inputProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_in_doNormalize( params.get<bool>("newValue") );
			return genom_ok;
		} else if ( strcmp( nameParam, "in_normalizeValue" ) == 0 ) {
				ids->inputProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_in_normalizeValue( params.get<unsigned long long>("newValue") );
				return genom_ok;
		} else return rosAFE_e_noSuchParameter(self);
	  }
	  else if ( type == _preProc ) {
		
		if ( strcmp( nameParam, "pp_bRemoveDC" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bRemoveDC( params.get<bool>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_cutoffHzDC" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_cutoffHzDC( params.get<double>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_bPreEmphasis" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bPreEmphasis( params.get<bool>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_coefPreEmphasis" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_coefPreEmphasis( params.get<double>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_bNormalizeRMS" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bNormalizeRMS( params.get<bool>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_intTimeSecRMS" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_intTimeSecRMS( params.get<double>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_bLevelScaling" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bLevelScaling( params.get<bool>("newValue") );
				return genom_ok;	
		} else if ( strcmp( nameParam, "pp_refSPLdB" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_refSPLdB( params.get<double>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_bMiddleEarFiltering" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bMiddleEarFiltering( params.get<bool>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_middleEarModel" ) == 0 ) {
				middleEarModel thisModel = _jepsen;
				if ( strcmp( params["newValue"].c_str(), "lopezpoveda" ) == 0 )
					thisModel = _lopezpoveda;
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_middleEarModel( thisModel );
				return genom_ok;
		} else if ( strcmp( nameParam, "pp_bUnityComp" ) == 0 ) {
				ids->preProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_pp_bUnityComp( params.get<bool>("newValue") );
				return genom_ok;																				
		} else return rosAFE_e_noSuchParameter(self);		
	  }
	  else if ( type == _gammatone ) {

		if ( strcmp( nameParam, "fb_nGamma" ) == 0 ) {
				ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_fb_nGamma( params.get<unsigned int>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "fb_bwERBs" ) == 0 ) {
				ids->gammatoneProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_fb_bwERBs( params.get<double>("newValue") );
				return genom_ok;
		} else return rosAFE_e_noSuchParameter(self);
	  }
	  else if ( type == _ihc ) {

		if ( strcmp( nameParam, "ihc_method" ) == 0 ) {
				ihcMethod thisMethod = _dau;
				if ( strcmp( params["newValue"].c_str(), "none" ) == 0 )
					thisMethod = _none;
				else if ( strcmp( params["newValue"].c_str(), "halfwave" ) == 0 )
					thisMethod = _halfwave;
				else if ( strcmp( params["newValue"].c_str(), "fullwave" ) == 0 )
					thisMethod = _fullwave;
				else if ( strcmp( params["newValue"].c_str(), "square" ) == 0 )
					thisMethod = _square;
				else if ( strcmp( params["newValue"].c_str(), "hilbert" ) == 0 )
					thisMethod = _hilbert;
				else if ( strcmp( params["newValue"].c_str(), "joergensen" ) == 0 )
					thisMethod = _joergensen;
				else if ( strcmp( params["newValue"].c_str(), "breebart" ) == 0 )
					thisMethod = _breebart;
				else if ( strcmp( params["newValue"].c_str(), "bernstein" ) == 0 )
					thisMethod = _bernstein;
				ids->ihcProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_ihc_method( thisMethod );
				return genom_ok;
		} else return rosAFE_e_noSuchParameter(self);
	  }
	  else if ( type == _ild ) {

		if ( strcmp( nameParam, "ild_wSizeSec" ) == 0 ) {
			ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_ild_wSizeSec( params.get<double>("newValue") );
			return genom_ok;
		} else if ( strcmp( nameParam, "ild_hSizeSec" ) == 0 ) {
				ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_ild_hSizeSec( params.get<double>("newValue") );
				return genom_ok;
		} else if ( strcmp( nameParam, "ild_wname" ) == 0 ) {
				ildWindowType thisWindow = _hann;
				if ( strcmp( params["newValue"].c_str(), "hamming" ) == 0 )
					thisWindow = _hamming;
				else if ( strcmp( params["newValue"].c_str(), "hanning" ) == 0 )
					thisWindow = _hanning;
				else if ( strcmp( params["newValue"].c_str(), "blackman" ) == 0 )
					thisWindow = _blackman;
				else if ( strcmp( params["newValue"].c_str(), "triang" ) == 0 )
					thisWindow = _triang;
				else if ( strcmp( params["newValue"].c_str(), "sqrt_win" ) == 0 )
					thisWindow = _sqrt_win;															
				ids->ildProcessorsSt->processorsAccessor.getProcessor ( nameProc )->set_ild_wname( thisWindow );
				return genom_ok;
		} else return rosAFE_e_noSuchParameter(self);
	  }  
  } catch ( const std::exception & e )  { 
		return rosAFE_e_badNewValue( self); 
  }
  return rosAFE_e_noSuchProcessor( self );
}


/* --- Function removeProcessor ----------------------------------------- */

/** Codel removeProcessor of function removeProcessor.
 *
 * Returns genom_ok.
 * Throws rosAFE_e_noSuchProcessor.
 */
genom_event
removeProcessor(const char *name, rosAFE_flagMap **flagMapSt,
                rosAFE_flagMap **newDataMapSt, genom_context self)
{
  SM::removeFlag( name, newDataMapSt, self );
  if ( SM::removeFlag( name, flagMapSt, self ) ) {
	sleep(1);
	return genom_ok;  
  }
  else
	return rosAFE_e_noSuchProcessor( self );
}
