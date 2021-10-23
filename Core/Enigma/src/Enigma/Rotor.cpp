#include "Rotor.h"

namespace Encryption {

	Rotor::Rotor(const std::string& name, const std::string& inputs, const std::string& outputs, char turnoverPosition) : 
		m_Name(name),
		m_KeyPosition(0), 
		m_RingPosition(0), 
		m_TurnoverPosition(turnoverPosition),
		m_Wires(std::make_shared<Wires>(inputs, outputs)) {
	}


	void Rotor::Move() {
		if (m_KeyPosition < 25) {
			m_KeyPosition++;
		}
		else {
			m_KeyPosition = 0;
		}
	}


	void Rotor::ProcessSignal(char& letter) const {
		AddOffset(letter, m_KeyPosition - m_RingPosition);

		int index = m_Wires->FindInput(letter);
		letter = m_Wires->GetOutput(index);

		RemoveOffset(letter, m_KeyPosition - m_RingPosition);
	}

	void Rotor::ProcessReversedSignal(char& letter) const {
		AddOffset(letter, m_KeyPosition - m_RingPosition);

		int index = m_Wires->FindOutput(letter);
		letter = m_Wires->GetInput(index);

		RemoveOffset(letter, m_KeyPosition - m_RingPosition);
	}


	void Rotor::AddOffset(char& letter, int offset) const {
		letter += offset;

		if (letter < 'A') {
			letter += 26;
		}
		else if (letter > 'Z') {
			letter -= 26;
		}
	}

	void Rotor::RemoveOffset(char& letter, int offset) const {
		letter -= offset;

		if (letter < 'A') {
			letter += 26;
		}
		else if (letter > 'Z') {
			letter -= 26;
		}
	}

}