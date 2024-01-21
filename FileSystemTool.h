#pragma once
#include <vector>
#include <string>
#include <filesystem>

using namespace std::experimental::filesystem::v1;

class CFileSystemTool
{
public:
	static void GetArrayOfFiles(std::string const&, std::vector<std::wstring>&);
};