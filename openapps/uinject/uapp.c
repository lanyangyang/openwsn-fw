//#include "uapp.h"
//#include "opendefs.h"
//#include "sock.h"
//#include "opentimers.h"
//#include "openserial.h"
//#include "packetfunctions.h"
//#include "openrandom.h"
//#include "idmanager.h"
//#define TASKPRIO_UDP 1  // 为 UDP 模块分配一个低优先级

////=========================== variables =======================================

//static sock_udp_t _sock;        // UDP socket
//static uapp_vars_t app_vars;    // Variables for the application

//static const uint8_t app_payload[] = "app_payload";  // Default payload
//static const uint8_t app_dst_addr[] = {
//    0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01   // Destination address (e.g., DAGroot)
//};

////=========================== prototypes ======================================

//void _app_timer_cb(opentimers_id_t id);
//void _app_task_cb(void);

////=========================== public ==========================================

//void uapp_init(void) {
//    // Clear local variables
//    memset(&_sock, 0, sizeof(sock_udp_t));
//    memset(&app_vars, 0, sizeof(uapp_vars_t));

//    // Create and bind UDP socket
//    sock_udp_ep_t local;
//    local.family = AF_INET6;
//    local.port = WKP_UDP_INJECT;  // Replace with desired port

//    if (sock_udp_create(&_sock, &local, NULL, 0) < 0) {
//        openserial_printf("Could not create socket\n");
//        return;
//    }
//    openserial_printf("Created UDP socket\n");

//    // Set the periodic timer
//    app_vars.period = APP_PERIOD_MS;  // Default period defined in the header
//    app_vars.timerId = opentimers_create(TIMER_GENERAL_PURPOSE, TASKPRIO_UDP);
//    opentimers_scheduleIn(
//        app_vars.timerId,
//        APP_PERIOD_MS,
//        TIME_MS,
//        TIMER_PERIODIC,
//        _app_timer_cb
//    );
//}

////=========================== private =========================================

//void _app_timer_cb(opentimers_id_t id) {
//    // Check if we need to schedule the task
//    if (openrandom_get16b() < 0xffff) {
//        _app_task_cb();  // Call task directly
//    }
//}

//void _app_task_cb(void) {
//    sock_udp_ep_t remote;
//    uint8_t payload[50];
//    uint8_t len = 0;

//    // Set remote destination
//    remote.family = AF_INET6;
//    remote.port = WKP_UDP_INJECT;  // Replace with actual port
//    memcpy(remote.addr.ipv6, app_dst_addr, sizeof(app_dst_addr));

//    // Build payload
//    memcpy(&payload[len], app_payload, sizeof(app_payload) - 1);
//    len += sizeof(app_payload) - 1;

//    // Add counter to payload
//    payload[len++] = (uint8_t)(app_vars.counter & 0x00ff);
//    payload[len++] = (uint8_t)((app_vars.counter & 0xff00) >> 8);
//    app_vars.counter++;

//    // Send payload
//    if (sock_udp_send(&_sock, payload, len, &remote) > 0) {
//        app_vars.busySending = TRUE;
//    } else {
//        openserial_printf("Failed to send UDP packet\n");
//    }
//}
