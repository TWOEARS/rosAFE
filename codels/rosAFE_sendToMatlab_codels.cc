#include "acrosAFE.h"

#include "rosAFE_c_types.h"

#include <sys/time.h>

struct timeval tv;

/* --- Task sendToMatlab ------------------------------------------------ */


/* --- Activity SendToMatlab -------------------------------------------- */

/** Codel initSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_exec.
 */
genom_event
initSendToMatlab(const rosAFE_dataObj *dataObj,
                 const rosAFE_runningProcessors *runningProcessors,
                 genom_context self)
{
  gettimeofday(&tv, NULL);
	 
  dataObj->data(self)->header.seq = 0;
  dataObj->data(self)->header.stamp.sec = tv.tv_sec;
  dataObj->data(self)->header.stamp.usec = tv.tv_usec;
	    
  dataObj->write( self );
  	
  return rosAFE_exec;
}

/** Codel send of activity SendToMatlab.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_exec, rosAFE_ether.
 */
genom_event
send(const rosAFE_dataObj *dataObj, const rosAFE_ids *ids,
     const rosAFE_runningProcessors *runningProcessors,
     genom_context self)
{
  gettimeofday(&tv, NULL);
	 
  dataObj->data(self)->header.seq += 1;
  dataObj->data(self)->header.stamp.sec = tv.tv_sec;
  dataObj->data(self)->header.stamp.usec = tv.tv_usec;
	    
	    
	    
  dataObj->write( self );  
  return rosAFE_exec;
}

/** Codel stopSendToMatlab of activity SendToMatlab.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 */
genom_event
stopSendToMatlab(genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_ether;
}
