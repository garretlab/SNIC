#include "SNICClass.h"
#include "SNICEthernetClass.h"
#include "SNICEthernetClient.h"

SNICEthernetClass::SNICEthernetClass() {
}

int SNICEthernetClass::begin(Stream *serialPort, char *accessPoint, uint8_t securityMode, char *key) {
  SNIC.begin(serialPort, 20);

  if ((SNIC.reset() != SNIC_SUCCESS) ||
      (SNIC.snicInit(NULL) != SNIC_SUCCESS)) {
    return 0;
  }

  delay(1000);

  if ((SNIC.wifiDisconnect() != SNIC_SUCCESS) ||
      (SNIC.wifiJoin(accessPoint, securityMode, strlen(key), key) != SNIC_SUCCESS) ||
      (SNIC.snicDataIndAckConfig(3, 1, 5000, 0) != SNIC_SUCCESS) ||
      (SNIC.snicIpConfig(0) != SNIC_SUCCESS)) {
    return 0;
  }

  return 1;
}

int SNICEthernetClass::begin(Stream *serialPort, char *accessPoint, uint8_t securityMode, char *key, IPAddress ip, IPAddress gateway, IPAddress subnet) {
  uint8_t ipAddress[4], gwAddress[4], netmask[4];
  
  for (int i = 0; i < 4; i++) {
    ipAddress[i] = ip[i];
    gwAddress[i] = gateway[i];
    netmask[i] = subnet[i];
  }
  
  SNIC.begin(serialPort, 20);

  if ((SNIC.reset() != SNIC_SUCCESS) ||
      (SNIC.snicInit(NULL) != SNIC_SUCCESS)) {
    return 0;
  }

  delay(1000);
  
  if ((SNIC.wifiDisconnect() != SNIC_SUCCESS) ||
      (SNIC.wifiJoin(accessPoint, securityMode, strlen(key), key) != SNIC_SUCCESS) ||
      (SNIC.snicDataIndAckConfig(3, 1, 5000, 0) != SNIC_SUCCESS) ||
      (SNIC.snicIpConfig(0, ipAddress, netmask, gwAddress) != SNIC_SUCCESS)) {
    return 0;
  }

  return 1;
}

IPAddress SNICEthernetClass::localIP() {
  IPAddress ipAdress;
  snicGetDhcpInfoResponse_t response;

  if ((SNIC.snicGetDhcpInfo(0, &response) == SNIC_SUCCESS)) {
    ipAdress = response.localhost;
  }
  return ipAdress;
}

IPAddress SNICEthernetClass::subnetMask() {
  IPAddress subnetMask;
  snicGetDhcpInfoResponse_t response;

  if ((SNIC.snicGetDhcpInfo(0, &response) == SNIC_SUCCESS)) {
    subnetMask = response.netmask;
  }
  return subnetMask;
}

IPAddress SNICEthernetClass::gatewayIP() {
  IPAddress gatewayIP;
  snicGetDhcpInfoResponse_t response;

  if ((SNIC.snicGetDhcpInfo(0, &response) == SNIC_SUCCESS)) {
    gatewayIP = response.gateway;
  }
  return gatewayIP;
}

int SNICEthernetClass::maintain() {
  return 0;
}

SNICEthernetClass SNICEthernet;
