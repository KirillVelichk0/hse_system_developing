#pragma once
#include "TaskInput.hpp"

std::uint64_t CalculateTimeV2(const InputData &input);

/* !
 * \brief Получение стоимости переноса детали между конвейерами
 * \details Получение стоимости переноса детали между конвейрами. Передается
 * индекс сегмента, на котором деталь ТОЛЬКО НАЧНЕТ обрабатываться
 * */
uint64_t GetMoveCost(std::uint64_t segmentIndex,
                     const std::uint64_t currentPipeIndex,
                     std::uint64_t nextPipeIndex, const InputData &input);
