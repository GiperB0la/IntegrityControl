#include "../include/IntegrityControl.h"


IntegrityControl::IntegrityControl(const std::wstring& directoryPath, bool isTesting) :
	directoryPath(directoryPath), isTesting(isTesting)
{
    directoryHandle = CreateFileW(
        directoryPath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    if (directoryHandle == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[-] Ошибка: Не удается открыть дескриптор каталога." << std::endl;
    }
}

IntegrityControl::~IntegrityControl()
{
    if (directoryHandle != nullptr) {
        CloseHandle(directoryHandle);
    }
}

void IntegrityControl::start()
{
    if (isTesting) {
        monitorChanges_test();
    }
    else {
        bool hasFiles = false;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                if (!hasFiles) {
                    std::wcout << L"Папка содержит файлы:" << std::endl;
                    hasFiles = true;
                }
                std::wcout << entry.path().wstring() << std::endl;
            }
        }

        if (hasFiles) {
            std::wcout << L"-----------------------------------------------------------------------------------------" << std::endl;
        }
    }

    monitorChanges();
}

void IntegrityControl::modifyFile(const std::wstring& fileName, int count)
{
    std::wofstream file(directoryPath + L"\\" + fileName, std::ios::app);
    if (file.is_open()) {
        file << L"Изменение " << count << L" " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        file.close();
    }
    else {
        std::wcerr << L"[-] Ошибка: Не удается открыть файл для внесения изменений." << std::endl;
    }
}

void IntegrityControl::monitorChanges() 
{
    constexpr int bufferSize = 8172;
    char buffer[bufferSize];
    DWORD bytesReturned;
    OVERLAPPED overlapped = { 0 };

    while (true) {
        if (ReadDirectoryChangesW(
            directoryHandle,
            &buffer,
            bufferSize,
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            &overlapped,
            nullptr
        )) {
            FILE_NOTIFY_INFORMATION* notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

            do {
                std::wstring wideFileName(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
                auto start = std::chrono::high_resolution_clock::now();

                switch (notifyInfo->Action) {
                case FILE_ACTION_ADDED:
                    std::wcout << L"STRUCTURE_CHANGED_ADDED: " << wideFileName << std::endl;
                    break;
                case FILE_ACTION_REMOVED:
                    std::wcout << L"STRUCTURE_CHANGED_REMOVED: " << wideFileName << std::endl;
                    break;
                case FILE_ACTION_MODIFIED:
                    std::wcout << L"CONTENT_CHANGED: " << wideFileName << std::endl;
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    std::wcout << L"STRUCTURE_CHANGED_RENAMED_FROM: " << wideFileName << std::endl;
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    std::wcout << L"STRUCTURE_CHANGED_RENAMED_TO: " << wideFileName << std::endl;
                    break;
                default:
                    std::wcout << L"Unknown action: " << notifyInfo->Action << std::endl;
                    break;
                }

                if (isTesting) {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = end - start;
                    double averageEventTimeout = elapsed.count();

                    test.recordEventStatistics(wideFileName, notifyInfo->Action, averageEventTimeout);
                }

                if (notifyInfo->NextEntryOffset != 0) {
                    notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(notifyInfo) + notifyInfo->NextEntryOffset);
                }
                else {
                    break;
                }
            } while (true);
        }
        else {
            std::wcerr << L"[-] Ошибка: Не удалось отследить изменения в каталоге." << std::endl;
            break;
        }
    }
}

void IntegrityControl::monitorChanges_test()
{
    if (isTesting) {
        constexpr int bufferSize = 8172;
        char buffer[bufferSize];
        DWORD bytesReturned;
        OVERLAPPED overlapped = { 0 };

        std::vector<size_t> sizes = {
            1 * 1024 * 1024,   // 1 MB
            10 * 1024 * 1024,  // 10 MB
            100 * 1024 * 1024, // 100 MB
            500 * 1024 * 1024, // 500 MB
            1LL * 1024 * 1024 * 1024,  // 1 GB
            2LL * 1024 * 1024 * 1024,  // 2 GB
            5LL * 1024 * 1024 * 1024,  // 5 GB
            10LL * 1024 * 1024 * 1024, // 10 GB
            20LL * 1024 * 1024 * 1024  // 20 GB
        };

        test.createFiles(sizes, L"Test", files_test);

        std::wstring file_name;
        DWORD event_ = 0;
        int size = 0;

        for (const auto& file : files_test) {
            double totalDuration = 0.0;
            for (int i = 0; i < 5; ++i) {
                auto start = std::chrono::high_resolution_clock::now();

                modifyFile(file, i);

                if (ReadDirectoryChangesW(
                    directoryHandle,
                    &buffer,
                    bufferSize,
                    TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
                    &bytesReturned,
                    &overlapped,
                    nullptr
                )) {
                    FILE_NOTIFY_INFORMATION* notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
                    event_ = notifyInfo->Action;

                    do {
                        std::wstring wideFileName(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
                        std::wcout << L"Обнаруженное изменение: " << wideFileName << std::endl;
                        file_name = wideFileName;

                        if (notifyInfo->NextEntryOffset != 0) {
                            notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                                reinterpret_cast<BYTE*>(notifyInfo) + notifyInfo->NextEntryOffset);
                        }
                        else {
                            break;
                        }
                    } while (true);
                }
                else {
                    std::wcerr << L"[-] Ошибка: Не удалось отследить изменения в каталоге." << std::endl;
                    break;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;

                totalDuration += elapsed.count();
            }

            double averageTime = totalDuration / 5.0;
            std::wcout << L"Среднее время мониторинга файла " << file << L": " << averageTime << L" секунд" << std::endl;
            test.recordEventStatistics_test(file_name, event_, averageTime, size);
            size++;
        }

        std::wcout << L"Проверьте файл 'stats.txt '." << std::endl;
        std::wcout << L"-----------------------------------------------------------------------------------------" << std::endl;
    }
}