#include "AES.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <hex.h>
#include <modes.h>
#include <osrng.h>
#include <pwdbased.h>

#define SALT_SIZE 16
#define KEY_SIZE 32
#define IV_SIZE 16

#define HEX_SALT_SIZE 32
#define HEX_IV_SIZE 32

#define HEX_HASHED_PASSWORD_SIZE 64
#define HEX_CHECKSUM_SIZE 128

using namespace CryptoPP;

namespace Encryption {

	enum class FORMAT_TYPE {
		NONE = 0,
		ENCRYPT, DECRYPT
	};

	static std::string FormatHexEncodedString(std::string hex_data, FORMAT_TYPE type) {
		switch (type) {
			case FORMAT_TYPE::ENCRYPT: {
				std::stringstream ss;

				if (hex_data.size() % 2 != 0) {
					throw "Invalid \"hex_data\" parameter!";
				}

				for (int i = 0; i < hex_data.size(); i += 2) {
					if (i != 0 && i % 32 == 0) {
						ss << '\n';
					}

					ss << hex_data[i] << hex_data[i + 1] << ' ';
				}

				return ss.str();
			}
			case FORMAT_TYPE::DECRYPT: {
				hex_data.erase(std::remove(hex_data.begin(), hex_data.end(), ' '), hex_data.end());
				hex_data.erase(std::remove(hex_data.begin(), hex_data.end(), '\n'), hex_data.end());

				if (hex_data.size() % 2 != 0) {
					throw "Invalid \"hex_data\" parameter!";
				}

				return hex_data;
			}
		}

		throw "Unknown format type!";
	}


	std::string AES::Encrypt(const std::string& data, const std::string& password) {
		SecByteBlock salt(SALT_SIZE);	// 128-bit salt
		SecByteBlock key(KEY_SIZE);		// 256-bit key
		SecByteBlock iv(IV_SIZE);		// 128-bit iv

		if (data.size() == 0) {
			throw "Invalid \"data\" parameter!";
		}
		else if (password.size() == 0) {
			throw "Invalid \"password\" parameter!";
		}

		// Generate 128-bit salt and 128-bit iv
		AutoSeededRandomPool rng;
		rng.GenerateBlock(salt, salt.size());
		rng.GenerateBlock(iv, iv.size());

		// Generate 256-bit key
		PKCS5_PBKDF2_HMAC<SHA256> pbkdf2;
		pbkdf2.DeriveKey(key, key.size(), 0, (byte*)password.data(), password.size(), salt, salt.size(), 10000);

		// Init AES in CBC mode
		CBC_Mode<CryptoPP::AES>::Encryption encryption;
		encryption.SetKeyWithIV(key, key.size(), iv, iv.size());

		// Init HMAC
		HMAC<SHA256> hmac;
		hmac.SetKey(key, key.size());

		// Encrypt data
		std::string encrypted_data;
		StringSource ss1(data, true, new StreamTransformationFilter(encryption, new StringSink(encrypted_data)));

		// Hash password (256-bit)
		std::string hashed_password;
		StringSource ss2("TOP-SECRET-STRING-VARIABLE", true, new HashFilter(hmac, new StringSink(hashed_password)));

		// Convert (salt, iv) to hexadecimal
		std::string encoded_salt = BytesToHex(salt, salt.size());
		std::string encoded_iv = BytesToHex(iv, iv.size());

		// Convert (encrypted_data, hashed_password) to hexadecimal
		std::string encoded_data = BytesToHex((byte*)&encrypted_data[0], encrypted_data.size());
		std::string encoded_hashed_password = BytesToHex((byte*)&hashed_password[0], hashed_password.size());;

		// Concatenate hex encoded strings
		std::string encoded_salt_iv_data_password = encoded_salt + encoded_iv + encoded_data + encoded_hashed_password;

		// Create SHA-512 checksum
		std::string checksum = CalculateChecksum(encoded_salt_iv_data_password);

		// Format hex_data
		std::string hex_data = FormatHexEncodedString(encoded_salt_iv_data_password + checksum, FORMAT_TYPE::ENCRYPT);

		return hex_data;
	}

	std::string AES::Decrypt(const std::string& hex_data, const std::string& password) {
		SecByteBlock salt(SALT_SIZE);	// 128-bit salt
		SecByteBlock key(KEY_SIZE);		// 256-bit key
		SecByteBlock iv(IV_SIZE);		// 128-bit iv

		if (hex_data.size() == 0) {
			throw "Invalid \"hex_data\" parameter!";
		}
		else if (password.size() == 0) {
			throw "Invalid \"password\" parameter!";
		}

		// Format hex_data
		std::string formatted_hex_data = FormatHexEncodedString(hex_data, FORMAT_TYPE::DECRYPT);

		// Calculate encoded_data size
		int encoded_salt_iv_password_checksum_size = HEX_SALT_SIZE + HEX_IV_SIZE + HEX_HASHED_PASSWORD_SIZE + HEX_CHECKSUM_SIZE;
		int encoded_data_size = static_cast<int>(formatted_hex_data.size()) - encoded_salt_iv_password_checksum_size;

		// Split formatted_hex_data
		std::string encoded_salt_iv_data_password = formatted_hex_data.substr(0, encoded_data_size + HEX_SALT_SIZE + HEX_IV_SIZE + HEX_HASHED_PASSWORD_SIZE);
		std::string prev_checksum = formatted_hex_data.substr(encoded_data_size + HEX_SALT_SIZE + HEX_IV_SIZE + HEX_HASHED_PASSWORD_SIZE, HEX_CHECKSUM_SIZE);

		// Calculate SHA-512 checksum
		std::string curr_checksum = CalculateChecksum(encoded_salt_iv_data_password);

		// Verify hex_encoded integrity
		if (prev_checksum != curr_checksum) {
			throw "Corrupted data!";
		}

		// Split encoded_salt_iv_data_password
		std::string encoded_salt = formatted_hex_data.substr(0, HEX_SALT_SIZE);
		std::string encoded_iv = formatted_hex_data.substr(HEX_SALT_SIZE, HEX_IV_SIZE);

		std::string encoded_data = formatted_hex_data.substr(HEX_SALT_SIZE + HEX_IV_SIZE, encoded_data_size);
		std::string encoded_hashed_password = formatted_hex_data.substr(encoded_data_size + HEX_SALT_SIZE + HEX_IV_SIZE, HEX_HASHED_PASSWORD_SIZE);

		// Convert (encoded_salt, encoded_iv) to bytes
		std::string decoded_salt = HexToBytes(encoded_salt);
		std::string decoded_iv = HexToBytes(encoded_iv);

		// Convert (encoded_data, encoded_hashed_password) to bytes
		std::string decoded_data = HexToBytes(encoded_data);
		std::string decoded_hashed_password = HexToBytes(encoded_hashed_password);

		// Set 128-bit salt and 128-bit iv
		salt.Assign((byte*)decoded_salt.data(), decoded_salt.size());
		iv.Assign((byte*)decoded_iv.data(), decoded_iv.size());

		// Generate 256-bit key
		PKCS5_PBKDF2_HMAC<SHA256> pbkdf2;
		pbkdf2.DeriveKey(key, key.size(), 0, (byte*)password.data(), password.size(), salt, salt.size(), 10000);

		// Init HMAC
		HMAC<SHA256> hmac;
		hmac.SetKey(key, key.size());

		// Hash password (256-bit)
		std::string hashed_password;
		StringSource ss1("TOP-SECRET-STRING-VARIABLE", true, new HashFilter(hmac, new StringSink(hashed_password)));

		// Verify password
		if (decoded_hashed_password != hashed_password) {
			throw "Invalid password!";
		}

		// Init AES in CBC mode
		CBC_Mode<CryptoPP::AES>::Decryption decryption;
		decryption.SetKeyWithIV(key, key.size(), iv, iv.size());

		// Decrypt data
		std::string decrypted_data;
		StringSource ss2(decoded_data, true, new StreamTransformationFilter(decryption, new StringSink(decrypted_data)));

		return decrypted_data;
	}

	std::string AES::CalculateChecksum(const std::string& encoded_string) {
		SecByteBlock data(64);
		HexEncoder encoder;

		SHA512 sha;
		sha.CalculateDigest(data, (byte*)encoded_string.data(), encoded_string.size());

		std::string checksum;
		encoder.Attach(new StringSink(checksum));
		encoder.Put(data, data.size());
		encoder.MessageEnd();

		return checksum;
	}


	std::string AES::BytesToHex(uint8_t* bytes, size_t size) {
		HexEncoder encoder;

		std::string hex;
		encoder.Attach(new StringSink(hex));
		encoder.Put(bytes, size);
		encoder.MessageEnd();

		return hex;
	}

	std::string AES::HexToBytes(const std::string& hex) {
		HexDecoder decoder;

		std::string bytes;
		decoder.Attach(new StringSink(bytes));
		decoder.Put((byte*)hex.data(), hex.size());
		decoder.MessageEnd();

		return bytes;
	}

}