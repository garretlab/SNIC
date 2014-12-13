#ifndef SNIC_ETHERNET_H
#define SNIC_ETHERNET_H

#include <Arduino.h>
#include <IPAddress.h>
#include "SNICClass.h"

class SNICEthernetClass {
  public:
    SNICEthernetClass();
    int begin(Stream *serialPort, char *accessPoint, uint8_t securityMode, char *key);
    int begin(Stream *serialPort, char *accessPoint, uint8_t securityMode, char *key, IPAddress ip, IPAddress gateway, IPAddress subnet);
    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    int maintain();
  private:
};

extern SNICEthernetClass SNICEthernet;

#endif /* SNIC_ETHERNET_H */
