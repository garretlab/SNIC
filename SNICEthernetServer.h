#ifndef SNIC_EHTERNET_SERVER_H
#define SNIC_EHTERNET_SERVER_H

#include <Arduino.h>
#include <Server.h>
#include "SNICClass.h"

class SNICEthernetClient;

class SNICEthernetServer : public Server {
  public:
    SNICEthernetServer(uint16_t port);
    SNICEthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    using Print::write;
  private:
    uint16_t port;
    int socketId;
};
#endif /* SNIC_EHTERNET_SERVER_H */
