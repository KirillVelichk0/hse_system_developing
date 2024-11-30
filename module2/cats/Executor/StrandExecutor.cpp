#include <Executor/StrandExecutor.hpp>

StrandExecutor::StrandExecutor(std::shared_ptr<BaseExecutor> base)
    : m_base(base), m_strand(boost::asio::make_strand(base->GetService())) {}

StrandExecutor::~StrandExecutor() { Stop(); }

void StrandExecutor::Stop() { m_base->Stop(); }

std::shared_ptr<StrandExecutor>
StrandExecutor::Create(std::shared_ptr<BaseExecutor> base) {
  if (base == nullptr) {
    throw std::invalid_argument("BaseExecutor cant be nullptr");
  }
  return std::shared_ptr<StrandExecutor>(new StrandExecutor(base));
}

void StrandExecutor::AddTask(std::function<void()> task) {
  if (task == nullptr) {
    throw std::invalid_argument("Task cant be nullptr");
  }
  boost::asio::post(m_strand, task);
}
