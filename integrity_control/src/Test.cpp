#include "../include/Test.h"


void Test::createFiles(const std::vector<size_t>& sizes, const std::wstring& directory, std::vector<std::wstring>& files)
{
    std::wcout << "Files are being created for the test..." << std::endl;

    if (GetFileAttributesW(directory.c_str()) == INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryW(directory.c_str(), nullptr);
    }

    for (const auto& size : sizes) {
        std::wstring fileName = directory + L"/file_" + std::to_wstring(size) + L".bin";
        files.push_back(L"file_" + std::to_wstring(size) + L".bin");

        if (GetFileAttributesW(fileName.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wcout << L"File already exists: " << fileName << std::endl;
            continue;
        }

        std::wofstream ofs(fileName, std::ios::binary);
        if (!ofs) {
            std::wcerr << L"Error: Unable to create file " << fileName << std::endl;
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

    std::wcout << "The files for the test have been created." << std::endl;
    std::wcout << "Total file size: " << input_data_size << " MB = " << input_data_size / 1024 << " GB." << std::endl;
    std::wcout << "-----------------------------------------------------------------------------------------" << std::endl;
    std::wcout << "For testing, delete the first file named 'file_1048576.bin' from the 'Test' folder." << std::endl;
    std::wcout << "After running the test in the file 'stats.txt ' information about the completion time of\n5 tests for each file of different sizes will be saved." << std::endl;
    std::wcout << "-----------------------------------------------------------------------------------------" << std::endl;
}

void Test::recordEventStatistics(const std::wstring& file_name, DWORD event_, double averageEventTimeout)
{
    std::wofstream statsFile(L"stats.txt", std::ios::app);
    if (!statsFile.is_open()) {
        std::wcerr << "Error: Unable to open stats file." << std::endl;
        return;
    }

    static bool header = true;
    if (header) {
        statsFile << std::endl << L"*************************************************************************" << std::endl
            << std::endl << std::left << std::setw(30) << L"File name" << L"| "
            << std::setw(30) << L"Event" << L"| "
            << std::setw(10) << L"Time" << std::endl;
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
        std::wcerr << "Error: Unable to open stats file." << std::endl;
        return;
    }

    static bool header_test = true;
    if (header_test) {
        statsFile << std::left << std::setw(30) << L"Size" << L"| "
            << std::setw(30) << L"File name" << L"| "
            << std::setw(30) << L"Event" << L"| "
            << std::setw(10) << L"Time (average of 5 tests)" << std::endl;
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