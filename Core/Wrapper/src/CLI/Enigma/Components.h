#pragma once
#include "CLI/Enigma/Rotor.h"

using namespace System;
using namespace System::Collections::Generic;

#using <System.dll>
using namespace System::Collections::ObjectModel;

namespace CLI::ENIGMA {

	public ref struct AvailableComponents {
	public:
		AvailableComponents();
	public:
		property List<String^>^ Rotors {
			List<String^>^ get() {
				return _Rotors;
			}
			void set(List<String^>^ value) {
				_Rotors = value;
			}
		}

		property List<String^>^ Reflectors {
			List<String^>^ get() {
				return _Reflectors;
			}
			void set(List<String^>^ value) {
				_Reflectors = value;
			}
		}
	private:
		List<String^>^ _Rotors;
		List<String^>^ _Reflectors;
	};

	public ref struct ConfigurationComponents : ObservableObject {
	public:
		ConfigurationComponents();
	public:
		property ObservableCollection<Rotor^>^ Rotors {
			ObservableCollection<Rotor^>^ get() {
				return _Rotors;
			}
			void set(ObservableCollection<Rotor^>^ value) {
				_Rotors = value;
			}
		}

		property String^ Reflector {
			String^ get() {
				return _Reflector;
			}
			void set(String^ value) {
				_Reflector = value;
				OnPropertyChanged("Reflector");
			}
		}
	private:
		ObservableCollection<Rotor^>^ _Rotors;
		String^ _Reflector;
	};


	public ref struct Components {
	public:
		Components();
	public:
		property AvailableComponents^ Available {
			AvailableComponents^ get() {
				return _Available;
			}
			void set(AvailableComponents^ value) {
				_Available = value;
			}
		}

		property ConfigurationComponents^ Configuration {
			ConfigurationComponents^ get() {
				return _Configuration;
			}
			void set(ConfigurationComponents^ value) {
				_Configuration = value;
			}
		}
	private:
		AvailableComponents^ _Available;
		ConfigurationComponents^ _Configuration;
	};

}