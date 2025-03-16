#include "CalculateDP.hpp"
#include "TaskInput.hpp"

#include <optional>

std::uint64_t CalculateSimple(InputData &data) {
  std::uint64_t cost = 0;
  std::uint64_t curPipeId = 0; // на начальной стадии без разницы

  for (std::uint64_t i = 0; i < data.segmentsCount; i++) {
    std::optional<std::uint64_t> curOptimal;
    std::uint64_t optimalId = 0;

    for (std::uint64_t j = 0; j < data.pipelinesCount; j++) {
      const std::uint64_t diff =
          data.pipelinesMoveCosts[j][i] + GetMoveCost(i, j, curPipeId, data);
      if (!curOptimal || curOptimal.value() > diff) {
        curOptimal = diff;
        optimalId = j;
      }
    }
    cost += curOptimal.value();
    curPipeId = optimalId;
  }
  return cost;
}

int main(int argc, char *argv[]) {
  InputData input;
  input.pipelinesCount = 5;
  input.segmentsCount = 4;
  input.detailsCount = 1;
  input.transportCosts = {1, 2, 3, 4};
  input.pipelinesMoveCosts = {{1, 1, 10, 1},
                              {2, 7, 20, 9},
                              {7, 7, 7, 7},
                              {3, 2, 11, 4},
                              {5, 100, 5, 100}};
  std::cout << CalculateTimeV2(input) << std::endl;
}
