#include "acrosAFE.h"

#include "rosAFE_c_types.h"


/* --- Task preProc ----------------------------------------------------- */


/* --- Activity PreProc ------------------------------------------------- */

/** Codel startPreProc of activity PreProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
startPreProc(char **name, rosAFE_preProcessors **preProcessorsSt,
             genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitExec;
}

/** Codel waitExecPreProc of activity PreProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_waitExec, rosAFE_exec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
waitExecPreProc(char **name, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitExec;
}

/** Codel execPreProc of activity PreProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
execPreProc(char **name, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitRelease;
}

/** Codel waitReleasePreProc of activity PreProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
waitReleasePreProc(char **name, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitRelease;
}

/** Codel releasePreProc of activity PreProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noData.
 */
genom_event
releasePreProc(char **name, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitExec;
}

/** Codel stopPreProc of activity PreProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noData.
 */
genom_event
stopPreProc(rosAFE_preProcessors **preProcessorsSt,
            genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_ether;
}
