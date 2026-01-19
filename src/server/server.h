// (protocol.h) define message type, payload structs, encode/decode, validation of req fields

#ifndef SERVER_H
#define SERVER_H

typedef struct Server Server;
Server* server_create(void);
int server_init(Server* s, const char* listen_addr, const char* web_root);
void server_poll(Server* s, int timeout_ms);
void server_free(Server* s);
void server_destroy(Server* s);

#endif //SERVER_H