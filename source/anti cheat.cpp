#include <windows.h>
#include <memoryapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <memory>
#include <iostream>
#include <algorithm>
#include <thread>
#include <wchar.h>
#include "vmprotect.h"
#include "util.h"

#pragma warning (disable : 4996)

using namespace std;

void* get_module_data(HANDLE process_handle, const std::wstring module_name)
{
	auto loaded_modules = std::make_unique<HMODULE[]>(64);
	DWORD loaded_module_sz = 0;

	// enumerate all modules by handle, using size of 512 since the required size is in bytes, and an HMODULE is 8 bytes large.
	if (!EnumProcessModules(process_handle, loaded_modules.get(), 512, &loaded_module_sz))
	{
		return {};
	}

	for (auto i = 0u; i < loaded_module_sz / 8u; i++)
	{
		wchar_t file_name[MAX_PATH] = L"";

		// get the full working path for the current module
		if (!GetModuleFileNameExW(process_handle, loaded_modules.get()[i], file_name, _countof(file_name)))
			continue;

		// module name returned will be a full path, check only for file name sub string.
		if (wcsstr(file_name, module_name.data()) != nullptr)
			return loaded_modules.get()[i];
	}

	return {};
} 

class rainbow_memory {
public:
	DWORD process_id = 0;
	HANDLE process_handle;
	LPCVOID module_base = 0;

	//read memory
	template <class T>
	T read(uintptr_t address) {
		T buffer{};
		ReadProcessMemory(process_handle, reinterpret_cast<void*>(address), &buffer, sizeof(T), nullptr);
		return buffer;
	}

	//check feature, defalt params are nice af
	bool check_feature(byte curr_val, bool unlock = false, int i = 0) {
		if (!unlock) return curr_val;

		else {
			//if this byte is here there not using unlockall
			if (i == 0 && curr_val == 0x34) 
 				return false;

			//cheater detectedddddd
			else if (i == 0 && curr_val == 0x41)
				return true;
		}
	}
};
rainbow_memory* mem = new rainbow_memory;


void main_thread() {

	while (true) {
		DWORD aProcesses[1024], cbNeeded, cProcesses; unsigned int i;

		if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
			return;

		cProcesses = cbNeeded / sizeof(DWORD);

		for (i = 0; i < cProcesses; i++) {
			if (aProcesses[i] != 0)
			{
				util.print_proc_info(aProcesses[i]);
			}
		}

		Sleep(5000);
	}
}

void screenshot_thread() {

	srand(time(NULL));

	while (true) {

		util.screenshot.take();

		int sleep_ammount = rand() % 60000;

		if (sleep_ammount > 500) {
			Sleep(sleep_ammount);
			printf("sleep %i\n", sleep_ammount);
		}

	}
}

int main() {

	if (!VMProtectIsValidImageCRC()) CRASH;

    GetWindowThreadProcessId(FindWindowW(L"R6Game", nullptr), &mem->process_id); //get proc id of rainbow by window name, then store that value

    mem->process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mem->process_id); //create handle, this allows us to read memory, cannot use online, 
    //printf("Handle: 0x%p\n", mem->process_handle); //BattlEye will instantly strip the handle

    mem->module_base = get_module_data(mem->process_handle, L"RainbowSix"); //now get the base of the game since we now have a handle
	//printf("Module base: 0x%p\n", mem->module_base); //print base

	const auto hthread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main_thread, 0, 0, 0); //create thread

	const auto pthread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)screenshot_thread, 0, 0, 0); //create thread

	if (hthread != NULL) WaitForSingleObject(hthread, INFINITE); 
	else printf("Failed to create the main thread, aborting!\n");

	if (pthread != NULL) WaitForSingleObject(pthread, INFINITE);
	else printf("Failed to create the screenshot thread, aborting!\n");

	if (!VMProtectIsValidImageCRC()) CRASH;

}
