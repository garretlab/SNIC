#include <Arduino.h>
#include "SNICClass.h"
#include "SNICEthernetClient.h."

SNICEthernetClient::SNICEthernetClient() {

}

int SNICEthernetClient::connect(IPAddress ip, uint16_t port) {
  uint16_t bytes;
  uint8_t ipAddress[4];

  for (int i = 0; i < 4; i++) {
    ipAddress[i] = ip[i];
  }

  if ((SNIC.snicTcpCreateSocket(&socketId)  == SNIC_SUCCESS)) {
    Serial.println("CreateSocket OK");
  } else {
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
  return SNIC.socketAvailable(socketId);
}

int SNICEthernetClient::read() {
  SNIC.socketReadChar(socketId);
}

