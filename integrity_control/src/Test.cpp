#include "../include/Test.h"


void Test::createFiles(const std::vector<size_t>& sizes, const std::wstring& directory, std::vector<std::wstring>& files)
{
    std::wcout << L"Для тестирования создаются файлы..." << std::endl;

    if (GetFileAttributesW(directory.c_str()) == INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryW(directory.c_str(), nullptr);
    }

    for (const auto& size : sizes) {
        std::wstring fileName = directory + L"/file_" + std::to_wstring(size) + L".bin";
        files.push_back(L"file_" + std::to_wstring(size) + L".bin");

        if (GetFileAttributesW(fileName.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wcout << L"Файл уже существует: " << fileName << std::endl;
            continue;
        }

        std::wofstream ofs(fileName, std::ios::binary);
        if (!ofs) {
            std::wcerr << L"[-] Ошибка: Не удалось создать файл " << fileName << std::endl;
            continue;
        }

        ofs.seekp(size - 1);
        ofs.write(L"", 1);

        ofs.close();
    }

    size_t input_data_size = 0;
    for (const auto& s : sizes) {
        input_data_size += s;
    }
    input_data_size /= 1024 * 1024;

    std::wcout << L"Файлы для теста уже созданы." << std::endl;
    std::wcout << L"Общий размер файла: " << input_data_size << " MB = " << input_data_size / 1024 << " GB." << std::endl;
    std::wcout << L"-----------------------------------------------------------------------------------------" << std::endl;
    std::wcout << L"Для тестирования удалите первый файл с именем 'file_1048576.bin' из папки 'Test'." << std::endl;
    std::wcout << L"После запуска теста в файле 'stats.txt' будет сохранена информация о времени завершения\n5 тестов для каждого файла разного размера." << std::endl;
    std::wcout << L"-----------------------------------------------------------------------------------------" << std::endl;
}

void Test::recordEventStatistics(const std::wstring& file_name, DWORD event_, double averageEventTimeout)
{
    std::wofstream statsFile(L"stats.txt", std::ios::app);
    if (!statsFile.is_open()) {
        std::wcerr << L"[-] Ошибка: Не удается открыть файл статистики." << std::endl;
        return;
    }

    static bool header = true;
    if (header) {
        statsFile << std::endl << L"*************************************************************************" << std::endl
            << std::endl << std::left << std::setw(30) << L"Имя файла" << L"| "
            << std::setw(30) << L"Событие" << L"| "
            << std::setw(10) << L"Время" << std::endl;
        statsFile << L"-------------------------------------------------------------------------" << std::endl;
        header = false;
    }

    statsFile << std::left << std::setw(30) << file_name << L"| "
        << std::setw(30) << eventToString(event_) << L"| "
        << std::setw(10) << averageEventTimeout << std::endl;

    statsFile.close();
}

void Test::recordEventStatistics_test(const std::wstring& file_name, DWORD event_, double averageEventTimeout, int size)
{
    std::wofstream statsFile(L"stats.txt", std::ios::app);
    if (!statsFile.is_open()) {
        std::wcerr << L"[-]Ошибка: Не удается открыть файл статистики." << std::endl;
        return;
    }

    static bool header_test = true;
    if (header_test) {
        statsFile << std::left << std::setw(30) << L"Size" << L"| "
            << std::setw(30) << L"Имя файла" << L"| "
            << std::setw(30) << L"Событие" << L"| "
            << std::setw(10) << L"Время (в среднем за 5 тестов)" << std::endl;
        statsFile << L"-------------------------------------------------------------------------------------------------------------------------" << std::endl;
        header_test = false;
    }

    statsFile << std::left << std::setw(30) << sizeToString(size) << L"| "
        << std::setw(30) << file_name << L"| "
        << std::setw(30) << eventToString(event_) << L"| "
        << std::setw(10) << averageEventTimeout << std::endl;

    statsFile.close();
}

std::wstring Test::eventToString(DWORD event)
{
    switch (event) {
    case STRUCTURE_CHANGED_ADDED:
        return L"STRUCTURE_CHANGED_ADDED";
    case STRUCTURE_CHANGED_REMOVED:
        return L"STRUCTURE_CHANGED_REMOVED";
    case CONTENT_CHANGED:
        return L"CONTENT_CHANGED";
    case STRUCTURE_CHANGED_RENAMED_FROM:
        return L"STRUCTURE_CHANGED_RENAMED_FROM";
    case STRUCTURE_CHANGED_RENAMED_TO:
        return L"STRUCTURE_CHANGED_RENAMED_TO";
    default:
        return L"UNKNOWN_EVENT";
    }
}

std::wstring Test::sizeToString(int size)
{
    switch (size) {
    case 0:
        return L"1 MB";
    case 1:
        return L"10 MB";
    case 2:
        return L"100 MB";
    case 3:
        return L"500 MB";
    case 4:
        return L"1 GB";
    case 5:
        return L"2 GB";
    case 6:
        return L"5 GB";
    case 7:
        return L"10 GB";
    case 8:
        return L"20 GB";
    default:
        return L"UNKNOWN_EVENT";
    }
}