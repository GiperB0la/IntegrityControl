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
    std::wcout << L"[?] Хотите запустить тест? (1 - Да / 0 - Нет): ";
    std::cin >> isTesting;

    if (isTesting) {
        directoryPath = L"Test";
        std::wcout << L"\nЗапуск теста производительности..." << std::endl;
        std::wcout << L"В папке 'Test' будут созданы файлы различных размеров." << std::endl;
        std::wcout << L"Программа измерит время анализа изменений файлов." << std::endl;
        std::wcout << L"Результаты теста будут сохранены в 'stats.txt'." << std::endl;
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
                std::wcerr << L"[-] Ошибка: Не удалось создать каталог." << std::endl;
                return 1;
            }
        }
        else if (!std::filesystem::is_directory(directoryPath)) {
            std::wcerr << L"[-] Ошибка: Указанный путь существует, но не является каталогом." << std::endl;
            return 1;
        }

        std::wcout << L"Мониторинг папки: " << std::filesystem::absolute(directoryPath).wstring() << std::endl;
        std::wcout << L"---------------------------------------------------------------" << std::endl;
        std::wcout << L"Папка '" << directoryPath << L"' теперь находится под контролем." << std::endl;
    }

    IntegrityControl icontrol(directoryPath, isTesting);
    icontrol.start();

	return 0;
}