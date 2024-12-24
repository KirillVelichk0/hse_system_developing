#pragma once
#include <Cats/IRegistry.hpp>
#include <filesystem>
#include <set>

class UniqueRegistry : public IRegistry {
public:
  explicit UniqueRegistry(
      const std::filesystem::path &dir = std::filesystem::current_path());
  ~UniqueRegistry() override;
  bool TryToSave(std::string &&image) override;
  void Clear() override;
  std::string GetZip() override;
  std::uint32_t GetSize() const override;
  void Reset() override;

private:
  std::set<std::size_t> m_hashes;
  const std::filesystem::path m_path;
  const std::filesystem::path m_imagesPath;
  const std::filesystem::path m_zipFilePath;
};
