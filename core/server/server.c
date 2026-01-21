#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "server.h"
#include "mongoose.h"
#include "mem.h"
#include "const.h"

struct Server {
    struct mg_mgr mgr;
    const char* listen_addr;   // e.g. "http://0.0.0.0:8080"
    const char* web_root;      // e.g. "./web"
};

Server* server_create(void) {
    Server* s = malloc(sizeof(Server));
    if (!s) {
        return NULL;
    }
    memset(s, 0, sizeof(*s));
    return s;
}

static void send_full_screen(struct mg_connection* c) {
    const uint16_t* screen = screen_read();

    // serialize as little-endian bytes: 8192 words => 16384 bytes
    static uint8_t payload[SCREEN_LEN * 2];
    for (int i = 0; i < SCREEN_LEN; i++) {
        uint16_t w = screen[i];
        payload[2*i + 0] = (uint8_t)(w & 0xFF);
        payload[2*i + 1] = (uint8_t)((w >> 8) & 0xFF);
    }

    mg_ws_send(c, (const char*)payload, sizeof(payload), WEBSOCKET_OP_BINARY);
}

static void on_event(struct mg_connection* c, int ev, void* ev_data) {
    Server* s = (Server*) c->fn_data;

    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*) ev_data;

        // WS upgrade endpoint
        if (hm->uri.len == 3 && memcmp(hm->uri.buf, "/ws", 3) == 0) {
            mg_ws_upgrade(c, hm, NULL);
            return;
        }

        // static files
        struct mg_http_serve_opts opts = {0};
        opts.root_dir = s->web_root;
        mg_http_serve_dir(c, hm, &opts);
        return;
    }

    if (ev == MG_EV_WS_OPEN) {
        fprintf(stderr, "[ws] connected\n");
        // Optional hello
        mg_ws_send(c, "hello", 5, WEBSOCKET_OP_TEXT);
        // Send the full screen immediately
        send_full_screen(c);
        return;
    }

    if (ev == MG_EV_WS_MSG) {
        // Optional: if client sends "get", resend
        struct mg_ws_message* wm = (struct mg_ws_message*) ev_data;
        if (wm->data.len == 3 && memcmp(wm->data.buf, "get", 3) == 0) {
            send_full_screen(c);
        }
        return;
    }

    if (ev == MG_EV_CLOSE && c->is_websocket) {
        fprintf(stderr, "[ws] disconnected\n");
    }
}

int server_init(Server* s, const char* listen_addr, const char* web_root) {
    memset(s, 0, sizeof(*s));
    s->listen_addr = listen_addr;
    s->web_root = web_root;

    mg_mgr_init(&s->mgr);

    if (mg_http_listen(&s->mgr, s->listen_addr, on_event, s) == NULL) {
        fprintf(stderr, "Failed to listen on %s\n", s->listen_addr);
        mg_mgr_free(&s->mgr);
        return 0;
    }

    fprintf(stderr, "Listening on %s (web_root=%s)\n", s->listen_addr, s->web_root);
    return 1;
}

void server_poll(Server* s, int timeout_ms) {
    mg_mgr_poll(&s->mgr, timeout_ms);
}

void server_free(Server* s) {
    mg_mgr_free(&s->mgr);
}

void server_destroy(Server* s) {
    if (!s) {
        return;
    }
    server_free(s);
    free(s);
}
