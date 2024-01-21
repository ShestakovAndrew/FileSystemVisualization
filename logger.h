#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

const std::string SEARCH_FILES_DATA = "bytes_file.txt";
const std::string ERROR_FILE_READ = "error_files_read.txt";
const std::string TIME_FILE = "time_file.txt";

inline void Logger(std::string const& file, std::string const& logMsg)
{
    std::ofstream ofs(file, std::ios_base::out | std::ios_base::app);
    ofs << logMsg << '\n';
    ofs.close();
}