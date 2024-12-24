#include <Executor/BaseExecutor.hpp>
#include <boost/asio/post.hpp>
#include <iostream>

BaseExecutor::BaseExecutor(std::size_t poolSize)
    : m_guard(boost::asio::make_work_guard(m_service)) {

  auto threadRunner = [this]() { m_service.run(); };
  for (std::size_t i = 0; i < poolSize; i++) {
    m_group.create_thread(threadRunner);
  }
}

std::shared_ptr<BaseExecutor> BaseExecutor::Create(std::size_t poolSize) {
  return std::shared_ptr<BaseExecutor>(new BaseExecutor(poolSize));
}

void BaseExecutor::AddTask(std::function<void(asio::any_io_executor)> task) {
  if (task == nullptr) {
    throw std::invalid_argument("Task cant be nullptr");
  }
  boost::asio::post(
      m_service, [task = std::move(task),
                  executor = m_service.get_executor()]() { task(executor); });
}

void BaseExecutor::Stop() {
  m_guard.reset();
  m_group.interrupt_all();
  m_service.stop();
}

BaseExecutor::~BaseExecutor() {
  Stop();
  m_group.join_all();
}

boost::asio::any_io_executor BaseExecutor::GetExecutor() {
  return m_service.get_executor();
}
