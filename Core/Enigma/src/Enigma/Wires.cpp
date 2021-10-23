#include "Wires.h"

namespace Encryption {

	Wires::Wires(const std::string& inputs, const std::string& outputs) {
		m_Wires.resize(std::min(inputs.length(), outputs.length()));

		for (int i = 0; i < m_Wires.size(); i++) {
			m_Wires[i] = { inputs[i], outputs[i] };
		}
	}


	void Wires::SwapInputs(int index1, int index2) {
		int size = static_cast<int>(m_Wires.size());

		if (index1 >= 0 && index1 < size && index2 >= 0 && index2 < size) {
			std::swap(m_Wires[index1].Input, m_Wires[index2].Input);
		}
		else {
			throw "Index out of range!";
		}
	}

	void Wires::SwapOutputs(int index1, int index2) {
		int size = static_cast<int>(m_Wires.size());

		if (index1 >= 0 && index1 < size && index2 >= 0 && index2 < size) {
			std::swap(m_Wires[index1].Output, m_Wires[index2].Output);
		}
		else {
			throw "Index out of range!";
		}
	}


	int Wires::FindInput(char input) const {
		for (int i = 0; i < m_Wires.size(); i++) {
			if (m_Wires[i].Input == input) {
				return i;
			}
		}

		return -1;
	}

	int Wires::FindOutput(char output) const {
		for (int i = 0; i < m_Wires.size(); i++) {
			if (m_Wires[i].Output == output) {
				return i;
			}
		}

		return -1;
	}


	char Wires::GetInput(int index) const {
		int size = static_cast<int>(m_Wires.size());

		if (index >= 0 && index < size) {
			return m_Wires[index].Input;
		}
		else {
			throw "Index out of range!";
		}
	}

	char Wires::GetOutput(int index) const {
		int size = static_cast<int>(m_Wires.size());

		if (index >= 0 && index < size) {
			return m_Wires[index].Output;
		}
		else {
			throw "Index out of range!";
		}
	}

}