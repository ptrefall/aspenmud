#ifndef BAN_LIST_H
#define BAN_LIST_H
#include "mud.h"
#include "conf.h"
#include "serializer.h"
#include <vector>

class BanList:public ISerializable
{
  std::vector<unsigned long> _addresses;
public:
  BanList();
  ~BanList();
  BOOL AddAddress(const std::string &address);
  BOOL RemoveAddress(const std::string &address);
  BOOL AddressExists(const std::string &address);
  BOOL AddressExists(unsigned long address);
  void ListAddresses(std::vector<std::string> *addresses);
  void Serialize(TiXmlElement* root);
  void Deserialize(TiXmlElement* root);
};
#endif
