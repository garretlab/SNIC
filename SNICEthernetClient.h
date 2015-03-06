#ifndef SNIC_ETHERNET_CLIENT_H
#define SNIC_ETHERNET_CLIENT_H

#include <Arduino.h>
#include "SNICClass.h"
#include <Client.h>

class SNICEthernetClient : public Client {
  public:
    SNICEthernetClient();                                    // done
    SNICEthernetClient(uint8_t socketId);                    // done

    int connect(IPAddress ip, uint16_t port);        // done
    int connect(const char *host, uint16_t port);    // done
    size_t write(uint8_t c);                         // done
    size_t write(const uint8_t *buf, size_t size);   // done
    int available();                                 // done
    int read();                                      // done
    int read(uint8_t *buf, size_t size);             // done
    int peek();                                      // done
    void flush();                                    // done
    void stop();                                     // done
    uint8_t connected();                             // done
    operator bool();                                 // done
    bool operator==(const SNICEthernetClient&);      // done
    bool operator!=(const SNICEthernetClient& rhs) {
      return !this->operator==(rhs);
    };

    using Print::write;
  private:
    int socketId;
};

#endif /* SNIC_ETHERNET_CLIENT_H */
