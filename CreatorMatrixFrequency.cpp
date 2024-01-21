#define NOMINMAX
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <random>
#include <windows.h>

#include "CreatorMatrixFrequency.h"
#include "FileSystemTool.h"
#include "logger.h"

#include "progressbar.hpp"
#include "SFML/Graphics.hpp"

using Byte = unsigned char;

std::vector<std::string> getListOfDrives()
{
    std::vector<std::string> arrayOfDrives;
    char* szDrives = new char[MAX_PATH]();
    GetLogicalDriveStringsA(MAX_PATH, szDrives);

    for (int i = 0; i < 100; i += 4)
    {
        if (szDrives[i] != (char)0)
        {
            arrayOfDrives.push_back(std::string{ szDrives[i], szDrives[i + 1],szDrives[i + 2] });
        }
    }
    delete[] szDrives;
    return arrayOfDrives;
}

CCreatorMatrixFrequency::CCreatorMatrixFrequency()
{
}

void CCreatorMatrixFrequency::FillFrequencyMatrix()
{
    CFileSystemTool fileSystemTool;
    std::vector<std::wstring> filesFromDisks;

    for (std::string currentDrive : getListOfDrives())
    {
        fileSystemTool.GetArrayOfFiles(currentDrive, filesFromDisks);
    }

    progressbar bar(filesFromDisks.size());
    auto start = std::chrono::steady_clock::now();

    for (std::wstring file : filesFromDisks)
    {
        try
        {
            Byte firstByte, secondByte;
            std::ifstream input(file, std::ios::binary);

            if ((file_size(file) > 2) and input)
            {
                firstByte = input.get();
                while (input.good())
                {
                    secondByte = input.get();
                    m_frequencyMatrix[firstByte][secondByte]++;
                    std::swap(firstByte, secondByte);
                }
            }
            bar.update();
        }
        catch (std::exception const& e)
        {
            Logger(ERROR_FILE_READ, e.what());
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    Logger(TIME_FILE, "Чтение файлов завершено за " + std::to_string(elapsed_seconds.count()) + " секунд.");
}

void CCreatorMatrixFrequency::NormolizeFrequencyMatrix()
{
    double sum = 0;

    for (unsigned i = 0; i < 256; i++)
    {
        for (unsigned j = 0; j < 256; j++)
        {
            Logger(SEARCH_FILES_DATA, std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(m_frequencyMatrix[i][j]));
            sum += m_frequencyMatrix[i][j];
        }
    }

    for (size_t i = 0; i < 256; i++)
    {
        for (size_t j = 0; j < 256; j++)
        {
            m_frequencyMatrix[i][j] /= sum;
            if (m_frequencyMatrix[i][j] > 0.001) m_frequencyMatrix[i][j] /= 100;
        }
    }
}

constexpr auto convert(double max, double min, double value) -> std::uint32_t
{
    constexpr std::uint32_t dst_min = 0;
    constexpr std::uint32_t dst_max = 255;
    
    auto normalized = static_cast<std::uint8_t>((dst_max - dst_min)
        * ((value - min) / (max - min))
        + dst_min);

    normalized = static_cast<uint8_t>(normalized * 1000);

    std::uint32_t color{};
    color |= static_cast<std::uint32_t>(normalized) << 8U;
    color |= static_cast<std::uint32_t>(normalized) << 16U;
    color |= static_cast<std::uint32_t>(normalized) << 24U;
    color |= static_cast<std::uint32_t>(255);

    return color;
}

void CCreatorMatrixFrequency::CreatePictureFrequency()
{
    sf::RenderWindow renderWindow(sf::VideoMode(256, 256), "3 Lab");
    sf::Event event;
    sf::Image image;
    image.create(256, 256, sf::Color::Black);

    double max = m_frequencyMatrix[0][1], min = m_frequencyMatrix[0][1];

    for (unsigned int x = 0; x < 256; ++x)
    {
        for (unsigned int y = 1; y < 256; ++y)
        {
            if (m_frequencyMatrix[x][y] < min) min = m_frequencyMatrix[x][y];
            if (m_frequencyMatrix[x][y] > max) max = m_frequencyMatrix[x][y];
        }
    }

    for (unsigned int x = 0; x < 256; ++x)
    {
        for (unsigned int y = 1; y < 256; ++y)
        {
            image.setPixel(x, y, sf::Color(convert(max, min, m_frequencyMatrix[x][y])));
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    while (renderWindow.isOpen())
    {
        while (renderWindow.pollEvent(event))
        {
            if (event.type == sf::Event::EventType::Closed)
            {
                renderWindow.close();
            }
        }

        renderWindow.clear();
        renderWindow.draw(sprite);
        renderWindow.display();
    }

    if (!image.saveToFile("result.png"))
    {
        Logger(ERROR_FILE_READ, "Картинка по матрице не была построена.");
    }
}

void CCreatorMatrixFrequency::ReadMatrixFromFile(std::string const& filePathWithMatrix)
{
    std::ifstream myfile;
    int row, col;
    float number;

    myfile.open(filePathWithMatrix);
    while (myfile >> row >> col >> number)
    {
        m_frequencyMatrix[row][col] = number;
    }
    myfile.close();
}
