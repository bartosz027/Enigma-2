#include "Reflector.h"

namespace Encryption {

	Reflector::Reflector(const std::string& name, const std::string& inputs, const std::string& outputs) : 
		m_Name(name), 
		m_Wires(std::make_shared<Wires>(inputs, outputs)) {
	}


	void Reflector::ProcessSignal(char& letter) const {
		int index = m_Wires->FindInput(letter);
		letter = m_Wires->GetOutput(index);
	}

}