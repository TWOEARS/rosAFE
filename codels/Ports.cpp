#include "Ports.hpp"


/* initPort ----------------------------------------------------------------- */

int initInputPort(const rosAFE_inputProcessorOutput *inputProcessorOutput, genom_context self)
{
  int fop_l = 44100;
  int fop_r = 44100;
    
  assert ( fop_l == fop_r ) ;
    
  if (genom_sequence_reserve(&(inputProcessorOutput->data(self)->left), fop_l) ||
      genom_sequence_reserve(&(inputProcessorOutput->data(self)->right), fop_r))
        //return -E_NOMEM;
      std::cout << "Memory Error" << std::endl;

  inputProcessorOutput->data(self)->left._length = fop_l;
  inputProcessorOutput->data(self)->right._length = fop_r;

  for (unsigned int ii = 0; ii < fop_l; ii++) {
    inputProcessorOutput->data(self)->left._buffer[ii] = 0;
    inputProcessorOutput->data(self)->right._buffer[ii] = 0;
  }

  inputProcessorOutput->write(self);
  
  return 0;
}

/* publishPort ------------------------------------------------------------- */

int publishInputPort(const rosAFE_inputProcessorOutput *inputProcessorOutput, genom_context self)
{
    return 0;
}


