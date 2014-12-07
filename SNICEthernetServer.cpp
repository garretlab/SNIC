#include "SNICEthernetServer.h"

SNICEthernetServer::SNICEthernetServer(uint16_t port) {
  this->port = port;
}

SNICEthernetClient SNICEthernetServer::available() {
  uint8_t clientSocketId;
  
  if (SNIC.select(socketId, &clientSocketId) == SNIC_COMMAND_SUCCESS) {
    SNICEthernetClient client(clientSocketId);
    return client;
  }

  return NULL;
}

void SNICEthernetServer::begin() {
  snicTcpConnectToServerResponse_t response;

  SNIC.snicTcpCreateSocket(port, (uint8_t *)&socketId);
  SNIC.snicTcpCreateConnection(socketId, SNIC_SOCKET_BUFFER_SIZE, 4, &response);
}

size_t SNICEthernetServer::write(uint8_t c) {
  return write(&c, 1);
}

size_t SNICEthernetServer::write(const uint8_t *buf, size_t size) {
  uint16_t bytes;
  
  for (int i = 0; i < SNIC_MAX_SOCKET_NUM; i++) {
    if ((SNIC.socket[i].parentSocketId == socketId) && (SNIC.socket[i].status == SNIC_SOCKET_STATUS_CONNECTED)) {
      SNIC.snicSendFromSocket(SNIC.socket[i].socketId, (uint16_t) size, (uint8_t *)buf, 0, (uint16_t *)&bytes);
    }
  }
  
  return bytes;
}

