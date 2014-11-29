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
    int snicGetDhcpInfo(uint8_t interface, snicGetDhcpInfoResponse_t *response, unsigned long timeout = 0);
    int snicResolveName(uint8_t interface, char *hostname, uint8_t *ipAddress, unsigned long timeout = 0);
    int snicIpConfig(uint8_t interface, unsigned long timeout = 0);
    int snicIpConfig(uint8_t interface, uint8_t *localhost, uint8_t *netmask, uint8_t *gateway, unsigned long timeout = 0);
    int snicDataIndAckConfig(uint8_t protocol, uint8_t ackEnable, uint16_t ackTimeout, uint8_t retryTimes, unsigned long timeout = 0);
    int snicTcpCreateSocket(uint8_t *socketId, unsigned long timeout = 0);
    int snicTcpConnectToServer(uint8_t socketId, uint8_t *server, uint16_t port, uint8_t connectTimeout, uint16_t *receiveBufferSize, unsigned long timeout = 0);
    void snicTcpConnectionStatus();
    void snicConnectionRecv();
    
    // ACK
    int ack();

    // Socket Operation Functions
    int socketGetStatus(uint8_t socketId);
    int socketSetStatus(uint8_t socketId, uint8_t status);
    int socketAvailable(uint8_t socketId);
    int socketReadChar(uint8_t socketId);
    int socketWriteChar(uint8_t socketId, uint8_t c);
    int socketsWritable();
    
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