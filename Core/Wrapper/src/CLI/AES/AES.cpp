#include "AES.h"

namespace CLI::AES {

	String^ AES::Encrypt(String^ data, String^ password) {
		std::string result = Encryption::AES::Encrypt(ConvertTo<std::string>(data), ConvertTo<std::string>(password));
		return ConvertTo<String^>(result);
	}

	String^ AES::Decrypt(String^ hex_data, String^ password) {
		try {
			std::string result = Encryption::AES::Decrypt(ConvertTo<std::string>(hex_data), ConvertTo<std::string>(password));
			return ConvertTo<String^>(result);
		}
		catch (const char* error) {
			throw gcnew Exception(ConvertTo<String^>(error));
		}
	}


	String^ AES::BytesToHex(String^ str) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(Marshal::StringToHGlobalAnsi(str).ToPointer());

		std::string result = Encryption::AES::BytesToHex(ptr, str->Length);
		return ConvertTo<String^>(result);
	}

	String^ AES::HexToBytes(String^ str) {
		std::string result = Encryption::AES::HexToBytes(ConvertTo<std::string>(str));
		return ConvertTo<String^>(result);
	}

}