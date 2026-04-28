#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

/*
  VmCommand maps one VM command mnemonic to the emitter responsible
  for lowering that command into assembly.

  Emitter interface:
      emit(out, segment, data)

  The parser stores operands in Operation.segment and Operation.data.
  Individual emitters may rename those parameters for clarity:
      push/pop      -> segment, index
      function      -> name, n_vars (vars used by callee)
      call          -> name, n_args (args provided by caller)
      label/goto    -> _seg, label
      alu           -> _seg, _dat

  Note setment has a strict definition of: local, argument, this, that,
  temp, pointer, static, constant. Everything else is data. The exception is function
  names.

  Stack Frame Model (built top down):

  ...

  ------------------ caller stack ------------------

  (Model: caller frame + working stack)

  ------------------ callee frame ------------------

ARG →  arg 0 (RAM: becomes return value)
       arg 1 (RAM: becomes caller SP)
       ...
       arg n_args-1

       return-address (ROM: inst immediatly following callers `goto`)
       caller LCL
       caller ARG
       caller THIS
       caller THAT

LCL →  local 0 (RAM: n_vars init to 0)
       local 1
       ...
       local n_vars-1

------------------ working stack ------------------

SP  →  next free slot
*/

typedef void (*VmEmitter)(FILE* out, const char* segment, const char* data);

typedef struct {
    const char* mnemonic; // command mnemonic
    VmEmitter emit;       // command emitter function
} VmCommand;

#endif
