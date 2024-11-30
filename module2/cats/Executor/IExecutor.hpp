#pragma once
#include <functional>

class IExecutor {
public:
  virtual ~IExecutor();
  virtual void AddTask(std::function<void()> task) = 0;
  virtual void Stop() = 0;
};
