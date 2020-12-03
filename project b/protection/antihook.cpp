#include "antihook.h"



namespace antihook
{

	secure_function::secure_function(void* addr)
	{
		std::cout << "Adding address to secure_function list: " <<
			std::hex << std::showbase << (uint64_t)addr << std::endl;

		set_address((uintptr_t)addr);
		generate_key();
		generate_salt();
		hash_function();
		AES_init_ctx_iv(&ctx, key, salt);
	}


	secure_function::secure_function(void* addr, std::size_t hash)
	{
		std::cout << "Adding address and hash to secure_function list: " <<
			std::hex << std::showbase << (uint64_t)addr << " (Hash) " << std::hex << hash << std::endl;

		assert(addr and hash);
		set_address((uintptr_t)addr);
		set_hash(hash);
		generate_key();
		generate_salt();
		AES_init_ctx_iv(&ctx, key, salt);
		encrypt();
	}

	void secure_function::encrypt()
	{
		if (is_encrypted)
			return;

		AES_ECB_encrypt(&ctx, (std::uint8_t*)addr);
		AES_ECB_encrypt(&ctx, (std::uint8_t*)hash);
		is_encrypted = true;
	}

	void secure_function::decrypt()
	{
		if (!is_encrypted)
			return;

		AES_ECB_decrypt(&ctx, (std::uint8_t*)addr);
		AES_ECB_decrypt(&ctx, (std::uint8_t*)hash);
		is_encrypted = false;
	}

	void secure_function::hash_function()
	{
		if (*(std::uintptr_t*)addr == NULL)
			return;

		std::cout << "Hashing function at: " << std::hex << std::showbase << (uint64_t)addr << std::endl;

		uint8_t buffer[SHELL_CODE_HASH_SIZE];
		memcpy(buffer, (void*)*(std::uintptr_t*)addr, SHELL_CODE_HASH_SIZE);
		buffer[SHELL_CODE_HASH_SIZE - 1] = 0x0;
		std::size_t temp_hash =
			std::hash<std::string>{}(std::string((char*)buffer));
		memcpy(hash, &temp_hash, sizeof(temp_hash));
	}

	std::size_t secure_function::get_hash() const
	{
		if (!hash)
			return NULL;
		return *(std::size_t*)hash;
	}

	void secure_function::generate_key()
	{
		std::srand(std::time(nullptr));
		for (auto iteration{ 0u }; iteration < 4; ++iteration)
			*(unsigned*)(&key + 0x4 * iteration) = std::rand();
	}

	void secure_function::generate_salt()
	{
		std::srand(std::time(nullptr));
		for (auto iteration{ 0u }; iteration < 4; ++iteration)
			*(unsigned*)((&salt) + (0x4 * iteration)) = std::rand();
	}

	std::uintptr_t secure_function::get_addr() const
	{
		if (!addr)
			return NULL;
		return *(std::uintptr_t*) addr;
	}


	void secure_function::set_address(std::uintptr_t addr)
	{
		std::cout << "Setting address of secure_function to: " << std::hex << addr << std::endl;

		std::srand(std::time(nullptr));
		*(std::uintptr_t*)this->addr = addr;
		*(std::uintptr_t*)(this->addr + 0x8) = std::rand();
		hash_function();
	}


	void secure_function::set_hash(std::size_t hash)
	{
		std::cout << "Setting hash of secure_function (Address) " <<
			std::hex << addr << " (Hash)" << std::hex << hash << std::endl;

		std::srand(std::time(nullptr));
		*(std::size_t*)this->hash = hash;
		*(std::size_t*)(this->hash + 0x8) = std::rand();
	}
}

