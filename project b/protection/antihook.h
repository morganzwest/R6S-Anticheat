#pragma once
#include <vector>
#include <ctime>
#include <Windows.h>
#include <string>
#include <memory>
#include <cassert>
#include <iostream>
#include <algorithm>
#include "vmprotect.h"
#include "aes.h"
#include "aes.c"


#define SHELL_CODE_HASH_SIZE 16
#define SHITHOOK_DEBUGGING 0

extern int __cdecl main(int argc, char** argv);

#define CRASH exit(0)

namespace antihook
{
	class secure_function
	{
	public:
		secure_function(void* addr);
		secure_function(void* addr, std::size_t hash);
		void encrypt();
		void decrypt();
		void hash_function();
		void set_hash(std::size_t hash);
		void set_address(std::uintptr_t addr);
		std::size_t get_hash() const;
		std::uintptr_t get_addr() const;

	private:
		bool is_encrypted = false;
		void generate_key();
		void generate_salt();
		AES_ctx ctx;

		uint8_t hash[16];
		uint8_t addr[16];
		uint8_t key[16];
		uint8_t salt[16];
	};

	std::size_t hash_function(void* addr);

	static std::vector<secure_function> secure_functions
	{

		secure_function(&DeleteDC),
		secure_function(&SelectObject),
		secure_function(&SelectObject),
		secure_function(&CreateCompatibleBitmap),
		secure_function(&GetDeviceCaps),
		secure_function(&CreateCompatibleDC),
		secure_function(&GetSystemMetrics),
		secure_function(&OpenFile),
		secure_function(&LoadLibrary),
		secure_function(&LoadLibraryW),
		secure_function(&CreateFile),
		secure_function(&ReadFile),
		secure_function(&WriteFile),
		secure_function(&TerminateProcess),
		secure_function(&ExitProcess),
		secure_function(&GetProcAddress),
		secure_function(&GetModuleHandle),
		secure_function(&memcpy),
		secure_function(&IsDebuggerPresent),
		secure_function(&GetModuleHandleW),
		secure_function(&GlobalAlloc),
		secure_function(&GlobalLock),
		secure_function(&GlobalUnlock),
		secure_function(&QueryPerformanceCounter),
		secure_function(&QueryPerformanceFrequency),
		secure_function(&WaitForSingleObjectEx),
		secure_function(&UpdateWindow),
		secure_function(&GetKeyState),
		secure_function(&malloc),
		secure_function(&memset),
		secure_function(&memcpy),
		secure_function(&memmove),
		secure_function(&OpenProcess),
		secure_function(&DeleteFile),
		secure_function(&DeleteFileA),
		secure_function(&GetCurrentProcessId),
		secure_function(&ShowWindow),
		secure_function(&EnumWindows),
		secure_function(&GetCurrentProcess),
		secure_function(&DeviceIoControl),

		/* vmp functions */
		secure_function(&VMProtectIsValidImageCRC),
		secure_function(&VMProtectIsDebuggerPresent),

		/* undocumented exported windows functions */
		secure_function(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtOpenFile")),
		secure_function(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtOpenProcess")),
		secure_function(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtReadFile")),
		secure_function(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtLoadDriver")),

		/* AES functions */
		secure_function(&AES_ECB_decrypt),
		secure_function(&AES_ECB_encrypt),
		secure_function(&AES_init_ctx),
		secure_function(&AES_init_ctx_iv)
	};

	__forceinline std::size_t hash_function(void* addr)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (!addr)
			return NULL;

		uint8_t buffer[SHELL_CODE_HASH_SIZE];
		memcpy(buffer, addr, SHELL_CODE_HASH_SIZE);
		buffer[SHELL_CODE_HASH_SIZE - 1] = 0x0;
		return std::hash<std::string>{}(std::string((char*)buffer));
	}

	__forceinline void add(void* addr)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (!addr)
			return;
		auto secure_func = secure_function(addr);
		secure_func.hash_function();
		secure_func.encrypt();
		secure_functions.push_back(std::move(secure_func));
	}

	__forceinline void add(secure_function& secure_func)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		secure_functions.push_back(secure_func);
	}

	__forceinline void remove(void* addr)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (!addr)
			return;

		auto result = std::find_if(
			secure_functions.begin(),
			secure_functions.end(),
			[addr](secure_function& secure_func) -> bool {
				secure_func.decrypt();
				if (secure_func.get_addr() == (std::uintptr_t)addr)
				{
					secure_func.encrypt();
					return true;
				}
				secure_func.encrypt();
				return false;
			}
		);
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (result != secure_functions.end())
			secure_functions.erase(result);
	}

	__forceinline void remove(std::size_t hash)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (!hash)
			return;

		auto result = std::find_if(
			secure_functions.begin(),
			secure_functions.end(),
			[hash](secure_function& secure_func) -> bool {
				secure_func.decrypt();
				if (secure_func.get_hash() == hash)
				{
					secure_func.encrypt();
					return true;
				}
				secure_func.encrypt();
				return false;
			}
		);

		if (result != secure_functions.end())
			secure_functions.erase(result);
	}

	__forceinline bool valid(void* addr)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		if (!addr)
			return false;

		for (auto secure_func = secure_functions.begin();
			secure_func != secure_functions.end(); ++secure_func)
		{
			secure_func->decrypt();
			if (secure_func->get_addr() == (std::uintptr_t)addr)
			{
				if (secure_func->get_hash() == hash_function((void*)secure_func->get_addr()))
				{
					secure_func->encrypt();
					return true;
				}
				else
				{
					secure_func->encrypt();
					return false;
				}
			}
			secure_func->encrypt();
		}
		return false;
	}

	__forceinline bool valid()
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		for (auto secure_func = secure_functions.begin();
			secure_func != secure_functions.end(); ++secure_func)
		{
			secure_func->decrypt();
			if (secure_func->get_hash() !=
				hash_function((void*)secure_func->get_addr()))
			{
				secure_func->encrypt();
				return false;
			}
			secure_func->encrypt();
		}
		return true;
	}

	__forceinline std::size_t get_hash(void* addr)
	{
		if (!VMProtectIsValidImageCRC()) CRASH;
		auto result = std::find_if(secure_functions.begin(), secure_functions.end(),
			[addr](secure_function& secure_func) -> bool {
				secure_func.decrypt();
				bool result = secure_func.get_addr() == (std::uintptr_t)addr;
				secure_func.encrypt();
				return result;
			});

		if (result == secure_functions.end())
			return NULL;
		else
		{
			result->decrypt();
			std::size_t temp_hash = result->get_hash();
			result->encrypt();
			return temp_hash;
		}
	}
}
