#pragma once
#include "Wires.h"

#include <memory>
#include <string>

namespace Encryption {

	class Reflector {
	public:
		Reflector(const std::string& name, const std::string& inputs, const std::string& outputs);
		void ProcessSignal(char& letter) const;

		inline const std::string& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		std::shared_ptr<Wires> m_Wires;
	};

}