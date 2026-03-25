#ifndef ESP_REQUESTS_H
#define ESP_REQUESTS_H

#include <cjson/cJSON.h>
#include <libserialport.h>


#define ESP_VID 4292
#define ESP_PID 60000

#define TIMEOUT 2000

int esp_switch_request(char *buffer, int buf_size, int on, int pin);
int esp_get_request(char *buffer, int buf_size, int pin, char *sensor, int sensor_size, char *model, int model_size);
int send_request(struct sp_port *port, char *req, int req_size, char *resp_buf, int resp_buf_size);
struct sp_port *setup_esp_port(char *port_name);
int is_esp_device(struct sp_port *port);

#endif