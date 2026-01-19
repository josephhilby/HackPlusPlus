// create emu instance
// initialize server
// start event loop
// shutdown
#include <stdlib.h>

#include "server.h"
#include "mem.h"

int main(void) {
    mem_init();
    Server* s = server_create();
    const char* listen_addr = "http://0.0.0.0:8080";
    const char* web_root = "./web";
    server_init(s, listen_addr, web_root);

    for(;;) {
        server_poll(s, 1000);
    }

    server_free(s);
}