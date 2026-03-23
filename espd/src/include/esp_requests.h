#ifndef ESP_REQUESTS_H
#define ESP_REQUESTS_H

#include <cjson/cJSON.h>
#include <libserialport.h>


#define ESP_VID 4292
#define ESP_PID 60000

#define TIMEOUT 2000

struct cJSON *esp_switch_request_json(int on, int pin);
int send_esp_request(struct sp_port *port, struct cJSON *request, struct cJSON **response);
struct sp_port *get_esp_port(char *port_name);
int is_esp_device(struct sp_port *port);
struct cJSON *esp_get_request_json(int pin, char *sensor, char *model);

#endif