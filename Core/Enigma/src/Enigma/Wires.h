#pragma once
#include <vector>
#include <string>

namespace Encryption {

	class Wires {
	public:
		Wires(const std::string& inputs, const std::string& outputs);

		void SwapInputs(int index1, int index2);
		void SwapOutputs(int index1, int index2);

		int FindInput(char input) const;
		int FindOutput(char output) const;

		char GetInput(int index) const;
		char GetOutput(int index) const;

		inline size_t GetCount() const { return m_Wires.size(); }
	private:
		struct Wire {
			char Input;
			char Output;
		};

		std::vector<Wire> m_Wires;
	};

}