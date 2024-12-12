#include <Client/Client.hpp>
#include <Executor/BaseExecutor.hpp>
#include <Executor/StrandExecutor.hpp>
#include <iostream>
int main(int argc, char *argv[]) {
  auto base = BaseExecutor::Create(8);
  auto strandExec = StrandExecutor::Create(base);
  auto client = Client::Create(strandExec);
  return 0;
}
