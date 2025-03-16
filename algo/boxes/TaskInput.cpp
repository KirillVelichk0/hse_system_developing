#include "TaskInput.hpp"

#include <algorithm>
#include <iterator>

bool ValidateInput(const InputData &input) noexcept {
  bool isOk = true;
  if (input.segmentsCount == 0 || input.pipelinesCount == 0 ||
      input.detailsCount == 0) {
    isOk = false;
  } else if (input.transportCosts.size() != input.segmentsCount ||
             input.pipelinesMoveCosts.size() != input.segmentsCount) {
    isOk = false;
  } else {
    for (const auto &pipelineCosts : input.pipelinesMoveCosts) {
      if (pipelineCosts.size() != input.segmentsCount) {
        isOk = false;
      }
    }
  }
  return isOk;
}

InputData GetInput(std::istream &inputStream) {
  InputData result;
  inputStream >> result.pipelinesCount >> result.segmentsCount >>
      result.detailsCount;
  auto FillCosts = [&inputStream](std::vector<std::uint64_t> &costsCont,
                                  auto count) {
    std::copy_n(std::istream_iterator<std::uint64_t>(inputStream), count,
                std::back_inserter(costsCont));
  };
  FillCosts(result.transportCosts, result.segmentsCount);
  for (std::uint32_t i = 0; i < result.pipelinesCount; i++) {
    auto &costsCont = result.pipelinesMoveCosts.emplace_back();
    FillCosts(costsCont, result.segmentsCount);
  }
  return result;
}
