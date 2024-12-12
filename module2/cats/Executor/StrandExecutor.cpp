#include <Executor/StrandExecutor.hpp>
#include <atomic>

StrandExecutor::StrandExecutor(std::shared_ptr<IResourceProvider> provider)
    : m_provider(provider),
      m_strand(boost::asio::make_strand(provider->GetExecutor())) {}

StrandExecutor::~StrandExecutor() { Stop(); }

void StrandExecutor::Stop() { m_isRun.store(false, std::memory_order_release); }

std::shared_ptr<StrandExecutor>
StrandExecutor::Create(std::shared_ptr<IResourceProvider> provider) {
  if (provider == nullptr) {
    throw std::invalid_argument("BaseExecutor cant be nullptr");
  }
  return std::shared_ptr<StrandExecutor>(new StrandExecutor(provider));
}

void StrandExecutor::AddTask(std::function<void(asio::any_io_executor)> task) {
  if (task == nullptr) {
    throw std::invalid_argument("Task cant be nullptr");
  }
  if (!m_isRun.load(std::memory_order_acquire)) {
    throw std::runtime_error("StrandExecutor stopped");
  }
  auto safeTask = [weakSelf = weak_from_this(), task = std::move(task)]() {
    auto self = weakSelf.lock();
    if (self == nullptr) {
      return;
    }
    task(self->m_strand);
  };
  boost::asio::post(m_strand, std::move(safeTask));
}
