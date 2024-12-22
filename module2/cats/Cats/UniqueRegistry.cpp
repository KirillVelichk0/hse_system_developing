#include <Cats/UniqueRegistry.hpp>
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

namespace {
auto ConstructPath(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    throw std::invalid_argument("This path dont exist");
  }
  if (!std::filesystem::is_directory(path)) {
    throw std::invalid_argument("This is not dir");
  }
  return std::filesystem::absolute(path) / "cats";
}

auto GetHash(const std::string &data) { return std::hash<std::string>()(data); }

} // namespace

UniqueRegistry::UniqueRegistry(const std::filesystem::path &dir)
    : m_path(ConstructPath(dir)) {
  std::filesystem::create_directory(m_path);
}

UniqueRegistry::~UniqueRegistry() { this->Clear(); }

bool UniqueRegistry::TryToSave(std::string &&image) {
  auto hash = GetHash(image);
  if (m_hashes.find(hash) != m_hashes.end()) {
    return false;
  }
  m_hashes.insert(hash);
  cv::Mat rawData(1, image.size(), CV_8U, image.data());
  cv::Mat img = cv::imdecode(rawData, cv::IMREAD_COLOR);
  if (img.empty()) {
    throw std::runtime_error("Cant decode image");
  }
  auto name = std::to_string(m_hashes.size()) + ".jpg";
  auto fullPathToFile = m_path / name;
  cv::imwrite(fullPathToFile, img);
  return true;
}

void UniqueRegistry::Clear() {
  std::filesystem::remove_all(m_path);
  m_hashes.clear();
}

std::string UniqueRegistry::GetZip() {}
