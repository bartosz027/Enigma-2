#include "Enigma.h"

#include <fstream>
#include <sstream>

#include <filesystem>
#include <regex>

#include "AES/AES.h"

namespace Encryption {

	Enigma::Enigma() :
		m_RotorConfigurationCache(1, std::string(MAX_ROTOR_COUNT, 'A')),
		m_RotorConfigurationCacheLastIndex(0) {
	}


	void Enigma::Init(const std::string& rotorDir, const std::string& reflectorDir) {
		m_Components.Configuration.Plugboard = std::make_unique<Plugboard>();

		InitRotors(rotorDir);
		InitReflectors(reflectorDir);
	}

	void Enigma::InitRotors(const std::string& dir) {
		if (std::filesystem::is_directory(dir)) {
			std::ifstream file;

			for (const auto& it : std::filesystem::directory_iterator(dir)) {
				auto path = it.path();

				std::string filepath = path.string();
				std::string extension = path.extension().string();

				// Read from file with extension .rot
				if (extension == ".rot") {
					file.open(filepath);

					std::stringstream ss1;
					ss1 << file.rdbuf();

					std::string hex_data = ss1.str();
					std::string data = AES::HexToBytes(hex_data);

					const std::regex pattern("[^A-Z ]");
					data = std::regex_replace(data, pattern, "");

					std::stringstream ss2;
					ss2 << data;

					std::string name, inputs, outputs, turnoverPosition;
					ss2 >> name >> inputs >> outputs >> turnoverPosition;

					file.close();
					m_Components.Available.Rotors.emplace_back(name, inputs, outputs, turnoverPosition[0]);
				}
			}
		}
		else {
			throw "Rotor directory not found!";
		}

		// Check if there is at least one rotor loaded
		if (m_Components.Available.Rotors.size() == 0) {
			throw "Missing rotor files!";
		}

		// Add three rotors
		m_Components.Configuration.Rotors.push_back(m_Components.Available.Rotors[0]);
		m_Components.Configuration.Rotors.push_back(m_Components.Available.Rotors[0]);
		m_Components.Configuration.Rotors.push_back(m_Components.Available.Rotors[0]);
	}

	void Enigma::InitReflectors(const std::string& dir) {
		if (std::filesystem::is_directory(dir)) {
			std::ifstream file;

			for (const auto& it : std::filesystem::directory_iterator(dir)) {
				auto path = it.path();

				std::string filepath = path.string();
				std::string extension = path.extension().string();

				// Read from file with extension .ref
				if (extension == ".ref") {
					file.open(filepath);

					std::stringstream ss1;
					ss1 << file.rdbuf();

					std::string hex_data = ss1.str();
					std::string data = AES::HexToBytes(hex_data);

					const std::regex pattern("[^A-Z ]");
					data = std::regex_replace(data, pattern, "");

					std::stringstream ss2;
					ss2 << data;

					std::string name, inputs, outputs;
					ss2 >> name >> inputs >> outputs;

					file.close();
					m_Components.Available.Reflectors.emplace_back(name, inputs, outputs);
				}
			}
		}
		else {
			throw "Reflector directory not found!";
		}

		// Check if there is at least one reflector loaded
		if (m_Components.Available.Reflectors.size() == 0) {
			throw "Missing reflector files!";
		}

		// Set reflector
		m_Components.Configuration.Reflector = std::make_unique<Reflector>(m_Components.Available.Reflectors[0]);
	}


	void Enigma::LoadSettings(const std::string& filepath, const std::string& password) {
		auto path = std::filesystem::path(filepath);

		std::string filename = path.filename().string();
		std::string extension = path.extension().string();

		if (filename == "") {
			throw "Invalid filename!";
		}
		else if (extension != ".stg") {
			throw "Invalid extension!";
		}

		// Load settings from file
		std::ifstream in(filepath);
		std::string decrypted_data;

		std::stringstream ss1;
		ss1 << in.rdbuf();

		// Decrypt settings
		try {
			std::string encrypted_data = ss1.str();
			decrypted_data = AES::Decrypt(encrypted_data, password);
		}
		catch (const char* error) {
			throw error;
		}

		std::stringstream ss2;
		ss2 << decrypted_data;

		// Reset current plugboard settings
		m_Components.Configuration.Plugboard->Reset();

		// Load plugboard settings
		std::string plugboard;
		ss2 >> plugboard;

		if (plugboard != "NULL" && plugboard.size() % 2 == 0) {
			for (int i = 0; i < plugboard.size(); i += 2) {
				AddPlugboardConnection(&plugboard[i]);
			}
		}

		// Load reflector settings
		std::string reflector;
		ss2 >> reflector;

		SetReflector(reflector);

		// Remove ALL rotors
		m_Components.Configuration.Rotors.clear();

		// Load rotor settings
		std::string rotor_count;
		ss2 >> rotor_count;

		for (int i = 0; i < std::stoi(rotor_count); i++) {
			std::string name;
			ss2 >> name;

			int ring;
			ss2 >> ring;

			auto it = std::find_if(m_Components.Available.Rotors.begin(), m_Components.Available.Rotors.end(), [name](Rotor rotor) {
				return name == rotor.GetName();
			});

			if (it != m_Components.Available.Rotors.end()) {
				int index1 = static_cast<int>(std::distance(m_Components.Available.Rotors.begin(), it));
				m_Components.Configuration.Rotors.push_back(m_Components.Available.Rotors[index1]);

				int index2 = static_cast<int>(m_Components.Configuration.Rotors.size()) - 1;
				m_Components.Configuration.Rotors[index2].SetRingPosition(ring);

				char key = m_RotorConfigurationCache[0][index2];
				m_Components.Configuration.Rotors[index2].SetKeyPosition(key);
			}
			else {
				throw "Rotor \"" + name + "\" not found!";
			}
		}

		EncryptMessageAfterConfigurationChanges();
	}

	void Enigma::SaveSettings(const std::string& filepath, const std::string& password) const {
		auto path = std::filesystem::path(filepath);
		std::string settings = "";

		std::string filename = path.filename().string();
		std::string extension = path.extension().string();

		if (filename == "") {
			throw "Invalid filename!";
		}
		else if (extension != ".stg") {
			throw "Invalid extension!";
		}

		// Get plugboard settings
		std::string plugs = m_Components.Configuration.Plugboard->GetConnectedPlugs();

		// Save plugboard settings
		settings += (plugs != "") ? plugs : "NULL";
		settings += ' ';

		// Save reflector settings
		settings += m_Components.Configuration.Reflector->GetName();
		settings += ' ';

		// Save rotor count
		settings += std::to_string(m_Components.Configuration.Rotors.size());
		settings += ' ';

		// Save rotor settings
		for (int i = 0; i < m_Components.Configuration.Rotors.size(); i++) {
			settings += m_Components.Configuration.Rotors[i].GetName();
			settings += ' ';

			settings += std::to_string(m_Components.Configuration.Rotors[i].GetRingPosition());
			settings += ' ';
		}

		// Encrypt settings
		try {
			std::ofstream out(filepath);
			out << AES::Encrypt(settings, password);
		}
		catch (const char* error) {
			throw error;
		}
	}


	void Enigma::AddPlugboardConnection(const std::string& plugs) {
		m_Components.Configuration.Plugboard->Connect(plugs);
		EncryptMessageAfterConfigurationChanges();
	}

	void Enigma::RemovePlugboardConnection(const std::string& plugs) {
		m_Components.Configuration.Plugboard->Disconnect(plugs);
		EncryptMessageAfterConfigurationChanges();
	}


	void Enigma::SetRotorCount(int count) {
		if (count < 1 || count > MAX_ROTOR_COUNT) {
			throw "Invalid parameter!";
		}

		int curr_count = static_cast<int>(m_Components.Configuration.Rotors.size());
		int difference = count - curr_count;

		for (int i = 0; i < std::abs(difference); i++) {
			if (count > curr_count) {
				m_Components.Configuration.Rotors.push_back(m_Components.Available.Rotors[0]);

				int index = static_cast<int>(m_Components.Configuration.Rotors.size()) - 1;
				m_Components.Configuration.Rotors[index].SetKeyPosition(m_RotorConfigurationCache[0][index]);
			}
			else if (count < curr_count) {
				m_Components.Configuration.Rotors.erase(m_Components.Configuration.Rotors.end() - 1);
			}
		}

		EncryptMessageAfterConfigurationChanges();
	}

	void Enigma::SetReflector(const std::string& name) {
		auto it = std::find_if(m_Components.Available.Reflectors.begin(), m_Components.Available.Reflectors.end(), [name](Reflector reflector) {
			return name == reflector.GetName();
		});

		if (it != m_Components.Available.Reflectors.end()) {
			int index = static_cast<int>(std::distance(m_Components.Available.Reflectors.begin(), it));
			m_Components.Configuration.Reflector = std::make_unique<Reflector>(m_Components.Available.Reflectors[index]);

			EncryptMessageAfterConfigurationChanges();
		}
		else {
			throw "Invalid parameter!";
		}
	}


	void Enigma::SetRotorName(int index, const std::string& name) {
		if (index < 0 || index >= m_Components.Configuration.Rotors.size()) {
			throw "Index out of range!";
		}

		auto it = std::find_if(m_Components.Available.Rotors.begin(), m_Components.Available.Rotors.end(), [name](Rotor rotor) {
			return name == rotor.GetName();
		});

		if (it != m_Components.Available.Rotors.end()) {
			int ring = m_Components.Configuration.Rotors[index].GetRingPosition();
			char key = m_Components.Configuration.Rotors[index].GetKeyPosition();

			int index1 = static_cast<int>(std::distance(m_Components.Available.Rotors.begin(), it));
			m_Components.Configuration.Rotors[index] = m_Components.Available.Rotors[index1];

			m_Components.Configuration.Rotors[index].SetRingPosition(ring);
			m_Components.Configuration.Rotors[index].SetKeyPosition(key);

			EncryptMessageAfterConfigurationChanges();
		}
		else {
			throw "Invalid \"name\" parameter!";
		}
	}

	void Enigma::SetRotorRing(int index, int ring) {
		if (index < 0 || index >= m_Components.Configuration.Rotors.size()) {
			throw "Index out of range!";
		}

		if (ring >= 1 && ring <= 26) {
			m_Components.Configuration.Rotors[index].SetRingPosition(ring);
			EncryptMessageAfterConfigurationChanges();
		}
		else {
			throw "Invalid \"ring\" parameter!";
		}
	}

	void Enigma::SetRotorKey(int index, char key) {
		if (index < 0 || index >= m_Components.Configuration.Rotors.size()) {
			throw "Index out of range!";
		}

		if (key >= 'A' && key <= 'Z') {
			m_Components.Configuration.Rotors[index].SetKeyPosition(key);
			m_RotorConfigurationCache[0][index] = key;

			EncryptMessageAfterConfigurationChanges();
		}
		else {
			throw "Invalid \"key\" parameter!";
		}
	}


	void Enigma::EncryptMessage(const std::string& message) {
		// Index of the first difference between previous and current encrypted string
		int index = 0; 

		// Find the first difference between previous and current encrypted string
		while (index < std::min(m_Message.Input.length(), message.length())) {
			if (m_Message.Input[index] != message[index]) {
				break;
			}

			index++;
		}

		// Check if there is rotor configuration in cache
		if (index < m_RotorConfigurationCacheLastIndex) {
			for (int i = 0; i < m_Components.Configuration.Rotors.size(); i++) {
				char key = m_RotorConfigurationCache[index][i];
				m_Components.Configuration.Rotors[i].SetKeyPosition(key);
			}

			m_RotorConfigurationCacheLastIndex = index;
		}

		// Remove not valid letters
		m_Message.Input = m_Message.Input.substr(0, index);
		m_Message.Output = m_Message.Output.substr(0, index);

		// Decrypt message
		for (int i = index; i < message.length(); i++) {
			char letter = message[i];

			if ((letter >= 'A' && letter <= 'Z') || letter == ' ') {
				AddLetter(letter);
			}
		}
	}

	void Enigma::AddLetter(char letter) {
		// Add encrypted letter
		m_Message.Input.push_back(letter);

		// Check if there is enough space in cache to save current configuration
		if (m_Message.Output.length() >= m_RotorConfigurationCache.size()) {
			m_RotorConfigurationCache.resize(2 * m_RotorConfigurationCache.size(), std::string(MAX_ROTOR_COUNT, '-'));
		}

		// Save current configuration in cache
		for (int i = 0; i < m_Components.Configuration.Rotors.size(); i++) {
			int index = m_RotorConfigurationCacheLastIndex;
			m_RotorConfigurationCache[index][i] = m_Components.Configuration.Rotors[i].GetKeyPosition();
		}

		m_RotorConfigurationCacheLastIndex++;

		// Decrypt letter
		if (letter != ' ') {
			int size = static_cast<int>(m_Components.Configuration.Rotors.size());

			// Move rotors
			m_Components.Configuration.Rotors[size - 1].Move();

			for (int i = size - 1; i > 0; i--) {
				if (ShouldAdjacentRotorMove(i)) {
					m_Components.Configuration.Rotors[i - 1].Move();
					continue;
				}

				break;
			}

			// Send signal to plugboard
			m_Components.Configuration.Plugboard->ProcessSignal(letter);

			// Send signal to rotors
			for (int i = size - 1; i >= 0; i--) {
				m_Components.Configuration.Rotors[i].ProcessSignal(letter);
			}
			
			// Send signal to reflector
			m_Components.Configuration.Reflector->ProcessSignal(letter);

			// Send back signal to rotors
			for (int i = 0; i < size; i++) {
				m_Components.Configuration.Rotors[i].ProcessReversedSignal(letter);
			}

			// Send back signal to plugboard
			m_Components.Configuration.Plugboard->ProcessSignal(letter);

			// Add decrypted letter
			m_Message.Output.push_back(letter);
		}
		else if (letter == ' ') {
			// Add space sign
			m_Message.Output.push_back(' ');
		}
	}

	bool Enigma::ShouldAdjacentRotorMove(int index) const {
		return m_Components.Configuration.Rotors[index].GetKeyPosition() == m_Components.Configuration.Rotors[index].GetTurnoverPosition();
	}


	void Enigma::EncryptMessageAfterConfigurationChanges() {
		std::string copy = m_Message.Input;
		m_Message.Input = "";

		EncryptMessage(copy);
	}

}