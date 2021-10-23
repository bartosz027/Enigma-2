#include "Enigma.h"

namespace CLI::ENIGMA {

	static String^ ConvertRingToString(int ring) {
		return (ring + " " + Char(ring + 64));
	}


	Enigma::Enigma(String^ rotor_dir, String^ reflector_dir) :
		ManagedObject(new Encryption::Enigma()) {

		_UpdateRotorKeysFlag = false;
		_PlugboardConnections = gcnew List<String^>();

		try {
			m_Instance->Init(ConvertTo<std::string>(rotor_dir), ConvertTo<std::string>(reflector_dir));
		}
		catch (const char* error) {
			throw gcnew Exception(ConvertTo<String^>(error));
		}

		Components = gcnew CLI::ENIGMA::Components();
		Message = gcnew CLI::ENIGMA::Message();

		LoadComponents();
	}

	Enigma::Enigma(String^ rotor_dir, String^ reflector_dir, bool update_rotor_keys) :
		ManagedObject(new Encryption::Enigma()) {

		this->Enigma::Enigma(rotor_dir, reflector_dir);
		_UpdateRotorKeysFlag = update_rotor_keys;
	}


	void Enigma::LoadComponents() {
		const auto& components = m_Instance->GetComponents();
		const auto& [rotors, reflectors] = components.Available;

		// Load rotors from core.dll
		for (const auto& rotor : rotors) {
			Components->Available->Rotors->Add(ConvertTo<String^>(rotor.GetName()));
		}

		for (const auto& rotor : components.Configuration.Rotors) {
			Rotor^ rot = gcnew Rotor();
			rot->Name = ConvertTo<String^>(rotor.GetName());
			rot->Ring = ConvertRingToString(rotor.GetRingPosition());
			rot->Key = m_Instance->GetRotorKeySettings()[Components->Configuration->Rotors->Count];

			Components->Configuration->Rotors->Add(rot);
		}

		// Load reflectors from core.dll
		for (const auto& reflector : reflectors) {
			Components->Available->Reflectors->Add(ConvertTo<String^>(reflector.GetName()));
		}

		Components->Configuration->Reflector = Components->Available->Reflectors[0];
	}


	void Enigma::LoadSettings(String^ filepath, String^ password) {
		// Load settings (core.dll)
		try {
			m_Instance->LoadSettings(ConvertTo<std::string>(filepath), ConvertTo<std::string>(password));
			Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);
		}
		catch (const char* error) {
			throw gcnew Exception(ConvertTo<String^>(error));;
		}

		// Clear current data (local)
		PlugboardConnections->Clear();
		Components->Configuration->Rotors->Clear();

		// Set new data (local)
		const auto& components = m_Instance->GetComponents();
		const auto& [plugboard, reflector, rotors] = components.Configuration;

		// Load plugboard settings (local)
		std::string connected_plugs = plugboard->GetConnectedPlugs();

		for (int i = 0; i < connected_plugs.size(); i += 2) {
			std::string plugs = "";

			plugs += connected_plugs[i];
			plugs += connected_plugs[i + 1];

			PlugboardConnections->Add(ConvertTo<String^>(plugs));
		}
		
		// Load rotors settings (local)
		for (const auto& rotor : rotors) {
			Rotor^ rot = gcnew Rotor();
			rot->Name = ConvertTo<String^>(rotor.GetName());
			rot->Ring = ConvertRingToString(rotor.GetRingPosition());
			rot->Key = m_Instance->GetRotorKeySettings()[Components->Configuration->Rotors->Count];

			Components->Configuration->Rotors->Add(rot);
		}

		// Load reflector settings (local)
		Components->Configuration->Reflector = ConvertTo<String^>(reflector->GetName());
	}

	void Enigma::SaveSettings(String^ filepath, String^ password) {
		m_Instance->SaveSettings(ConvertTo<std::string>(filepath), ConvertTo<std::string>(password));
	}


	void Enigma::AddPlugboardConnection(String^ plugs) {
		// Add plugboard connection (core.dll)
		m_Instance->AddPlugboardConnection(ConvertTo<std::string>(plugs));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Add plugboard connection (local)
		PlugboardConnections->Add(plugs);
	}

	void Enigma::RemovePlugboardConnection(String^ plugs) {
		// Remove plugboard connection (core.dll)
		m_Instance->RemovePlugboardConnection(ConvertTo<std::string>(plugs));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Remove plugboard connection (local)
		PlugboardConnections->Remove(plugs);
	}


	void Enigma::SetRotorCount(int count) {
		if (count < 1 || count > MAX_ROTOR_COUNT) {
			return;
		}

		// Adds or removes rotors up to the given number (core.dll)
		m_Instance->SetRotorCount(count);
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Adds or removes rotors up to the given number (local) 
		auto& [plugboard, reflector, rotors] = m_Instance->GetComponents().Configuration;

		int curr_count = Components->Configuration->Rotors->Count;
		int difference = count - curr_count;

		for (int i = curr_count; i < curr_count + std::abs(difference); i++) {
			if (count > curr_count) {
				Rotor^ rot = gcnew Rotor();
				rot->Name = ConvertTo<String^>(rotors[i].GetName());
				rot->Ring = ConvertRingToString(rotors[i].GetRingPosition());
				rot->Key = m_Instance->GetRotorKeySettings()[Components->Configuration->Rotors->Count];

				Components->Configuration->Rotors->Add(rot);
			}
			else if (count < curr_count) {
				Components->Configuration->Rotors->RemoveAt(Components->Configuration->Rotors->Count - 1);
			}
		}

		// Update rotor keys
		UpdateKeys();
	}

	void Enigma::SetReflector(String^ name) {
		// Set reflector (core.dll)
		m_Instance->SetReflector(ConvertTo<std::string>(name));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Set reflector (local)
		Components->Configuration->Reflector = name;
	}


	void Enigma::SetRotorName(int index, String^ name) {
		// Set rotor name (core.dll)
		m_Instance->SetRotorName(index, ConvertTo<std::string>(name));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Set rotor name (local)
		Components->Configuration->Rotors[index]->Name = name;
	}

	void Enigma::SetRotorRing(int index, int ring) {
		// Set rotor ring (core.dll)
		m_Instance->SetRotorRing(index, ring);
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Set rotor name (local)
		Components->Configuration->Rotors[index]->Ring = ConvertRingToString(ring);
	}

	void Enigma::SetRotorKey(int index, Char key) {
		// Set rotor key (core.dll)
		m_Instance->SetRotorKey(index, static_cast<char>(key));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Set rotor key (local)
		Components->Configuration->Rotors[index]->Key = key;
	}


	void Enigma::EncryptMessage(String^ message) {
		// Decrypt message
		m_Instance->EncryptMessage(ConvertTo<std::string>(message));
		Message->Output = ConvertTo<String^>(m_Instance->GetMessage().Output);

		// Update rotor keys
		UpdateKeys();
	}


	void Enigma::UpdateKeys() {
		if (_UpdateRotorKeysFlag) {
			const auto& components = m_Instance->GetComponents();
			auto count = Components->Configuration->Rotors->Count;

			for (int i = 0; i < count; i++) {
				Components->Configuration->Rotors[i]->Key = components.Configuration.Rotors[i].GetKeyPosition();
			}
		}
	}

}