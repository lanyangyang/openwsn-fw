#ifndef OPENWSN_CONFIG_H
#define OPENWSN_CONFIG_H

// ========================== Applications ==========================

/**
 * \def OPENWSN_C6T_C
 *
 * Application that allows direct manipulation of the 6top scheduling.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_C6T_C

/**
 * \def OPENWSN_CEXAMPLE_C
 *
 * A CoAP example application. It periodically sends a random string of numbers.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CEXAMPLE_C

/**
 * \def OPENWSN_CINFO_C
 *
 * CoAP application which responds with information about the OpenWSN version running on the board.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CINFO_C

/**
 * \def OPENWSN_CINFRARED_C
 *
 * A CoAP infrared application.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CINFRARED_C

/**
 * \def OPENWSN_CLED_C
 *
 * CoAP application that exposes the leds of the board as a CoAP resource.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CLED_C

/**
 * \def OPENWSN_CSENSORS_C
 *
 * A CoAP resource which allows an application to GET/SET the state of sensors.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CSENSORS_C

/**
 * \def OPENWSN_CSTORM_C
 *
 *
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CSTORM_C

/**
 * \def OPENWSN_CWELLKNOWN_C
 *
 * Implements the CoAP .wellknown endpoint on a mote.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_CWELLKNOWN_C

/**
 * \def OPENWSN_RRT_C
 *
 * A CoAP application.
 *
 * Requires: OPENWSN_COAP_C
 *
 */

// #define OPENWSN_RRT_C

/**
 * \def OPENWSN_UECHO_C
 *
 * An echo application (server side) that sits directly on top of UDP.
 *
 * Requires: OPENWSN_UDP_C
 *
 */

// #define OPENWSN_UECHO_C

/**
 * \def OPENWSN_UEXPIRATION_C
 *
 *
 *
 * Requires: OPENWSN_UDP_C
 *
 */

// #define OPENWSN_UEXPIRATION_C

/**
 * \def OPENWSN_UEXP_MONITOR_C
 *
 *
 *
 * Requires: OPENWSN_UDP_C
 *
 */

// #define OPENWSN_UEXP_MONITOR_C

/**
 * \def OPENWSN_UINJECT_C
 *
 * Application that creates UDP traffic and injects it in the network.
 *
 * Requires: OPENWSN_UDP_C
 *
 */

// #define OPENWSN_UINJECT_C

/**
 * \def OPENWSN_USERIALBRIDGE_C
 *
 *
 *
 * Requires: OPENWSN_OPENUDP_C
 *
 */

// #define OPENWSN_USERIALBRIDGE_C

// ========================== Application Layer ==========================

/**
 * \def OPENWSN_COAP_C
 *
 * Implementation of the CoAP protocol.
 *
 */

#define OPENWSN_COAP_C

/**
 * \def OPENWSN_CJOIN_C
 *
 * The CJOIN protocol performs a secure joining and install link-layer keys
 *
 */

#define OPENWSN_CJOIN_C


// ======================== Stack configuration ========================

/**
 * \def OPENWSN_OPENUDP_C
 *
 * Implementation of the UDP protocol.
 *
 */

#define OPENWSN_OPENUDP_C

/**
 * \def OPENWSN_6LO_FRAGMENTATION_C
 *
 * Implements 6LoWPAN fragmentation.
 *
 * Configuration options:
 *  - OPENWSN_MAX_PKTSIZE_SUPPORTED: defines the maximum IPV6 packet size (header + payload) the mote supports. Default
 *  value is 1320. This corresponds to a 40-byte IPv6 header + the minimal IPv6 MTU of 1280 bytes.
 *  - OPENWSN_MAX_NUM_BIGPKTS: defines how many static buffer space will be allocated for processing large packets.
 *
 */

#define OPENWSN_6LO_FRAGMENTATION_C
#define OPENWSN_MAX_PKTSIZE_SUPPORTED   1320
#define OPENWSN_MAX_NUM_BIGPKTS         2

/**
 * \def OPENWSN_AES_HW
 *
 * Enable AES hardware acceleration. This options is only available on boards that support hardware acceleration. It
 * cannot be combined with the python board.
 *
 */
// #define OPENWSN_AES_HW

/**
 * \def OPENWSN_OPENSERIAL_PRINTF
 *
 * Enable usage of openserial_printf function to print strings over the serial port.
 *
 */
// #define OPENWSN_OPENSERIAL_PRINTF

/**
 * \def OPENWSN_ADAPTIVE_MSF
 *
 * Allow the MSF algorithm to dynamically remove and allocate slots, based on the traffic load in the network.
 *
 */
#define OPENWSN_ADAPTIVE_MSF

#include "check_config.h"

#endif /* OPENWSN_CONFIG_H */
