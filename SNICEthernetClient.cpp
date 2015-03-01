#include <Arduino.h>
#include "SNICClass.h"
#include "SNICEthernetClient.h."

SNICEthernetClient::SNICEthernetClient() {

}

SNICEthernetClient::SNICEthernetClient(uint8_t socketId) {
  this->socketId = socketId;
}

int SNICEthernetClient::connect(IPAddress ip, uint16_t port) {
  uint16_t bytes;
  uint8_t ipAddress[4];

  for (int i = 0; i < 4; i++) {
    ipAddress[i] = ip[i];
  }

  if ((SNIC.snicTcpCreateSocket((uint8_t *)&socketId) != SNIC_SUCCESS)) {
    return 0;
  }

  int retval = SNIC.snicTcpConnectToServer(socketId, ipAddress, port, 60, &bytes);
  if ((retval == SNIC_SUCCESS) || (retval == SNIC_COMMAND_PENDING)) {
    return 1;
  } else {
    return 0;
  }
}

int SNICEthernetClient::connect(const char *host, uint16_t port) {
  uint8_t ip[4];

  SNIC.snicResolveName(0, (char *)host, ip);
  return connect(ip, port);
}

size_t SNICEthernetClient::write(uint8_t b) {
  return write(&b, 1);
}

size_t SNICEthernetClient::write(const uint8_t *buf, size_t size) {
  uint16_t bytes;
  SNIC.snicSendFromSocket(socketId, (uint16_t) size, (uint8_t *)buf, 0, (uint16_t *)&bytes);
  return bytes;
}

int SNICEthernetClient::available() {
  int n = SNIC.socketReadable(socketId);
  
  if (n > 0) {
    return n;
  } else {
    return 0;
  }
}

int SNICEthernetClient::read() {
  SNIC.socketReadChar(socketId);
}

int SNICEthernetClient::read(uint8_t *buf, size_t size) {
  int i;
  for (i = 0; i < size; i++) {
    uint8_t c = SNIC.socketReadChar(socketId);
    if (c > 0) {
      buf[i] = c;
    } else {
      break;
    }
  }
  return i;
}

int SNICEthernetClient::peek() {
  SNIC.socketReadChar(socketId, 1);
}

void SNICEthernetClient::flush() {
  SNIC.socketFlush(socketId);
}

void SNICEthernetClient::stop() {
  SNIC.snicSocketPartialClose(socketId, 2);
  SNIC.snicCloseSocket(socketId);
}

uint8_t SNICEthernetClient::connected() {
  if (SNIC.socketGetStatus(socketId) == SNIC_SOCKET_STATUS_CONNECTED) {
    return 1;
  } else {
    return 0;
  }
}

SNICEthernetClient::operator bool() {
  return socketId != -1;
}

bool SNICEthernetClient::operator==(const SNICEthernetClient& rhs) {
  return socketId == rhs.socketId && socketId != -1 && rhs.socketId != -1;
}

