#pragma once
#include "CLI/ObservableObject.h"

namespace CLI::ENIGMA {

	public ref struct Rotor : ObservableObject {
	public:
		Rotor();
	public:
		property String^ Name {
			String^ get() {
				return _Name;
			}
			void set(String^ value) {
				_Name = value;
				OnPropertyChanged("Name");
			}
		}

		property String^ Ring {
			String^ get() {
				return _Ring;
			}
			void set(String^ value) {
				_Ring = value;
				OnPropertyChanged("Ring");
			}
		}

		property Char Key {
			Char get() {
				return _Key;
			}
			void set(Char value) {
				_Key = value;
				OnPropertyChanged("Key");
			}
		}
	private:
		String^ _Name;
		String^ _Ring;
		Char _Key;
	};

}