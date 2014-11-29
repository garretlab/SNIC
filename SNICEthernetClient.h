#ifndef SNIC_EHTERNET_CLIENT_H
#define SNIC_EHTERNET_CLIENT_H

#include <Arduino.h>
#include <Client.h>

class SNICEthernetClient : public Client {
  public:
    SNICEthernetClient();                                    // done

    virtual int connect(IPAddress ip, uint16_t port);        // done
    virtual int connect(const char *host, uint16_t port);    // done
    virtual size_t write(uint8_t);                           // done
    virtual size_t write(const uint8_t *buf, size_t size);   // done
    virtual int available();                                 // done
    virtual int read();                                      // done
    virtual int read(uint8_t *buf, size_t size);             // done
    virtual int peek();                                      // done
    virtual void flush();                                    // done
    virtual void stop();                                     // done
    virtual uint8_t connected();                             // done
    virtual operator bool();                                 // done
    virtual bool operator==(const SNICEthernetClient&);
    virtual bool operator!=(const SNICEthernetClient& rhs) {
      return !this->operator==(rhs);
    };

    using Print::write;
  private:
    uint8_t socketId;
};

#endif /* SNIC_EHTERNET_CLIENT_H */
