#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>

#include "CreatorMatrixFrequency.h"
#include "logger.h"

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	CCreatorMatrixFrequency creatorMatrixFrequency;

	std::ifstream filledDataFile(SEARCH_FILES_DATA);

	filledDataFile 
		? creatorMatrixFrequency.ReadMatrixFromFile(SEARCH_FILES_DATA)
		: creatorMatrixFrequency.FillFrequencyMatrix();

	creatorMatrixFrequency.NormolizeFrequencyMatrix();
	creatorMatrixFrequency.CreatePictureFrequency();

	return 0;
}