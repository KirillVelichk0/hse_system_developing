#include "CalculateDP.hpp"
#include "TaskInput.hpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <optional>
#include <random>

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

InputData GenerateInput(std::uint64_t segmentsCount,
                        std::uint64_t pipelinesCount) {
  InputData input;
  input.pipelinesCount = pipelinesCount;
  input.segmentsCount = segmentsCount;
  input.detailsCount = 1;
  input.transportCosts.resize(segmentsCount);
  std::iota(input.transportCosts.begin(), input.transportCosts.end(), 100);
  std::mt19937 rand(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<> distrib(40, 100);
  for (std::uint64_t i = 0; i < pipelinesCount; i++) {
    auto &pipeline = input.pipelinesMoveCosts.emplace_back(segmentsCount);
    for (std::uint64_t j = 0; j < segmentsCount; j++) {
      pipeline[j] = distrib(rand);
    }
  }
  return input;
}

template <class Functor, class... Args>
auto WriteTime(std::string_view name, Functor &&functor, Args &&...args) {
  auto start = std::chrono::high_resolution_clock().now();
  auto result = functor(std::forward<Args>(args)...);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << name << " " << result << " With time: " << (end - start).count()
            << std::endl;
}

int main(int argc, char *argv[]) {
  // InputData input;
  // input.pipelinesCount = 5;
  // input.segmentsCount = 4;
  // input.detailsCount = 1;
  // input.transportCosts = {1, 2, 3, 4};
  // input.pipelinesMoveCosts = {{1, 1, 10, 1},
  //                             {2, 7, 20, 9},
  //                             {7, 7, 7, 7},
  //                             {3, 2, 11, 4},
  //                             {5, 100, 5, 100}};
  auto input = GenerateInput(500, 2);
  std::cout << "WTF\n";
  WriteTime("DP", &CalculateTimeV2, input);
  WriteTime("greed", &CalculateSimple, input);
  // std::cout << CalculateTimeV2(input) << std::endl;
  // std::cout << CalculateSimple(input) << std::endl;
}
