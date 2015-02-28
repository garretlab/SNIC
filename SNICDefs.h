/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* Copyright (C) 2014 Murata Manufacturing Co.,Ltd., MIT License
 *  port to the muRata, SWITCH SCIENCE Wi-FI module TypeYD SNIC-UART.
 */

/*
 * Copyright (c) 2014 garretlab at gmail.com, MIT License
 * port to Arduino.
 */

#ifndef SNICDEFS_H
#define SNICDEFS_H
#include "Arduino.h"

#define SNIC_CMD_SOM        0x02
#define SNIC_CMD_EOM        0x04

/* SNIC UART Command ID */
#define SNIC_CMD_ID_GEN     0x01    // General command
#define SNIC_CMD_ID_WIFI    0x50    // Wi-Fi command
#define SNIC_CMD_ID_SNIC    0x70    // SNIC command

/* SNIC UART Subcommand ID */
#define WIFI_JOIN_REQ           0x02   // Associate to a network
#define WIFI_DISCONNECT_REQ     0x03   // Disconnect from a network
#define WIFI_GET_STATUS_REQ     0x04   // Get WiFi status
#define WIFI_NETWORK_STATUS_IND 0x10

#define SNIC_INIT_REQ                      0x00    // SNIC API initialization
#define SNIC_SEND_FROM_SOCKET_REQ          0x02    // Send from socket
#define SNIC_CLOSE_SOCKET_REQ              0x03    // Close socket
#define SNIC_SOCKET_PARTIAL_CLOSE_REQ      0x04    // Socket partial close
#define SNIC_GET_DHCP_INFO_REQ             0x09    // Get DHCP info
#define SNIC_RESOLVE_NAME_REQ              0x0a    // Resolve a host name to IP address
#define SNIC_IP_CONFIG_REQ                 0x0b    // Configure DHCP or static IP
#define SNIC_DATA_IND_ACK_CONFIG_REQ       0x0c    // ACK configuration for data indications
#define SNIC_TCP_CREATE_SOCKET_REQ         0x10    // Create TCP socket
#define SNIC_TCP_CREATE_CONNECTION_REQ     0x11    // Create TCP connection server
#define SNIC_TCP_CONNECT_TO_SERVER_REQ     0x12    // Connect to TCP server
#define SNIC_TCP_CONNECTION_STATUS_IND     0x20    // Connection status indication
#define SNIC_TCP_CLIENT_SOCKET_IND         0x21    // TCP client socket indication
#define SNIC_CONNECTION_RECV_IND           0x22    // TCP or connected UDP packet received indication

#define GEN_PWR_UP_IND 0x00

/* SNIC UART Command response status code */
#define SNIC_SUCCESS                   0x00
#define SNIC_ACCEPT_SOCKET_FAIL        0x08
#define SNIC_SOCKET_PARTIALLY_CLOSED   0x0a
#define SNIC_SOCKET_CLOSED             0x0b
#define SNIC_SEND_FAIL                 0x0e
#define SNIC_CONNECT_TO_SERVER_FAIL    0x0f
#define SNIC_TIMEOUT                   0x11
#define SNIC_CONNECTION_UP             0x12
#define SNIC_COMMAND_PENDING           0x18

#define WIFI_SECURITY_OPEN         0x00
#define WIFI_SECURITY_WEP          0x01
#define WIFI_SECURITY_WPA_TKIP_PSK 0x02
#define WIFI_SECURITY_WPA2_AES_PSK 0x04
#define WIFI_SECURITY_WPA2_MIXED   0x06
#define WIFI_SECURITY_WPA_AES      0x07

#define WIFI_OFF   0
#define NO_NETWORK 1
#define STA_JOINED 2
#define AP_STARTED 3

#define SNIC_FIXED_HEADER_SIZE    3
#define UART_FIXED_SIZE_IN_FRAME  6

#define SNIC_MAX_TCP_SOCKET_NUM 5
#define SNIC_MAX_UDP_SOCKET_NUM 4
#define SNIC_MAX_SOCKET_NUM (SNIC_MAX_TCP_SOCKET_NUM + SNIC_MAX_UDP_SOCKET_NUM)

#define SNIC_SEND_BUFFER_SIZE                128  // Buffer to keep data to snic.
#define SNIC_RECEIVE_BUFFER_SIZE             128  // Buffer to keep data from snic for each socket.
#define SNIC_SOCKET_BUFFER_SIZE               64  // Buffer to keep received data for each socket.
#define SNIC_SOCKET_BUFFER_NOTIFICATION_SIZE  63  // Buffer size to tell SNIC.

#define SNIC_COMMAND_RECEIVING 0
#define SNIC_COMMNAD_RECEIVIED 1

#define SNIC_SOCKET_STATUS_NONEXIST           0x00
#define SNIC_SOCKET_STATUS_CREATED            0x01
#define SNIC_SOCKET_STATUS_PARTILLIY_CLOSED   0x02
#define SNIC_SOCKET_STATUS_CLOSED             0x03
#define SNIC_SOCKET_STATUS_CONNECTION_PENDING 0x04
#define SNIC_SOCKET_STATUS_CONNECTED          0x05
#define SNIC_SOCKET_STATUS_LISTEN             0x06

#define SNIC_SOCKET_STATUS_CLOSED         0x01
#define SNIC_SOCKET_STATUS_LISTEN         0x02
#define SNIC_SOCKET_STATUS_SENT           0x03
#define SNIC_SOCKET_STATUS_RECEIVED       0x04
#define SNIC_SOCKET_STATUS_ESTABLISHTED   0x05
#define SNIC_SOCKET_STATUS_CLOSE_WAIT     0x06
#define SNIC_SOCKET_STATUS_LAST_ACK       0x07
#define SNIC_SOCKET_STATUS_FIN_WAIT1      0x08
#define SNIC_SOCKET_STATUS_CLOSING        0x09
#define SNIC_SOCKET_STATUS_FIN_WAIT2      0x0a
#define SNIC_SOCKET_STATUS_TIME_WAIT      0x0b

#define SNIC_SOCKET_PROTOCOL_TCP 0x01
#define SNIC_SOCKET_PROTOCOL_UDP 0x02

#define SNIC_COMMAND_SUCCESS  0
#define SNIC_COMMAND_ERROR   -1

typedef union {
  uint8_t data[SNIC_SEND_BUFFER_SIZE];
  struct {
    uint8_t som;
    uint8_t length[2];
    uint8_t commandId;
    uint8_t subCommandId;
    uint8_t sequence;
    union {
      struct {
        uint8_t interface;
      } wifiGetStatus;
      struct {
        uint8_t bufferSize[2];
      } snicInit;
      struct {
        uint8_t socketId;
        uint8_t option;
        uint8_t payloadLength[2];
        uint8_t payload[SNIC_SEND_BUFFER_SIZE - 10];
      } snicSendFromSocket;
      struct {
        uint8_t socketId;
      } snicCloseSocket;
      struct {
        uint8_t socketId;
        uint8_t direction;
      } snicSocketPartialClose;
      struct {
        uint8_t interface;
      } snicGetDhcpInfo;
      struct {
        uint8_t interface;
        uint8_t length;
        uint8_t hostname[128];
      } snicResolveName;
      struct {
        uint8_t bind;
        uint8_t localIpAddress[4];
        uint8_t localPort[2];
      } snicTcpCreateSocket;
      struct {
        uint8_t socket;
        uint8_t receiveBufferSize[2];
        uint8_t maximumClientConnctions;
      } snicTcpCreateConnection;
      struct {
        uint8_t socketId;
        uint8_t serverIpAddress[4];
        uint8_t serverPort[2];
        uint8_t receiveBufferSize[2];
        uint8_t connectTimeout;
      } snicTcpConnectToServer;
      struct {
        uint8_t interface;
        uint8_t dhcp;
        uint8_t localhost[4];
        uint8_t netmask[4];
        uint8_t gateway[4];
      } snicIpConfig;
      struct {
        uint8_t protocol;
        uint8_t ackEnable;
        uint8_t ackTimeout[2];
        uint8_t retryTimes;
      } snicDataIndAckConfig;
      struct {
        uint8_t buffer[SNIC_SEND_BUFFER_SIZE - 6];
      } general;
    };
  } frame;
} SNICUartFrame_t;

typedef struct {
  uint8_t macAddress[6];
  uint8_t ssid[33];
} wifiGetStatusResponse_t;

typedef struct {
  uint16_t receiveBufferSize;
  uint8_t numberOfUdpSockets;
  uint8_t numberOfTcpSockets;
} snicInitResponse_t;

typedef struct {
  uint8_t macAddress[6];
  uint8_t localhost[4];
  uint8_t gateway[4];
  uint8_t netmask[4];
} snicGetDhcpInfoResponse_t;

typedef struct {
  uint16_t receiveBufferSize;
  uint8_t maximumClientConnctions;
} snicTcpConnectToServerResponse_t;

typedef union {
  uint8_t buffer[SNIC_RECEIVE_BUFFER_SIZE];
  struct {
    uint8_t som;
    uint8_t payloadLengt[2];
    uint8_t commandId;
    uint8_t subCommandId;
    uint8_t sequence;
    uint8_t status;
  };
} commandReturn_t;

typedef struct {
  int socketId;
  int parentSocketId;
  uint8_t protocol;
  uint16_t port;
  volatile uint8_t status;
  uint8_t buffer[SNIC_SOCKET_BUFFER_SIZE];
  int head;
  int tail;
} SNICSocket_t;

#endif /* SNICDEFS_H */
