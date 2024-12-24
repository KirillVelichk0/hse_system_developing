#include <Cats/Cats.hpp>

CatsProcessor::CatsProcessor(std::shared_ptr<StrandExecutor> executor,
                             std::unique_ptr<IRegistry> registry)
    : m_exec(std::move(executor)), m_registry(std::move(registry)) {
  if (m_exec == nullptr || m_registry == nullptr) {
    throw std::invalid_argument("Uncorrect params passed");
  }
}

std::shared_ptr<CatsProcessor>
CatsProcessor::Create(std::shared_ptr<StrandExecutor> executor,
                      std::unique_ptr<IRegistry> registry) {
  return std::shared_ptr<CatsProcessor>(
      new CatsProcessor(std::move(executor), std::move(registry)));
}

CatsProcessor::ProcessingStatus
CatsProcessor::ProcessNewImage(std::string &&data) {
  std::cout << "Start processing" << std::endl;
  const auto isSaved = m_registry->TryToSave(std::move(data));
  const auto curSize = m_registry->GetSize();
  if (!isSaved) {
    return {curSize, false, {}};
  }
  ProcessingStatus result;
  result.processedCount = curSize;
  result.imageAdd = true;
  if (curSize == 12) {
    result.zipBody = std::move(m_registry->GetZip());
    m_registry->Clear();
    m_exec->Stop();
  }
  return result;
}

CallbackType CatsProcessor::CreateNewImageProcessor() {
  return CreateDefaultWrapper(
      [weakSelf = weak_from_this()](std::string &&body) {
        auto self = weakSelf.lock();
        if (self == nullptr) {
          std::cout << "Processor is stop" << std::endl;
          return;
          // throw std::runtime_error("Processor is stop");
        }
        try {
          self->m_exec->AddTask(
              [weakSelf = std::move(weakSelf), body = std::move(body)](
                  [[maybe_unused]] boost::asio::any_io_executor exec) mutable {
                auto self = weakSelf.lock();
                if (self == nullptr) {
                  std::cout << "Processor strand is stop" << std::endl;
                  return;
                  // throw std::runtime_error("Processor strand is stop");
                }
                auto processingStatus = self->ProcessNewImage(std::move(body));
                if (self->m_statusObserver != nullptr) {
                  self->m_statusObserver(std::move(processingStatus));
                }
              });
        } catch (std::exception &ex) {
          std::cout << ex.what() << std::endl;
        }
      });
}
