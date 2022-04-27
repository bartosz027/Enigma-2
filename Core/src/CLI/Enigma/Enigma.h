#pragma once
#include <Enigma/Enigma.h>

#include "CLI/ManagedObject.h"
#include "CLI/ObservableObject.h"

#include "CLI/Enigma/Components.h"

namespace CLI::ENIGMA {

	public ref struct Message : ObservableObject {
	public:
		property String^ Input {
			String^ get() {
				return _Input;
			}
			void set(String^ value) {
				_Input = value;
				OnPropertyChanged("Input");
			}
		}

		property String^ Output {
			String^ get() {
				return _Output;
			}
			void set(String^ value) {
				_Output = value;
				OnPropertyChanged("Output");
			}
		}
	private:
		String^ _Input;
		String^ _Output;
	};

	public ref class Enigma : public ManagedObject<Encryption::Enigma> {
    public:
		Enigma(String^ rotor_dir, String^ reflector_dir);
        Enigma(String^ rotor_dir, String^ reflector_dir, bool update_rotor_keys);

		void LoadSettings(String^ filepath, String^ password);
		void SaveSettings(String^ filepath, String^ password);

		void AddPlugboardConnection(String^ plugs);
		void RemovePlugboardConnection(String^ plugs);

		void SetRotorCount(int count);
		void SetReflector(String^ name);

		void SetRotorName(int index, String^ name);
		void SetRotorRing(int index, int ring);
		void SetRotorKey(int index, Char key);

		void EncryptMessage(String^ message);
	public:
		property List<String^>^ PlugboardConnections {
			List<String^>^ get() {
				return _PlugboardConnections;
			}
			void set(List<String^>^ value) {
				_PlugboardConnections = value;
			}
		}

		property CLI::ENIGMA::Components^ Components {
			CLI::ENIGMA::Components^ get() { 
				return _Components; 
			}
			void set(CLI::ENIGMA::Components^ value) { 
				_Components = value; 
			}
		}

		property CLI::ENIGMA::Message^ Message {
			CLI::ENIGMA::Message^ get() {
				return _Message;
			}
			void set(CLI::ENIGMA::Message^ value) {
				_Message = value;
			}
		}
	private:
		void LoadComponents();
		void UpdateKeys();
	private:
		bool _UpdateRotorKeysFlag;
		List<String^>^ _PlugboardConnections;

		// Decryption components (local)
		CLI::ENIGMA::Components^ _Components;

		// Encrypted and decrypted message
		CLI::ENIGMA::Message^ _Message;
    };

}