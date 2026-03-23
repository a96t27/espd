#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <libserialport.h>
#include <cjson/cJSON.h>
#include <ubusmsg.h>
#include <esp_requests.h>
#include <libserialport.h>
#include <ubus_methods.h>


int reply_msg(struct ubus_context *ctx, struct ubus_request_data *req, int success, char *msg, struct cJSON *data)
{
        struct blob_buf buf = { 0 };
        blobmsg_buf_init(&buf);
        blobmsg_add_u8(&buf, "success", success);
        blobmsg_add_string(&buf, "msg", msg);
        if (data != NULL) {
                char *data_str = cJSON_PrintUnformatted(data);
                void *table_cookie = blobmsg_open_table(&buf, "data");
                blobmsg_add_json_from_string(&buf, data_str);
                blobmsg_close_table(&buf, table_cookie);
                free(data_str);
        }
        ubus_send_reply(ctx, req, buf.head);
        syslog(LOG_ERR, "%s", msg);
        blob_buf_free(&buf);
        return EXIT_SUCCESS;
}


static const struct blobmsg_policy on_off_policy[__MAX_ON_OFF] = {
        [PIN] = {.name = "pin", .type = BLOBMSG_TYPE_INT32},
        [PORT] = {.name = "port", .type = BLOBMSG_TYPE_STRING},
};


static int espd_switch_cb(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
        (void)obj;
        int switchTo = 0;
        if (strcmp(method, "on") == 0) {
                switchTo = 1;
        }
        syslog(LOG_INFO, "Handling '%s' call", method);
        struct blob_attr *tb[__MAX_ON_OFF];
        char *port_name;
        int32_t pin;
        blobmsg_parse(on_off_policy, __MAX_ON_OFF, tb, blob_data(msg), blob_len(msg));
        if (!tb[PIN] || !tb[PORT]) {
                return UBUS_STATUS_INVALID_ARGUMENT;
        }
        pin = blobmsg_get_u32(tb[PIN]);
        port_name = blobmsg_get_string(tb[PORT]);

        struct cJSON *request = esp_switch_request_json(switchTo, pin);
        if (request == NULL) {
                reply_msg(ctx, req, 0, "Failed to generate request object for esp", NULL);
                goto err_no_request;
        }
        syslog(LOG_DEBUG, "Getting port '%s'", port_name);
        struct sp_port *port = get_esp_port(port_name);
        if (port == NULL) {

                reply_msg(ctx, req, 0, "Failed to get esp port", NULL);
                goto err_no_port;
        }
        struct cJSON *response = NULL;
        if (send_esp_request(port, request, &response) != EXIT_SUCCESS) {
                reply_msg(ctx, req, 0, "Got no response", NULL);
                goto err_no_response;
        }

        struct cJSON *rc_json = cJSON_GetObjectItemCaseSensitive(response, "rc");
        double rc = cJSON_GetNumberValue(rc_json);
        struct cJSON *resp_msg_json = cJSON_GetObjectItemCaseSensitive(response, "msg");
        char *resp_msg = cJSON_GetStringValue(resp_msg_json);
        struct cJSON *data = cJSON_GetObjectItemCaseSensitive(response, "data");
        reply_msg(ctx, req, rc == 0, resp_msg, data);

        cJSON_Delete(response);
err_no_response:
        sp_close(port);
        sp_free_port(port);
err_no_port:
        cJSON_Delete(request);
err_no_request:
        return UBUS_STATUS_OK;
}


static const struct blobmsg_policy get_policy[__MAX_GET] = {
        [PIN] = {.name = "pin", .type = BLOBMSG_TYPE_INT32},
        [PORT] = {.name = "port", .type = BLOBMSG_TYPE_STRING},
        [SENSOR] = {.name = "sensor", .type = BLOBMSG_TYPE_STRING},
        [MODEL] = {.name = "model", .type = BLOBMSG_TYPE_STRING},
};


static int espd_get_cb(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
        (void)obj;
        syslog(LOG_INFO, "Handling '%s' call", method);
        struct blob_attr *tb[__MAX_GET];
        char *port_name;
        char *sensor_name;
        char *model_name;
        int32_t pin;
        blobmsg_parse(get_policy, __MAX_GET, tb, blob_data(msg), blob_len(msg));
        if (!tb[PIN] || !tb[PORT] || !tb[SENSOR] || !tb[MODEL]) {
                return UBUS_STATUS_INVALID_ARGUMENT;
        }
        pin = blobmsg_get_u32(tb[PIN]);
        port_name = blobmsg_get_string(tb[PORT]);
        sensor_name = blobmsg_get_string(tb[SENSOR]);
        model_name = blobmsg_get_string(tb[MODEL]);

        struct cJSON *request = esp_get_request_json(pin, sensor_name, model_name);
        if (request == NULL) {
                reply_msg(ctx, req, 0, "Failed to generate request object for esp", NULL);
                goto err_no_request;
        }
        struct sp_port *port = get_esp_port(port_name);
        if (port == NULL) {
                reply_msg(ctx, req, 0, "Failed to get esp port", NULL);
                goto err_no_port;
        }
        struct cJSON *response = NULL;
        if (send_esp_request(port, request, &response) != EXIT_SUCCESS) {
                reply_msg(ctx, req, 0, "Got no response", NULL);
                goto err_no_response;
        }

        struct cJSON *rc_json = cJSON_GetObjectItemCaseSensitive(response, "rc");
        double rc = cJSON_GetNumberValue(rc_json);
        struct cJSON *resp_msg_json = cJSON_GetObjectItemCaseSensitive(response, "msg");
        char *resp_msg = cJSON_GetStringValue(resp_msg_json);
        struct cJSON *data = cJSON_GetObjectItemCaseSensitive(response, "data");
        int is_success = (rc == 0) && (data != NULL);
        reply_msg(ctx, req, is_success, resp_msg, data);

        cJSON_Delete(response);
err_no_response:
        sp_close(port);
        sp_free_port(port);
err_no_port:
        cJSON_Delete(request);
err_no_request:
        return UBUS_STATUS_OK;
}


static int espd_devices(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
        (void)obj;
        (void)method;
        (void)msg;
        syslog(LOG_INFO, "Handling 'devices' call");
        struct blob_buf buf = { 0 };
        if (blob_buf_init(&buf, 0) != EXIT_SUCCESS) {
                syslog(LOG_ERR, "Failed to init blob buffer");
                return UBUS_STATUS_UNKNOWN_ERROR;
        }
        struct sp_port **port_list;
        enum sp_return result = sp_list_ports(&port_list);
        int success = 1;
        if (result != SP_OK) {
                success = 0;
                goto err_no_port_list;
        }
        void *data_cookie = blobmsg_open_table(&buf, "data");
        void *arr_cookie = blobmsg_open_array(&buf, "devices");
        int i;
        void *table_cookie = NULL;
        char *port_name = "";
        int vendor_id;
        int product_id;
        for (i = 0; port_list[i] != NULL; i++) {
                struct sp_port *port = port_list[i];
                if (!is_esp_device(port)) {
                        continue;
                }

                table_cookie = blobmsg_open_table(&buf, "");
                port_name = sp_get_port_name(port);
                blobmsg_add_string(&buf, "port", port_name);
                sp_get_port_usb_vid_pid(port, &vendor_id, &product_id);
                blobmsg_add_u32(&buf, "product_id", product_id);
                blobmsg_add_u32(&buf, "vendor_id", vendor_id);
                blobmsg_close_table(&buf, table_cookie);
        }
        blobmsg_close_array(&buf, arr_cookie);
        blobmsg_close_table(&buf, data_cookie);
        sp_free_port_list(port_list);
err_no_port_list:
        blobmsg_add_u8(&buf, "success", success);
        if (success) {
                blobmsg_add_string(&buf, "msg", "Successfully got device list");
        } else {
                blobmsg_add_string(&buf, "msg", "Failed to get device list");
        }
        ubus_send_reply(ctx, req, buf.head);
        blob_buf_free(&buf);

        return UBUS_STATUS_OK;
}

static const struct ubus_method espd_methods[] = {
        UBUS_METHOD_NOARG("devices", espd_devices),
        UBUS_METHOD("on", espd_switch_cb, on_off_policy),
        UBUS_METHOD("off", espd_switch_cb, on_off_policy),
        UBUS_METHOD("get", espd_get_cb, get_policy),
};

static struct ubus_object_type espd_object_type = UBUS_OBJECT_TYPE("espd", espd_methods);

static struct ubus_object espd_object = {
        .name = "espd",
        .type = &espd_object_type,
        .methods = espd_methods,
        .n_methods = ARRAY_SIZE(espd_methods),
};


void esp_loop(struct ubus_context *ubus_ctx)
{
        syslog(LOG_DEBUG, "Added uloop to ubus context");
        ubus_add_object(ubus_ctx, &espd_object);

        syslog(LOG_DEBUG, "Added espd object to ubus context");
        uloop_run();
}