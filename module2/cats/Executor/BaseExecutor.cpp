#include <Executor/BaseExecutor.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/post.hpp>

BaseExecutor::BaseExecutor(std::size_t poolSize) {

  auto threadRunner = [this]() { m_service.run(); };
  for (std::size_t i = 0; i < poolSize; i++) {
    m_group.create_thread(threadRunner);
  }
}

std::shared_ptr<BaseExecutor> BaseExecutor::Create(std::size_t poolSize) {
  return std::shared_ptr<BaseExecutor>(new BaseExecutor(poolSize));
}

void BaseExecutor::Stop() {
  m_group.interrupt_all();
  m_service.stop();
  m_group.join_all();
}

BaseExecutor::~BaseExecutor() { Stop(); }

boost::asio::executor BaseExecutor::GetExecutor() {
  return m_service.get_executor();
}
