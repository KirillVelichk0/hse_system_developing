#include "CalculateDP.hpp"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <optional>
#include <unordered_map>

namespace {
/*!
 * \brief Многомерный индекс + кумулятивная сумма в последнем элементе
 * \warning Фактический отсчет отдельного элемента индекса начинается с 1, т.к.
 * 0 позиция ввиду алгоритма занята "отсутствием движения"
 * */
using MultiIndex = std::vector<std::uint64_t>;
auto printValue = [](auto value) {
  // std::cout << "Pipeline: " << value.first << ", sum: " << value.second
  // << std::endl;
};
auto indexPrinterGlob = [](auto &index, const std::string &indexName,
                           auto currentIndexPart) {
  // std::cout << indexName << ", index data: ";
  for (int i = 0; i < index.size() - 1; i++) {
    // std::cout << index[i] << " ";
  }
  // std::cout << "Cum sum: " << index.back()
  // << ", current index part: " << currentIndexPart << std::endl;
};
struct ValueData {
  std::unordered_map<std::uint64_t, std::uint64_t> elems;
  std::uint64_t minVal;
  std::uint64_t minI;
};
using Value = std::optional<std::unordered_map<
    std::uint64_t, std::uint64_t>>; // ключ - индекс пайпы, значение - Текущая
                                    // оптимальная стоимость переезда из i-1 в i

struct MultiIndexHash {
  std::size_t operator()(const MultiIndex &multi) const {
    std::size_t hash = 0;
    for (std::uint64_t i = 0; i < multi.size() - 1; i++) {
      auto &index = multi[i];
      hash ^= std::hash<std::uint64_t>()(index) ^ std::hash<std::uint64_t>()(i);
    }
    return hash;
  }
};

class Table final {
public:
  Table(const InputData &input)
      : m_segmentsCount(input.segmentsCount),
        m_pipelinesCount(input.pipelinesCount) {
    if (m_segmentsCount == 0) {
      throw std::invalid_argument("SegmentsCount count cant be 0");
    }
    if (m_pipelinesCount == 0) {
      throw std::invalid_argument("Pipelines count cant be 0");
    }
    this->InitStartTableValues(input);
  }
  ~Table() = default;

  [[nodiscard]] Value &operator[](const MultiIndex &index) {
    if (!ValidateIndex(index)) {
      throw std::invalid_argument("Uncorrect index");
    }
    return m_table[index];
  }

  [[nodiscard]] const Value &operator[](const MultiIndex &index) const {
    if (!ValidateIndex(index)) {
      throw std::invalid_argument("Uncorrect index");
    }
    return m_table.at(index);
  }

private:
  bool ValidateIndex(const MultiIndex &index) const {
    return index.back() <= m_segmentsCount;
  }
  void InitStartTableValues(const InputData &input) {
    std::vector<std::uint64_t> multiIndex(m_pipelinesCount + 1, 0);
    multiIndex.back() =
        1; // кумулятивная сумма = 1 для прединициализированных значений
    for (std::uint64_t i = 0; i < m_pipelinesCount; i++) {
      multiIndex[i] = 1;
      std::unordered_map<std::uint64_t, std::uint64_t> elem;
      elem.insert({i, input.pipelinesMoveCosts[i].at(0)});
      m_table[multiIndex] = std::move(elem);
      multiIndex[i] = 0;
    }
  }

private:
  std::unordered_map<MultiIndex, Value, MultiIndexHash> m_table;
  const std::uint64_t m_segmentsCount;
  const std::uint64_t m_pipelinesCount;
};

void AddMultiIndexToSet(const MultiIndex &currentMultiIndex,
                        std::vector<MultiIndex> &multiIndexSet,
                        const std::uint64_t currentIndexPart,
                        const std::uint64_t segmentsCount) {
  const auto curLim = segmentsCount - currentMultiIndex.back();
  auto indexPrinter = [currentIndexPart](auto &index,
                                         const std::string &indexName) {
    // std::cout << indexName << ", index data: ";
    // for (int i = 0; i < index.size() - 1; i++) {
    // std::cout << index[i] << " ";
    // }
    // std::cout << "Cum sum: " << index.back()
    // << ", current index part: " << currentIndexPart << std::endl;
  };
  if (currentMultiIndex.back() == segmentsCount) {
    multiIndexSet.push_back(currentMultiIndex);
    // indexPrinter(currentMultiIndex, "Added multiindex");
    return;
  }
  if (currentIndexPart == currentMultiIndex.size() - 1) {
    return;
  }
  auto actualMultiIndex = currentMultiIndex;
  for (std::uint64_t count = 0;
       count <= curLim && actualMultiIndex.back() <= segmentsCount;
       count++, actualMultiIndex.back()++) {
    actualMultiIndex[currentIndexPart] = count;
    // indexPrinter(actualMultiIndex, "Candidat multiindex");
    AddMultiIndexToSet(actualMultiIndex, multiIndexSet, currentIndexPart + 1,
                       segmentsCount);
  }
}

std::vector<MultiIndex>
GetInitialIndexesSet(const std::uint64_t pipelinesCount,
                     const std::uint64_t segmentsCount) {
  std::vector<MultiIndex> result;
  MultiIndex initialMultiIndex(pipelinesCount + 1, 0);
  AddMultiIndexToSet(initialMultiIndex, result, 0, segmentsCount);
  return result;
}

// Задача напоминает задачу о распределении S шариков по n урнам. Сложность -
// число сочетаний с повторениями.
// Если число сегментов много больше числа
// конвейеров, то сложность примерно segmentsCount ^ pipeLinesCount (для
// удобства считаем число сегментов фиксированным).
// Если число конвейеров много больше числа сегментов, то получаем аналогичное
// pipeLinesCount ^ segmentsCount.
// Если и то и то велико, то получаем сложность, аналогичную экспоненциальной

Value CalculateRecursive(const MultiIndex &currentMultiIndex, Table &table,
                         const InputData &input) {
  if (table[currentMultiIndex].has_value()) {
    return table[currentMultiIndex].value();
  }
  auto nextMultiIndex = currentMultiIndex;

  Value optimalValue;
  // pipeLinesCount
  for (std::uint64_t i = 0; i < currentMultiIndex.size() - 1; i++) {
    indexPrinterGlob(nextMultiIndex, "NextIndex", i);
    auto &curSubIndex = nextMultiIndex[i];
    if (curSubIndex == 0) {
      continue;
    }
    curSubIndex--;
    nextMultiIndex.back()--;

    auto prevValue = CalculateRecursive(
        nextMultiIndex, table, input); // глубина - не более segmentsCount

    // Сложность - pipelinesCount
    auto calculatePotential = [i, &input, &nextMultiIndex](auto &&prevValue) {
      auto potentialValue = std::move(prevValue);
      // для нулевого элемента во входных данных в мультииндексе будет
      // соответствовать первый элемент. соответственно, нужно вычесть из
      // стоимости переезда во входных данных единицу, что уже было сделано в
      // curSubIndex--
      for (auto &elem : potentialValue.value()) {
        elem.second +=
            input.pipelinesMoveCosts[elem.first][nextMultiIndex.back()] +
            GetMoveCost(nextMultiIndex.back(), i, elem.first, input);
      }

      return potentialValue;
    };
    auto potentialValue = calculatePotential(std::move(prevValue));
    if (!optimalValue.has_value()) {
      optimalValue = potentialValue;

    } else {
      auto &optimalV = optimalValue.value();
      for (auto &elem : potentialValue.value()) {
        auto optimalIt = optimalV.find(elem.first);
        if (optimalIt == optimalV.end() || optimalIt->second > elem.second) {
          optimalV[elem.first] = elem.second;
        }
        printValue(*optimalV.find(elem.first));
      }
    }
    curSubIndex++;
    nextMultiIndex.back()++;
  }
  if (!optimalValue.has_value()) {
    throw std::runtime_error("Critical error in calculate algorithm");
  }
  table[currentMultiIndex] = optimalValue;
  return optimalValue;
}

} // namespace

/*
 * Алгоритм:
 *
 * - стоимость (i1, i2, ..., ik) элемента оценивается как минимальная из
 * стоимостей (..., id - 1, ..., ik) для d[0;k], где стоимость рассчитывается
 * как стоимость проезда по конвейеру + стоимость потенциального переноса
 *
 * - таблица при создании уже содержит инициализированные значения для индексов
 * (..., id, ...) для всех d[0;1], где id = 1, а остальные индексы = 0
 *
 * - в том случае, если значение еще неинициализировано, алгоритм пытается
 * рекурсивно определить стоимость
 *
 * - алгоритм определяет пул индексов, лежащих на ограничивающей плоскости
 * индексов по правилу(i + j+ k + ... = N), где N - количество сегментов, а
 * элементы - количество шагов на k-ом конвейере
 * */

std::uint64_t CalculateTimeV2(const InputData &input) {
  Table table(input);
  auto indexedSet = GetInitialIndexesSet(
      input.pipelinesCount,
      input.segmentsCount); // число сочетаний с повторениями. По идее, т.к.
                            // значения сохраняются, данная сложность будет не
                            // домножаться, а суммироваться
  std::optional<std::uint64_t> optimalValue;
  int LastPipe;
  for (auto &initialIndex : indexedSet) {
    int i = 0;
    auto potentialValue = CalculateRecursive(initialIndex, table, input);
    for (auto &elem : potentialValue.value()) {
      if (!optimalValue || elem.second < optimalValue.value()) {
        optimalValue = elem.second;
        LastPipe = i;
      }
      i++;
    }
  }
  if (!optimalValue.has_value()) {
    throw std::runtime_error("Critical error in calculate algorithm");
  }
  // std::cout << "Last pipe " << LastPipe << std::endl;

  return optimalValue.value();
}

uint64_t GetMoveCost(std::uint64_t segmentIndex,
                     const std::uint64_t currentPipeIndex,
                     std::uint64_t nextPipeIndex, const InputData &input) {
  // Так как в условии задачи не сказано иного, считаем, что деталь может начать
  // свой путь на любом из конвейеров
  auto printCost = [&](bool isSpec) {
    // std::cout << "AT1\n";
    auto val = input.transportCosts.at(segmentIndex);
    if (isSpec) {
      val = 0;
    }
    // std::cout << "Cost: " << val << ", segment index: " << segmentIndex
    // << ", curPipeIndex: " << currentPipeIndex
    // << ", nextPipeIndex: " << nextPipeIndex << std::endl;
  };
  if (segmentIndex == 0) {
    // printCost(true);
    return 0;
  }
  if (currentPipeIndex == nextPipeIndex) {
    // printCost(true);
    return 0;
  }
  // std::cout << "AT2\n";
  // std::cout << "Cost: " << input.transportCosts.at(nextPipeIndex);
  // printCost(false);
  return input.transportCosts.at(segmentIndex);
}
