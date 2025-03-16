#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

struct InputData {
  std::uint64_t segmentsCount;
  std::uint64_t pipelinesCount;
  std::uint64_t detailsCount;
  std::vector<std::uint64_t>
      transportCosts; // Стоимость переноса между конвейерами
  std::vector<std::vector<std::uint64_t>> pipelinesMoveCosts;
};

[[nodiscard]] bool ValidateInput(const InputData &input) noexcept;

InputData GetInput(std::istream &inputStream);
