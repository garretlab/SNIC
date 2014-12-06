#include "SNICEthernetServer.h"
#include "SNICEthernetClient.h"

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
