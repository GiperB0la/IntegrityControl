#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>
#include "../include/IntegrityControl.h"


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::wstring directoryPath = L"Controlled folder";

    bool isTesting = false;
    std::wcout << L"[?] Do you want to run a performance test? (1 - Yes / 0 - No): ";
    std::cin >> isTesting;

    if (isTesting) {
        directoryPath = L"Test";
        std::wcout << L"\nStarting performance test..." << std::endl;
        std::wcout << L"Files of various sizes will be created in the 'Test' folder." << std::endl;
        std::wcout << L"The program will measure how long it takes to analyze changes in each file." << std::endl;
        std::wcout << L"The results will be saved to the 'stats.txt' file." << std::endl;
        std::wcout << L"---------------------------------------------" << std::endl;

        if (!std::filesystem::exists(directoryPath)) {
            std::filesystem::create_directories(directoryPath);
        }
    }
    else {
        if (argc == 2) {
            directoryPath = std::filesystem::path(argv[1]).wstring();
        }

        if (!std::filesystem::exists(directoryPath)) {
            if (!std::filesystem::create_directory(directoryPath)) {
                std::wcerr << L"[-] Error: Failed to create the directory." << std::endl;
                return 1;
            }
        }
        else if (!std::filesystem::is_directory(directoryPath)) {
            std::wcerr << L"[-] Error: The specified path exists but is not a directory." << std::endl;
            return 1;
        }

        std::wcout << L"Monitoring directory: " << std::filesystem::absolute(directoryPath).wstring() << std::endl;
        std::wcout << L"---------------------------------------------------------------" << std::endl;
        std::wcout << L"The folder '" << directoryPath << L"' is now under control." << std::endl;
    }

    IntegrityControl icontrol(directoryPath, isTesting);
    icontrol.start();

    return 0;
}