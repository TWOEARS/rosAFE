#include "acrosAFE.h"

#include "rosAFE_c_types.h"


/* --- Task input ------------------------------------------------------- */


/* --- Activity InputProc ----------------------------------------------- */

/** Codel startInputProc of activity InputProc.
 *
 * Triggered by rosAFE_start.
 * Yields to rosAFE_waitExec.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
startInputProc(const char *name, uint32_t nFramesPerBlock,
               uint32_t bufferSize_s,
               rosAFE_inputProcessors **inputProcessorsSt,
               const rosAFE_Audio *Audio,
               const rosAFE_TDSPorts *TDSPorts, rosAFE_infos *infos,
               genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitExec;
}

/** Codel waitExecInputProc of activity InputProc.
 *
 * Triggered by rosAFE_waitExec.
 * Yields to rosAFE_pause_waitExec, rosAFE_exec, rosAFE_stop.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
waitExecInputProc(uint32_t nFramesPerBlock, const rosAFE_Audio *Audio,
                  genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_pause_waitExec;
}

/** Codel execInputProc of activity InputProc.
 *
 * Triggered by rosAFE_exec.
 * Yields to rosAFE_waitRelease, rosAFE_stop.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
execInputProc(const char *name,
              const rosAFE_inputProcessors *inputProcessorsSt,
              genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_waitRelease;
}

/** Codel waitReleaseInputProc of activity InputProc.
 *
 * Triggered by rosAFE_waitRelease.
 * Yields to rosAFE_pause_waitRelease, rosAFE_release, rosAFE_stop.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
waitReleaseInputProc(const char *name, rosAFE_flagMap **flagMapSt,
                     genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_pause_waitRelease;
}

/** Codel releaseInputProc of activity InputProc.
 *
 * Triggered by rosAFE_release.
 * Yields to rosAFE_pause_waitExec, rosAFE_stop.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
releaseInputProc(const char *name,
                 rosAFE_inputProcessors **inputProcessorsSt,
                 rosAFE_flagMap **newDataMapSt,
                 const rosAFE_TDSPorts *TDSPorts, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_pause_waitExec;
}

/** Codel stopInputProc of activity InputProc.
 *
 * Triggered by rosAFE_stop.
 * Yields to rosAFE_ether.
 * Throws rosAFE_e_noData, rosAFE_e_noMemory, rosAFE_e_existsAlready.
 */
genom_event
stopInputProc(const char *name, const rosAFE_TDSPorts *TDSPorts,
              rosAFE_inputProcessors **inputProcessorsSt,
              rosAFE_flagMap **flagMapSt,
              rosAFE_flagMap **newDataMapSt, genom_context self)
{
  /* skeleton sample: insert your code */
  /* skeleton sample */ return rosAFE_ether;
}
