#include <Cats/UniqueRegistry.hpp>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <minizip/zip.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

namespace {

void ZipFolder(const std::string &folderPath, const std::string &zipFilePath) {
  namespace fs = std::filesystem;

  zipFile zf = zipOpen(zipFilePath.c_str(), APPEND_STATUS_CREATE);
  if (!zf) {
    throw std::runtime_error("Cant create archive");
  }

  for (const auto &entry : fs::recursive_directory_iterator(folderPath)) {
    if (fs::is_regular_file(entry.status())) {
      std::string relativePath =
          entry.path().lexically_relative(folderPath).string();
      zip_fileinfo fileInfo = {0};
      int err = zipOpenNewFileInZip(zf, relativePath.c_str(), &fileInfo,
                                    nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED,
                                    Z_BEST_COMPRESSION);
      if (err != ZIP_OK) {
        throw std::runtime_error("Cant add file to archive");
      }

      std::ifstream file(entry.path(), std::ios::binary);
      char buffer[4096];
      while (file.read(buffer, sizeof(buffer))) {
        zipWriteInFileInZip(zf, buffer, file.gcount());
      }
      zipCloseFileInZip(zf);
    }
  }

  zipClose(zf, nullptr);
}

auto ValidatePath(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    throw std::invalid_argument("This path dont exist");
  }
  if (!std::filesystem::is_directory(path)) {
    throw std::invalid_argument("This is not dir");
  }
  return std::filesystem::absolute(path);
}

auto GetHash(const std::string &data) { return std::hash<std::string>()(data); }

} // namespace

UniqueRegistry::UniqueRegistry(const std::filesystem::path &dir)
    : m_path(ValidatePath(dir)), m_imagesPath(m_path / "cats"),
      m_zipFilePath(m_path / "cats.zip") {
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
  std::filesystem::remove_all(m_imagesPath);
  std::filesystem::remove(m_zipFilePath);
  m_hashes.clear();
}

std::string UniqueRegistry::GetZip() {
  ZipFolder(m_imagesPath, m_zipFilePath);
  std::ifstream zipData(m_zipFilePath);
  return {std::istreambuf_iterator<char>(zipData),
          std::istreambuf_iterator<char>()};
}
