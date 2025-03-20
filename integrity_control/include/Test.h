#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>


enum Events
{
	STRUCTURE_CHANGED_ADDED = 0x00000001,
	STRUCTURE_CHANGED_REMOVED = 0x00000002,
	CONTENT_CHANGED = 0x00000003,
	STRUCTURE_CHANGED_RENAMED_FROM = 0x00000004,
	STRUCTURE_CHANGED_RENAMED_TO = 0x00000005
};

class Test
{
public:
	Test() = default;
	~Test() = default;

public:
	void createFiles(const std::vector<size_t>& sizes, const std::wstring& directory, std::vector<std::wstring>& files);
	void recordEventStatistics(const std::wstring& file_name, DWORD event_, double averageEventTimeout);
	void recordEventStatistics_test(const std::wstring& file_name, DWORD event_, double averageEventTimeout, int count);

private:
	std::wstring eventToString(DWORD event);
	std::wstring sizeToString(int size);
};