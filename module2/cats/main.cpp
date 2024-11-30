#include <Executor/BaseExecutor.hpp>
#include <Executor/StrandExecutor.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
int main(int argc, char *argv[]) {
  auto base = BaseExecutor::Create(8);
  auto strandExec = StrandExecutor::Create(base);
  return 0;
}
