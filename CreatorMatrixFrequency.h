#pragma once
#include <array>

using FrequencyMatrix = std::array<std::array<double, 256>, 256>;

class CCreatorMatrixFrequency
{
public:
	CCreatorMatrixFrequency();

	void FillFrequencyMatrix();
	void NormolizeFrequencyMatrix();
	void CreatePictureFrequency();
	void ReadMatrixFromFile(std::string const&);

private:
	FrequencyMatrix m_frequencyMatrix{};
};