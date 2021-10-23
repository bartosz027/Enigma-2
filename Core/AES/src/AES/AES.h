#pragma once
#include <string>

namespace Encryption {

	class AES {
	public:
		static std::string Encrypt(const std::string& data, const std::string& password);		// return encoded (salt + iv + encrypted_data + hashed_password + checksum) in hexadecimal
		static std::string Decrypt(const std::string& hex_data, const std::string& password);	// return decrypted_data

		static std::string BytesToHex(uint8_t* bytes, size_t size);
		static std::string HexToBytes(const std::string& hex);
	private:
		static std::string CalculateChecksum(const std::string& encoded_string);
	};

}