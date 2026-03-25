#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>
#include <syslog.h>
#include <string.h>
#include <esp_requests.h>

int esp_switch_request(char *buffer, int buf_size, int on, int pin)
{
        return snprintf(buffer, buf_size, "{'action': '%s', 'pin': %d}", on ? "on" : "off", pin);
}

int esp_get_request(char *buffer, int buf_size, int pin, char *sensor, int sensor_size, char *model, int model_size)
{
        return snprintf(buffer, buf_size, "{'action': 'get', 'pin': %d, 'sensor': '%.*s', 'model': '%.*s'}", pin, sensor_size, sensor, model_size, model);
}

int send_request(struct sp_port *port, char *req, int req_size, char *resp_buf, int resp_buf_size)
{
        if (port == NULL || req == NULL || req == NULL) {
                return EXIT_FAILURE;
        }
        int res = sp_nonblocking_write(port, req, req_size);
        if (res < req_size) {
                return EXIT_FAILURE;
        }
        res = sp_blocking_read(port, resp_buf, resp_buf_size, TIMEOUT);
        if (res < 0) {
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}

struct sp_port *setup_esp_port(char *port_name)
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
