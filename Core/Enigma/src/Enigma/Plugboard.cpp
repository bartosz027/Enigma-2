#include "Plugboard.h"

namespace Encryption {

	Plugboard::Plugboard() :
		m_Wires(std::make_shared<Wires>("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ")) {
	}


	void Plugboard::Connect(const std::string& plugs) {
		if (plugs.length() < 2) {
			throw "Invalid parameter!";
		}

		if (!AreConnected(plugs)) {
			int index1 = m_Wires->FindOutput(plugs[0]);
			int index2 = m_Wires->FindOutput(plugs[1]);

			m_Wires->SwapOutputs(index1, index2);
		}
	}

	void Plugboard::Disconnect(const std::string& plugs) {
		if (plugs.length() < 2) {
			throw "Invalid parameter!";
		}

		if (AreConnected(plugs)) {
			int index1 = m_Wires->FindOutput(plugs[0]);
			int index2 = m_Wires->FindOutput(plugs[1]);

			m_Wires->SwapOutputs(index1, index2);
		}
	}

	bool Plugboard::AreConnected(const std::string& plugs) const {
		if (m_Wires->FindInput(plugs[0]) == m_Wires->FindOutput(plugs[1]) && m_Wires->FindInput(plugs[1]) == m_Wires->FindOutput(plugs[0])) {
			return true;
		}
		else if (m_Wires->FindInput(plugs[0]) != m_Wires->FindOutput(plugs[0]) || m_Wires->FindInput(plugs[1]) != m_Wires->FindOutput(plugs[1])) {
			return true;
		}
		else {
			return false;
		}
	}


	void Plugboard::ProcessSignal(char& letter) const {
		int index = m_Wires->FindInput(letter);
		letter = m_Wires->GetOutput(index); 
	}


	void Plugboard::Reset() {
		m_Wires = std::make_shared<Wires>("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	}


	std::string Plugboard::GetConnectedPlugs() const {
		std::string plugs = "";

		for (int i = 0; i < m_Wires->GetCount(); i++) {
			char input = m_Wires->GetInput(i);
			char output = m_Wires->GetOutput(i);

			if (input < output) {
				plugs += input;
				plugs += output;
			}
		}

		return plugs;
	}

}