#ifndef SNIC_ETHERNET_SERVER_H
#define SNIC_ETHERNET_SERVER_H

#include <Arduino.h>
#include <Server.h>
#include "SNICClass.h"
#include "SNICEthernetClient.h"

class SNICEthernetClient;

class SNICEthernetServer : public Server {
  public:
    SNICEthernetServer(uint16_t port);                        // done
    SNICEthernetClient available();                           // done
    void begin();                                     // done
    size_t write(uint8_t c);                          // done
    size_t write(const uint8_t *buf, size_t size);    // done
    using Print::write;
  private:
    uint16_t port;
    int socketId;
};
#endif /* SNIC_ETHERNET_SERVER_H */
