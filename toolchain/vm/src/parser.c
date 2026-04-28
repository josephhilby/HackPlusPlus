#include "parser.h"

/*
  Parses a cleaned VM source line into an Operation.

  General storage model:
      Operation := command segment data

  Command shapes:
      push/pop     command segment index
      function     command name    n_vars
      call         command name    n_args
      label/goto   command         label
      if-goto      command         label
      arithmetic   command
      return       command

  For one-operand commands, the operand is stored in op->data.
  For two-operand commands, operands are stored in op->segment and op->data.
*/

void parse_operation(Operation* op) {
    if (!op) {
        return;
    }

    long i;
    // command segment data, command data, command
    const char* first = op->ltrl;
    //         segment data,         data,
    const char* second = strchr(first, ' ');
    //                 data,
    const char* third = NULL;
    if (second) {
        third = strchr(second + 1, ' ');
    }

    if (second) {
        // command segment data, command data
        i = second - first;
        memcpy(op->command, first, i);
        op->command[i] = '\0';

        if (third) {
            // command segment data
            i = third - second - 1;
            memcpy(op->segment, second + 1, i);
            op->segment[i] = '\0';
            memcpy(op->data, third + 1, strlen(third));
            op->data[strlen(third)] = '\0';
        } else {
            // command data
            memcpy(op->data, second + 1, strlen(second));
            op->data[strlen(second)] = '\0';
        }
    } else {
        // command
        memcpy(op->command, first, strlen(first));
        op->command[strlen(first)] = '\0';
    }
}
