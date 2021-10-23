#pragma once
#include "Wires.h"

#include <memory>
#include <string>

namespace Encryption {

	class Rotor {
	public:
		Rotor(const std::string& name, const std::string& inputs, const std::string& outputs, char turnoverPosition);

		void Move();
		void ProcessSignal(char& letter) const;
		void ProcessReversedSignal(char& letter) const;

		inline const std::string& GetName() const { return m_Name; }
		inline char GetTurnoverPosition() const { return m_TurnoverPosition; }

		inline char GetKeyPosition() const { return m_KeyPosition + 'A'; }
		inline void SetKeyPosition(char position) { m_KeyPosition = ConvertToKeyPosition(position); }

		inline int  GetRingPosition() const { return m_RingPosition + 1; }
		inline void SetRingPosition(int position) { m_RingPosition = ConvertToRingPosition(position); };
	private:
		void AddOffset(char& letter, int offset) const;
		void RemoveOffset(char& letter, int offset) const;

		inline char ConvertToKeyPosition(char position) const { return position - 'A'; }
		inline int ConvertToRingPosition(int position) const { return position - 1; }
	private:
		std::string m_Name;
		std::shared_ptr<Wires> m_Wires;

		int m_KeyPosition;
		int m_RingPosition;

		char m_TurnoverPosition;
	};

}