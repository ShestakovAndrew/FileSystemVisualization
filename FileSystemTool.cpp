#include "FileSystemTool.h"
#include "logger.h"

struct HumanReadable {
    std::uintmax_t size{};
private: friend
    std::ostream& operator<<(std::ostream& os, HumanReadable hr) {
        int i{};
        double mantissa = static_cast<double>(hr.size);
        for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
        mantissa = std::ceil(mantissa * 10.) / 10.;
        os << mantissa << "BKMGTPE"[i];
        return i == 0 ? os : os << "B";
    }
};

void CFileSystemTool::GetArrayOfFiles(std::string const& disk, std::vector<std::wstring>& result)
{
    auto start = std::chrono::steady_clock::now();
    std::cout << "Searching files in disk " + disk +  " ...";
    std::uintmax_t filesSize = 0;
    
    for (const auto& entry : recursive_directory_iterator(disk, directory_options::follow_directory_symlink))
    {
        try
        {
            if (!is_directory(entry))
            {
                filesSize += file_size(entry.path());
                result.push_back(entry.path().wstring());
            }
        }
        catch (std::exception const& e)
        {
            Logger(ERROR_FILE_READ, e.what());
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = end - start;

    std::cout << " OK - " << result.size() << " files, all size: " << HumanReadable{ filesSize } << "\n";
    Logger(TIME_FILE, "Поиск файлов завершён за " + std::to_string(elapsedSeconds.count()) + " секунд на диске " + disk);
}
