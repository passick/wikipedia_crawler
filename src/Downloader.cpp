#include <cstdlib>
#include <string>
#include "Downloader.h"

const std::string Downloader::downloader_program = "wget";
const std::string Downloader::downloader_program_options = "--quiet";
const std::string Downloader::downloader_program_to_file_option = "--output-document=";

int Downloader::DownloadPage(const std::string& link,
    const std::string& filename)
{
  return system((downloader_program + " " + downloader_program_options +
        " '" + link + "' " +
        downloader_program_to_file_option + "'" + filename + "'").c_str());
}
