#ifndef UBUS_METHODS_H
#define UBUS_METHODS_H
#include <libubus.h>
#include <cjson/cJSON.h>

enum {
        PORT,
        PIN,
        __MAX_ON_OFF,
        MODEL = __MAX_ON_OFF,
        SENSOR,
        __MAX_GET
};

void esp_loop(struct ubus_context *ubus_ctx);
int reply_msg(struct ubus_context *ctx, struct ubus_request_data *req, int success, char *msg, struct cJSON *data);


#endif