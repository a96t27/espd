#include <cjson/cJSON.h>
#include <stdlib.h>
#include <libserialport.h>
#include <syslog.h>
#include <string.h>
#include <esp_requests.h>

struct cJSON *esp_switch_request_json(int on, int pin)
{
        struct cJSON *json = cJSON_CreateObject();
        if (json == NULL) {
                return NULL;
        }
        if (on) {
                cJSON_AddStringToObject(json, "action", "on");
        } else {

                cJSON_AddStringToObject(json, "action", "off");
        }
        cJSON_AddNumberToObject(json, "pin", (double)pin);
        return json;
}

struct cJSON *esp_get_request_json(int pin, char *sensor, char *model)
{
        struct cJSON *json = cJSON_CreateObject();
        if (json == NULL) {
                return NULL;
        }
        cJSON_AddStringToObject(json, "action", "get");
        cJSON_AddNumberToObject(json, "pin", (double)pin);
        cJSON_AddStringToObject(json, "sensor", sensor);
        cJSON_AddStringToObject(json, "model", model);
        return json;
}

int send_esp_request(struct sp_port *port, struct cJSON *request, struct cJSON **response)
{
        if (port == NULL || request == NULL || response == NULL) {
                return EXIT_FAILURE;
        }
        int ret = EXIT_SUCCESS;
        char *request_str = cJSON_PrintUnformatted(request);
        int req_len = strlen(request_str);
        syslog(LOG_INFO, "Sending request to esp");
        int res = sp_nonblocking_write(port, request_str, req_len);
        if (res < req_len) {
                syslog(LOG_ERR, "failed to send");
                ret = EXIT_FAILURE;
                goto err_failed_send;
        }
        char buf[128] = "";
        syslog(LOG_INFO, "Receiving response from esp");
        sp_blocking_read(port, buf, sizeof(buf), TIMEOUT);
        *response = cJSON_ParseWithLength(buf, sizeof(buf));
err_failed_send:
        free(request_str);
        return ret;
}

struct sp_port *get_esp_port(char *port_name)
{
        struct sp_port *port;
        int ok = 1;
        if (sp_get_port_by_name(port_name, &port) != SP_OK) {
                return NULL;
        }
        if (!is_esp_device(port)) {
                ok = 0;
                goto end;
        }
        if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK) {
                ok = 0;
                goto end;
        }
        if (sp_set_baudrate(port, 9600) != SP_OK) {
                ok = 0;
                goto end;
        }
        if (sp_set_bits(port, 8) != SP_OK) {
                ok = 0;
                goto end;
        }
        if (sp_set_parity(port, SP_PARITY_NONE) != SP_OK) {
                ok = 0;
                goto end;
        }
        if (sp_set_stopbits(port, 1)) {
                ok = 0;
                goto end;
        }
        if (sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE)) {
                ok = 0;
                goto end;
        }
end:
        if (!ok) {
                sp_free_port(port);
                port = NULL;
        }
        return port;
}


int is_esp_device(struct sp_port *port)
{
        if (port == NULL) {
                return 0;
        }
        if (sp_get_port_transport(port) != SP_TRANSPORT_USB) {
                return 0;
        }
        int vendor_id;
        int product_id;
        if (sp_get_port_usb_vid_pid(port, &vendor_id, &product_id) != SP_OK) {
                return 0;
        }
        if (vendor_id != ESP_VID || product_id != ESP_PID) {
                return 0;
        }
        return 1;

}
