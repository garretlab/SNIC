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

#ifndef SCNICLASS_H
#define SCNICLASS_H

#ifndef __SAM3X8E__
#error "Not Arduino Due"
#error "Remove this with care"
#endif

#include "SNICDefs.h"

class SNICClass {
  public:
    SNICClass();
    void begin(Stream *serialPort, int resetPin);
    int reset();

    // WiFi Commands
    int wifiJoin(char *ssid, uint8_t securityMode, uint8_t securityKeyLength, char *securityKey, unsigned long timeout = 0);
    int wifiDisconnect(unsigned long timeout = 0);
    int wifiGetStatus(uint8_t interface, wifiGetStatusResponse_t *response, unsigned long timeout = 0);
    
    // SNIC Commands
    int snicInit(snicInitResponse_t * response, unsigned long timeout = 0);
    int snicSendFromSocket(uint8_t socketId, uint16_t payloadLength, uint8_t *payload, uint8_t option, uint16_t *numberOfBytes, unsigned long timeout = 0);
    int snicCloseSocket(uint8_t socketId, unsigned long timeout = 0);
    int snicSocketPartialClose(uint8_t socketId, uint8_t direction, unsigned long timeout = 0);
    int snicGetDhcpInfo(uint8_t interface, snicGetDhcpInfoResponse_t *response, unsigned long timeout = 0);
    int snicResolveName(uint8_t interface, char *hostname, uint8_t *ipAddress, unsigned long timeout = 0);
    int snicIpConfig(uint8_t interface, unsigned long timeout = 0);
    int snicIpConfig(uint8_t interface, uint8_t *localhost, uint8_t *netmask, uint8_t *gateway, unsigned long timeout = 0);
    int snicDataIndAckConfig(uint8_t protocol, uint8_t ackEnable, uint16_t ackTimeout, uint8_t retryTimes, unsigned long timeout = 0);
    int snicTcpCreateSocket(uint8_t *socketId, unsigned long timeout = 0);
    int snicTcpCreateSocket(uint16_t localPort, uint8_t *socketId, unsigned long timeout = 0);
    int snicTcpCreateConnection(uint8_t socketId, uint16_t receiveBufferSize, uint8_t maximumClientConnctions, snicTcpConnectToServerResponse_t *response, unsigned long timeout = 0);
    int snicTcpConnectToServer(uint8_t socketId, uint8_t *server, uint16_t port, uint8_t connectTimeout, uint16_t *receiveBufferSize, unsigned long timeout = 0);
    void snicTcpConnectionStatus();
    void snicTcpClientSocket();
    void snicConnectionRecv();
    
    // ACK
    int ack();
    
    // Server Side Functions
    int accept(uint8_t listeningSocketId, uint8_t *clientSocketId);

    // Socket Operation Functions
    int socketsReset();
    int socketAllocate(int socketId, int parentSocketId, uint8_t status, uint8_t protocol);
    int socketFree(int socketId);
    int socketSetInformation(int socketId, int parentSocketId, uint8_t status, uint8_t protocol);
    int socketGetStatus(int socketId);
    int socketSetStatus(int socketId, uint8_t status);
    int socketAvailable(int socketId);
    int socketReadChar(int socketId, uint8_t peek = 0);
    int socketWriteChar(int socketId, uint8_t c);
    int socketsWritable();
    int socketFlush(int socketId);
    
    void uartHandler();

  private:
    Stream *serialPort;
    int resetPin;
    volatile uint8_t commandStatus;
    uint8_t sequence;
    volatile int needAck;
    SNICUartFrame_t sendBuffer;
    uint8_t receiveBuffer[SNIC_RECEIVE_BUFFER_SIZE];
    commandReturn_t commandReturn;
    SNICSocket_t socket[SNIC_MAX_SOCKET_NUM];

    int sendRequest(uint8_t commandId, uint8_t subCommandId, uint16_t dataLength);
    void storeChar(uint8_t c);
    int waitFor(uint8_t commandId, uint8_t subCommandId, unsigned long timeout);
    void dispatch(int length);
    
    void dumpBuffer(int number);
    void dumpBufferAscii(int number);
};

extern SNICClass SNIC;

#endif /* SCNICLASS_H */
