#ifndef SNIC_EHTERNET_CLIENT_H
#define SNIC_EHTERNET_CLIENT_H

#include <Arduino.h>
#include <Client.h>

class SNICEthernetClient : public Client {
  public:
    SNICEthernetClient();

    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int available();
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool();
    virtual bool operator==(const SNICEthernetClient&);
    virtual bool operator!=(const SNICEthernetClient& rhs) {
      return !this->operator==(rhs);
    };

    using Print::write;
  private:
    uint8_t socketId;
};

#endif /* SNIC_EHTERNET_CLIENT_H */
