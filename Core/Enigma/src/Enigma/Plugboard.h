#pragma once
#include "Wires.h"

#include <memory>
#include <string>

namespace Encryption {

	class Plugboard {
	public:
		Plugboard();

		void Connect(const std::string& plugs);
		void Disconnect(const std::string& plugs);

		void ProcessSignal(char& letter) const;
		void Reset();

		std::string GetConnectedPlugs() const;
	private:
		bool AreConnected(const std::string& plugs) const;
	private:
		std::shared_ptr<Wires> m_Wires;
	};

}