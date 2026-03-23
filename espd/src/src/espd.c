#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <libserialport.h>
#include <ubus_methods.h>
#include <espd.h>

int main(void)
{
        openlog("tuyad", LOG_CONS | LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);


        struct ubus_context *ubus_ctx;
        if (uloop_init() != EXIT_SUCCESS) {
                syslog(LOG_CRIT, "Failed to initiate ubus context");
                goto err_no_init;
        }

        syslog(LOG_DEBUG, "Initiated ubus context");
        ubus_ctx = ubus_connect(NULL);

        if (ubus_ctx == NULL) {
                syslog(LOG_CRIT, "Failed to connect to ubusd");
                goto err_no_ubus_ctx;
        }

        syslog(LOG_DEBUG, "Connected to ubusd");
        ubus_add_uloop(ubus_ctx);


        esp_loop(ubus_ctx);

        ubus_free(ubus_ctx);
        syslog(LOG_DEBUG, "Freed ubus context");
err_no_ubus_ctx:
        uloop_done();
err_no_init:
        syslog(LOG_DEBUG, "Finished espd");
        closelog();
        return 0;
}
