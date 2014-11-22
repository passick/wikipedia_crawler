#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

class Downloader
{
  static const std::string downloader_program;
  static const std::string downloader_program_options;
  static const std::string downloader_program_to_file_option;

 public:
  static int DownloadPage(const std::string& link,
      const std::string& filename);
};

#endif
