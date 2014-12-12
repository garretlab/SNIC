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

#include "SNICClass.h"

SNICClass::SNICClass() {
  commandStatus = SNIC_COMMAND_RECEIVING;
  sequence = 0;
  needAck = 0;

  socketsReset();
}

void SNICClass::begin(Stream *serialPort, int resetPin) {
  this->serialPort = serialPort;
  this->resetPin = resetPin;
  pinMode(resetPin, OUTPUT);
}

int SNICClass::reset() {
  digitalWrite(resetPin, LOW);
  delay(100);
  digitalWrite(resetPin, HIGH);
  delay(100);

  if (waitFor(SNIC_CMD_ID_GEN, GEN_PWR_UP_IND, 0) == 0) {
    return 0;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::wifiJoin(char *ssid, uint8_t securityMode, uint8_t securityKeyLength, char *securityKey, unsigned long timeout) {
  uint8_t ssidLength = strlen(ssid);

  memcpy(&(sendBuffer.frame.general.buffer[0]), ssid, ssidLength);
  sendBuffer.frame.general.buffer[ssidLength] = 0;
  sendBuffer.frame.general.buffer[ssidLength + 1] = securityMode;
  if (securityMode != WIFI_SECURITY_OPEN) {
    sendBuffer.frame.general.buffer[ssidLength + 2] = securityKeyLength;
    memcpy(&(sendBuffer.frame.general.buffer[ssidLength + 3]), securityKey, securityKeyLength);
  } else {
    securityKeyLength = -1; // Adjust the payload length.
  }

  sendRequest(SNIC_CMD_ID_WIFI, WIFI_JOIN_REQ, ssidLength + securityKeyLength + 3);
  if (waitFor(SNIC_CMD_ID_WIFI, WIFI_JOIN_REQ, timeout) == 0) {
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::wifiDisconnect(unsigned long timeout) {
  sendRequest(SNIC_CMD_ID_WIFI, WIFI_DISCONNECT_REQ, 0);
  if (waitFor(SNIC_CMD_ID_WIFI, WIFI_DISCONNECT_REQ, timeout) == 0) {
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::wifiGetStatus(uint8_t interface, wifiGetStatusResponse_t *response, unsigned long timeout) {
  sendBuffer.frame.wifiGetStatus.interface = interface;

  sendRequest(SNIC_CMD_ID_WIFI, WIFI_GET_STATUS_REQ, sizeof(sendBuffer.frame.wifiGetStatus.interface));
  if (waitFor(SNIC_CMD_ID_WIFI, WIFI_GET_STATUS_REQ, timeout) == 0) {
    if (response) {
      switch (commandReturn.status) {
        case STA_JOINED:
        case AP_STARTED:
          strcpy((char *)response->ssid, (const char *)commandReturn.buffer[11]);
          // Fall through
        case NO_NETWORK:
          for (int i = 0; i < 6; i++) {
            response->macAddress[i] = commandReturn.buffer[i + 7];
          }
          break;
      }
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicInit(snicInitResponse_t *response, unsigned long timeout) {
  sendBuffer.frame.snicInit.bufferSize[0] = 0x08;
  sendBuffer.frame.snicInit.bufferSize[1] = 0x00;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_INIT_REQ, sizeof(sendBuffer.frame.snicInit));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_INIT_REQ, timeout) == 0) {
    if (response && (commandReturn.status == SNIC_SUCCESS)) {
      response->receiveBufferSize = commandReturn.buffer[7] << 8 | commandReturn.buffer[8];
      response->numberOfUdpSockets = commandReturn.buffer[9];
      response->numberOfTcpSockets = commandReturn.buffer[10];
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicSendFromSocket(uint8_t socketId, uint16_t payloadLength, uint8_t *payload, uint8_t option, uint16_t *numberOfBytes, unsigned long timeout) {
  sendBuffer.frame.snicSendFromSocket.socketId = socketId;
  sendBuffer.frame.snicSendFromSocket.option = option;
  sendBuffer.frame.snicSendFromSocket.payloadLength[0] = payloadLength >> 8;
  sendBuffer.frame.snicSendFromSocket.payloadLength[1] = payloadLength & 0xff;
  memcpy(sendBuffer.frame.snicSendFromSocket.payload, payload, payloadLength);

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_SEND_FROM_SOCKET_REQ, payloadLength + 4);
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_SEND_FROM_SOCKET_REQ, timeout) == 0) {
    if (commandReturn.status == SNIC_SUCCESS) {
      *numberOfBytes = commandReturn.buffer[7] << 8 | commandReturn.buffer[8];
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicCloseSocket(uint8_t socketId, unsigned long timeout) {
  sendBuffer.frame.snicCloseSocket.socketId = socketId;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_CLOSE_SOCKET_REQ, sizeof(sendBuffer.frame.snicCloseSocket));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_CLOSE_SOCKET_REQ, timeout) == 0) {
    if (socketFree(socketId) == SNIC_COMMAND_SUCCESS) {
      return commandReturn.status;
    } else {
      return SNIC_COMMAND_ERROR;
    }
  }
}

int SNICClass::snicSocketPartialClose(uint8_t socketId, uint8_t direction, unsigned long timeout) {
  sendBuffer.frame.snicSocketPartialClose.socketId = socketId;
  sendBuffer.frame.snicSocketPartialClose.direction = direction;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_SOCKET_PARTIAL_CLOSE_REQ, sizeof(sendBuffer.frame.snicSocketPartialClose));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_SOCKET_PARTIAL_CLOSE_REQ, timeout) == 0) {
    if (socketSetStatus(socketId, SNIC_SOCKET_STATUS_PARTILLIY_CLOSED) == 0) {
      return commandReturn.status;
    } else {
      return SNIC_COMMAND_ERROR;
    }
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicGetDhcpInfo(uint8_t interface, snicGetDhcpInfoResponse_t * response, unsigned long timeout) {
  sendBuffer.frame.snicGetDhcpInfo.interface = interface;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_GET_DHCP_INFO_REQ, sizeof(sendBuffer.frame.snicGetDhcpInfo));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_GET_DHCP_INFO_REQ, timeout) == 0) {
    for (int i = 0; i < 6; i++) {
      response->macAddress[i] = commandReturn.buffer[i + 7];
    }
    for (int i = 0; i < 4; i++) {
      response->localhost[i] = commandReturn.buffer[i + 13];
      response->gateway[i] = commandReturn.buffer[i + 17];
      response->netmask[i] = commandReturn.buffer[i + 21];
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicResolveName(uint8_t interface, char * hostname, uint8_t * ipAddress, unsigned long timeout) {
  uint8_t hostnameLength = strlen(hostname);

  sendBuffer.frame.snicResolveName.interface = interface;
  sendBuffer.frame.snicResolveName.length = hostnameLength;
  memcpy(sendBuffer.frame.snicResolveName.hostname, hostname, hostnameLength);

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_RESOLVE_NAME_REQ, hostnameLength + 2);
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_RESOLVE_NAME_REQ, timeout) == 0) {
    if (commandReturn.status == SNIC_SUCCESS) {
      for (int i = 0; i < 4; i++) {
        ipAddress[i] = commandReturn.buffer[i + 7];
      }
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicIpConfig(uint8_t interface, unsigned long timeout) {
  sendBuffer.frame.snicIpConfig.interface = interface;
  sendBuffer.frame.snicIpConfig.dhcp = 1;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_IP_CONFIG_REQ, 2);
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_IP_CONFIG_REQ, timeout) == 0) {
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicIpConfig(uint8_t interface, uint8_t * localhost, uint8_t * netmask, uint8_t * gateway, unsigned long timeout) {
  sendBuffer.frame.snicIpConfig.interface = interface;
  sendBuffer.frame.snicIpConfig.dhcp = 0;
  for (int i = 0; i < 4; i++) {
    sendBuffer.frame.snicIpConfig.localhost[i] = localhost[i];
    sendBuffer.frame.snicIpConfig.netmask[i] = netmask[i];
    sendBuffer.frame.snicIpConfig.gateway[i] = gateway[i];
  }

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_IP_CONFIG_REQ, sizeof(sendBuffer.frame.snicIpConfig));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_IP_CONFIG_REQ, timeout) == 0) {
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicDataIndAckConfig(uint8_t protocol, uint8_t ackEnable, uint16_t ackTimeout, uint8_t retryTimes, unsigned long timeout) {
  sendBuffer.frame.snicDataIndAckConfig.protocol = protocol;
  sendBuffer.frame.snicDataIndAckConfig.ackEnable = ackEnable;
  sendBuffer.frame.snicDataIndAckConfig.ackTimeout[0] = ackTimeout >> 8;
  sendBuffer.frame.snicDataIndAckConfig.ackTimeout[1] = ackTimeout & 0xff;
  sendBuffer.frame.snicDataIndAckConfig.retryTimes = retryTimes;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_DATA_IND_ACK_CONFIG_REQ, sizeof(sendBuffer.frame.snicDataIndAckConfig));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_DATA_IND_ACK_CONFIG_REQ, timeout) == 0) {
    return (commandReturn.status);
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicTcpCreateSocket(uint8_t * socketId, unsigned long timeout) {
  sendBuffer.frame.snicTcpCreateSocket.bind = 0;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_SOCKET_REQ, 1);
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_SOCKET_REQ, timeout) == 0) {
    if (commandReturn.status == SNIC_SUCCESS) {
      *socketId = commandReturn.buffer[7];
      if (socketAllocate(*socketId, -1, SNIC_SOCKET_STATUS_CREATED, SNIC_SOCKET_PROTOCOL_TCP) == SNIC_COMMAND_SUCCESS) {
        return SNIC_COMMAND_SUCCESS;
      } else {
        // command success but no slot. should never happen.
        snicCloseSocket(*socketId);
        return SNIC_COMMAND_ERROR;
      }
    } else { // command failure.
      return commandReturn.status;
    }
  } else { // timeout
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicTcpCreateSocket(uint16_t localPort, uint8_t *socketId, unsigned long timeout) {
  sendBuffer.frame.snicTcpCreateSocket.bind = 1;
  for (int i = 0; i < 4; i++) {
    sendBuffer.frame.snicTcpCreateSocket.localIpAddress[i] = 0;
  }
  sendBuffer.frame.snicTcpCreateSocket.localPort[0] = localPort >> 8;
  sendBuffer.frame.snicTcpCreateSocket.localPort[1] = localPort & 0xff;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_SOCKET_REQ, sizeof(sendBuffer.frame.snicTcpCreateSocket));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_SOCKET_REQ, timeout) == 0) {
    if (commandReturn.status == SNIC_SUCCESS) {
      *socketId = commandReturn.buffer[7];
      if (socketAllocate(*socketId, -1, SNIC_SOCKET_STATUS_CREATED, SNIC_SOCKET_PROTOCOL_TCP) == SNIC_COMMAND_SUCCESS) {
        return SNIC_COMMAND_SUCCESS;
      } else {
        // command success but no slot. should never happen.
        snicCloseSocket(*socketId);
        return SNIC_COMMAND_ERROR;
      }
    } else { // command failure.
      return commandReturn.status;
    }
  } else { // timeout
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicTcpCreateConnection(uint8_t socketId, uint16_t receiveBufferSize, uint8_t maximumClientConnctions, snicTcpConnectToServerResponse_t *response, unsigned long timeout) {
  if (socketGetStatus(socketId) != SNIC_SOCKET_STATUS_CREATED) {
    return SNIC_COMMAND_ERROR;
  }

  sendBuffer.frame.snicTcpCreateConnection.socket = socketId;
  sendBuffer.frame.snicTcpCreateConnection.receiveBufferSize[0] = receiveBufferSize >> 8;
  sendBuffer.frame.snicTcpCreateConnection.receiveBufferSize[1] = receiveBufferSize & 0xff;
  sendBuffer.frame.snicTcpCreateConnection.maximumClientConnctions = maximumClientConnctions;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_CONNECTION_REQ, sizeof(sendBuffer.frame.snicTcpCreateConnection));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_TCP_CREATE_CONNECTION_REQ, timeout) == 0) {
    if (commandReturn.status == SNIC_SUCCESS) {
      response->receiveBufferSize = commandReturn.buffer[7] << 8 | commandReturn.buffer[8];
      response->maximumClientConnctions = commandReturn.buffer[9];
      if (socketSetStatus(socketId, SNIC_SOCKET_STATUS_LISTEN) != SNIC_COMMAND_SUCCESS) {
        return SNIC_COMMAND_ERROR;
      }
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

int SNICClass::snicTcpConnectToServer(uint8_t socketId, uint8_t * server, uint16_t port, uint8_t connectTimeout, uint16_t * receiveBufferSize, unsigned long timeout) {
  sendBuffer.frame.snicTcpConnectToServer.socketId = socketId;
  for (int i = 0; i < 4; i++) {
    sendBuffer.frame.snicTcpConnectToServer.serverIpAddress[i] = server[i];
  }
  sendBuffer.frame.snicTcpConnectToServer.serverPort[0] = port >> 8;
  sendBuffer.frame.snicTcpConnectToServer.serverPort[1] = port & 0xff;
  sendBuffer.frame.snicTcpConnectToServer.receiveBufferSize[0] = SNIC_SOCKET_BUFFER_NOTIFICATION_SIZE >> 8;
  sendBuffer.frame.snicTcpConnectToServer.receiveBufferSize[1] = SNIC_SOCKET_BUFFER_NOTIFICATION_SIZE & 0xff;
  sendBuffer.frame.snicTcpConnectToServer.connectTimeout = connectTimeout;

  sendRequest(SNIC_CMD_ID_SNIC, SNIC_TCP_CONNECT_TO_SERVER_REQ, sizeof(sendBuffer.frame.snicTcpConnectToServer));
  if (waitFor(SNIC_CMD_ID_SNIC, SNIC_TCP_CONNECT_TO_SERVER_REQ, timeout) == 0) {
    switch (commandReturn.status) {
      case SNIC_SUCCESS:
        *receiveBufferSize = commandReturn.buffer[7] << 8 | commandReturn.buffer[8];
        socketSetStatus(socketId, SNIC_SOCKET_STATUS_CONNECTED);
        break;
      case SNIC_COMMAND_PENDING:
        while (socketGetStatus(socketId) == SNIC_SOCKET_STATUS_CREATED) {
          ;
        }
        break;
    }
    return commandReturn.status;
  } else {
    return SNIC_COMMAND_ERROR;
  }
}

void SNICClass::snicTcpConnectionStatus() {
  uint8_t socketId = receiveBuffer[7];

  switch (receiveBuffer[6]) {
    case SNIC_CONNECTION_UP:
      socketSetStatus(socketId, SNIC_SOCKET_STATUS_CONNECTED);
      break;
    case SNIC_SOCKET_PARTIALLY_CLOSED:
      socketSetStatus(socketId, SNIC_SOCKET_PARTIALLY_CLOSED);
      break;
    case SNIC_SOCKET_CLOSED:
    case SNIC_SEND_FAIL:
    case SNIC_CONNECT_TO_SERVER_FAIL:
    case SNIC_TIMEOUT:
    case SNIC_ACCEPT_SOCKET_FAIL:
      socketFree(socketId);
      break;
    default:
      socketFree(socketId);
      break;
  }
}

void SNICClass::snicTcpClientSocket() {
  uint8_t parentSocketId = receiveBuffer[6];
  uint8_t socketId = receiveBuffer[7];

  socketAllocate(socketId, parentSocketId, SNIC_SOCKET_STATUS_CONNECTED, SNIC_SOCKET_PROTOCOL_TCP);
}

void SNICClass::snicConnectionRecv() {
  uint8_t socketId = receiveBuffer[6];
  uint16_t payloadLength = (receiveBuffer[7] << 8) | receiveBuffer[8];
  for (int i = 0; i < payloadLength; i++) {
    socketWriteChar(socketId, receiveBuffer[i + 9]);
  }

  if (socketsWritable()) {
    ack();
  } else {
    needAck = 1;
  }

  needAck = 1;
}

int SNICClass::ack() {
  // This function may called from within interrupt handler.
  // So do not use sendBuffer or sendBuffer would be corrupted.
  serialPort->write(0x02);
  serialPort->write(0x80);
  serialPort->write(0x80);
  serialPort->write(0xff);
  serialPort->write(0xff);
  serialPort->write(0x04);
}

int SNICClass::accept(uint8_t listeningSocketId, uint8_t *clientSocketId) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].parentSocketId == listeningSocketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      socket[i].parentSocketId = -1;
      *clientSocketId = socket[i].socketId;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::select(uint8_t listeningSocketId, uint8_t *clientSocketId) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].parentSocketId == listeningSocketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      if (socketReadable(socket[i].socketId)) {
        *clientSocketId = socket[i].socketId;
        return SNIC_COMMAND_SUCCESS;
      }
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::sendRequest(uint8_t commandId, uint8_t subCommandId, uint16_t dataLength) {
  uint8_t checksum = 0;
  uint16_t payloadLength = dataLength + 2;

  sendBuffer.frame.som = SNIC_CMD_SOM;
  sendBuffer.frame.length[0] = (payloadLength & 0x007f) | 0x80;
  sendBuffer.frame.length[1] = ((payloadLength >> 7) & 0x003f) | 0x80;
  sendBuffer.frame.commandId = commandId | 0x80;
  sendBuffer.frame.subCommandId = subCommandId;
  sendBuffer.frame.sequence = sequence++;

  checksum = sendBuffer.frame.length[0] + sendBuffer.frame.length[1] + sendBuffer.frame.commandId;
  sendBuffer.data[payloadLength + 4] = checksum | 0x80;
  sendBuffer.data[payloadLength + 5] = SNIC_CMD_EOM;

  commandStatus = SNIC_COMMAND_RECEIVING;

  for (int i = 0; i < (payloadLength + 6); i++) {
    serialPort->write(sendBuffer.data[i]);
  }

  return payloadLength + 6;
}

int SNICClass::waitFor(uint8_t commandId, uint8_t subCommandId, unsigned long timeout) {
  unsigned long waitUntil = millis() + timeout;

  while ((commandStatus == SNIC_COMMAND_RECEIVING) ||
         ((commandReturn.commandId & 0x7f) != commandId) ||
         ((commandReturn.subCommandId & 0x7f) != subCommandId)) {
    if (timeout && (waitUntil < millis())) {
      return SNIC_COMMAND_ERROR;
    }
  }

  return 0;
}

void SNICClass::storeChar(uint8_t c) {
  static int receiving = 0;
  static int position;
  static int length;

  if (receiving) {
    receiveBuffer[position++] = c;
    if (position == SNIC_FIXED_HEADER_SIZE) {
      length = ((receiveBuffer[1] & 0x7f) | ((receiveBuffer[2] & 0x3f) << 7)) + UART_FIXED_SIZE_IN_FRAME;
    } else if (position == length) {
      receiving = 0;
      dispatch(length);
    }
  } else {
    if (c == SNIC_CMD_SOM) {
      receiving = 1;
      position = 0;
      length = 0;
      receiveBuffer[position++] = c;
    }
  }
}

void SNICClass::dispatch(int length) {
  if (((receiveBuffer[3] & 0x7f) == SNIC_CMD_ID_WIFI) && ((receiveBuffer[4] & 0x7f) == WIFI_NETWORK_STATUS_IND)) {
    ; // ignore
  } else if (((receiveBuffer[3] & 0x7f) == SNIC_CMD_ID_SNIC) && ((receiveBuffer[4] & 0x7f) == SNIC_TCP_CONNECTION_STATUS_IND)) {
    snicTcpConnectionStatus();
  } else if (((receiveBuffer[3] & 0x7f) == SNIC_CMD_ID_SNIC) && ((receiveBuffer[4] & 0x7f) == SNIC_TCP_CLIENT_SOCKET_IND)) {
    snicTcpClientSocket();
  } else if (((receiveBuffer[3] & 0x7f) == SNIC_CMD_ID_SNIC) && ((receiveBuffer[4] & 0x7f) == SNIC_CONNECTION_RECV_IND)) {
    snicConnectionRecv();
  } else {
    /* Copy to Command Buffer */
    for (int i = 0; i < length; i++) {
      commandReturn.buffer[i] = receiveBuffer[i];
    }
    commandStatus = SNIC_COMMNAD_RECEIVIED;
  }
}

int SNICClass::socketsReset() {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    socket[i].socketId = -1;
    socket[i].parentSocketId = -1;
    socket[i].protocol = SNIC_SOCKET_PROTOCOL_TCP;
    socket[i].status = SNIC_SOCKET_STATUS_NONEXIST;
    socket[i].head = socket[i].tail = 0;
  }
  return SNIC_COMMAND_SUCCESS;
}

// Allocate a new socket structure for specified socketId
int SNICClass::socketAllocate(int socketId, int parentSocketId, uint8_t status, uint8_t protocol) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].status == SNIC_SOCKET_STATUS_NONEXIST) {
      socket[i].socketId = socketId;
      socket[i].parentSocketId = parentSocketId;
      socket[i].protocol = protocol;
      socket[i].status = status;
      socket[i].head = socket[i].tail = 0;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketFree(int socketId) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].socketId == socketId) {
      socket[i].socketId = -1;
      socket[i].parentSocketId = -1;
      socket[i].protocol = SNIC_SOCKET_PROTOCOL_TCP;
      socket[i].status = SNIC_SOCKET_STATUS_NONEXIST;
      socket[i].head = socket[i].tail = 0;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketSetInformation(int socketId, int parentSocketId, uint8_t status, uint8_t protocol) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].socketId == socketId) {
      socket[i].parentSocketId = parentSocketId;
      socket[i].protocol = protocol;
      socket[i].status = status;
      socket[i].head = socket[i].tail = 0;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketGetStatus(int socketId) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].socketId == socketId) {
      return socket[i].status;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketSetStatus(int socketId, uint8_t status) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].socketId == socketId) {
      socket[i].status = status;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketReadable(int socketId) {
  // Think
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].socketId == socketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      return (SNIC_SOCKET_BUFFER_SIZE + socket[i].tail - socket[i].head) % SNIC_SOCKET_BUFFER_SIZE;
    }
  }

  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketReadChar(int socketId, uint8_t peek) {
  int ret = SNIC_COMMAND_ERROR;
  
  noInterrupts();
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].socketId == socketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      if (socket[i].head == socket[i].tail) {
        ret = SNIC_COMMAND_ERROR;
        break;
      } else {
        uint8_t c = socket[i].buffer[socket[i].tail];
        if (!peek) {
          socket[i].tail = (socket[i].tail + 1) % SNIC_SOCKET_BUFFER_SIZE;
          if ((needAck == 1) && socketsWritable()) {
            needAck = 0;
            ack();
          }
        }
        ret = c;
        break;
      }
    }
  }
  interrupts();
  return ret;
}

int SNICClass::socketsWritable() {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED) {
      int bufferFree;
      char buf[64];

      bufferFree = (SNIC_SOCKET_BUFFER_SIZE + socket[i].tail - socket[i].head - 1) % SNIC_SOCKET_BUFFER_SIZE;
      if (bufferFree < SNIC_SOCKET_BUFFER_NOTIFICATION_SIZE) {
        return 0;
      }
    }
  }
  return 1;
}

int SNICClass::socketWriteChar(int socketId, uint8_t c) {
  int ret = SNIC_COMMAND_ERROR;
  noInterrupts();
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].socketId == socketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      int nextHead = (socket[i].head + 1) % SNIC_SOCKET_BUFFER_SIZE;
      if (nextHead != socket[i].tail) {
        socket[i].buffer[socket[i].head] = c;
        socket[i].head = nextHead;
        interrupts();
        ret = SNIC_COMMAND_SUCCESS;
        break;
      } else {
        interrupts();
        ret = -2;
        break;
      }
    }
  }
  interrupts();
  return SNIC_COMMAND_ERROR;
}

int SNICClass::socketFlush(int socketId) {
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((socket[i].socketId == socketId) && (socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      socket[i].head = socket[i].tail = 0;
      return SNIC_COMMAND_SUCCESS;
    }
  }
  return SNIC_COMMAND_ERROR;
}

void SNICClass::uartHandler() {
  uint32_t status = USART0->US_CSR;

  // Did we receive data ?
  if ((status & US_CSR_RXRDY) == US_CSR_RXRDY) {
    SNIC.storeChar(USART0->US_RHR);
  }

  // Acknowledge errors
  if ((status & US_CSR_OVRE) == US_CSR_OVRE ||
      (status & US_CSR_FRAME) == US_CSR_FRAME)
  {
    // TODO: error reporting outside ISR
    USART0->US_CR |= US_CR_RSTSTA;
  }
}

void USART0_Handler(void)
{
  SNIC.uartHandler();
}

SNICClass SNIC;

void SNICClass::dumpBuffer(int number) {
  for (int i = 0; i < number; i++) {
    Serial.println(receiveBuffer[i], HEX);
  }
}

void SNICClass::dumpBufferAscii(int number) {
  for (int i = 0; i < number; i++) {
    Serial.print((char)receiveBuffer[i]);
  }
}
