#include "Components.h"

namespace CLI::ENIGMA {

	AvailableComponents::AvailableComponents() {
		_Rotors = gcnew List<String^>();
		_Reflectors = gcnew List<String^>();
	}


	ConfigurationComponents::ConfigurationComponents() {
		_Rotors = gcnew ObservableCollection<Rotor^>();
	}


	Components::Components() {
		_Available = gcnew AvailableComponents();
		_Configuration = gcnew ConfigurationComponents();
	}

}