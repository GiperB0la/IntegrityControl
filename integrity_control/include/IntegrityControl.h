#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "../include/Test.h"


class IntegrityControl
{
public:
	IntegrityControl(const std::wstring& directoryPath, bool isTesting);
	~IntegrityControl();

public:
	void start();

private:
	void monitorChanges();
	void monitorChanges_test();
	void modifyFile(const std::wstring& fileName, int count);

private:
	bool isTesting;
	std::wstring directoryPath;
	HANDLE directoryHandle = nullptr;

private:
	Test test;
	std::vector<std::wstring> files_test;
};