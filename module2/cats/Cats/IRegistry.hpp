#pragma once
#include <string>

class IRegistry {
public:
  virtual ~IRegistry() = default;
  virtual bool TryToSave(std::string &&image) = 0;
  virtual std::string GetZip() = 0;
  virtual void Clear() = 0;
};
