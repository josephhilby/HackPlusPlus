#include <stdlib.h>

#include "server.h"
#include "mem.h"

int main(void) {
    // init emu
    mem_init();

    // start webserver
    Server* s = server_create();
    const char* listen_addr = "http://0.0.0.0:8080";
    const char* web_root = "./web";
    server_init(s, listen_addr, web_root);

    // event loop
    for(;;) {
        server_poll(s, 1000);
    }

    // shutdown
    server_free(s);
}