#pragma once
#include "CLI/ManagedObject.h"
#include <AES/AES.h>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace CLI::AES {

	public ref class AES {
	public:
		static String^ Encrypt(String^ data, String^ password);
		static String^ Decrypt(String^ hex_data, String^ password);

		static String^ BytesToHex(String^ str);
		static String^ HexToBytes(String^ str);
	};

}