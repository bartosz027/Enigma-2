using System.Text.RegularExpressions;

using EnigmaApp.Core;
using CLI_Enigma = CLI.ENIGMA.Enigma;

namespace EnigmaApp.ViewModel {

    class SettingsViewModel : ObservableObject {
        public SettingsViewModel(CLI_Enigma enigma) {
            Enigma = enigma;

            AddRotorCommand = new RelayCommand(obj => {
                Enigma.SetRotorCount(Enigma.Components.Configuration.Rotors.Count + 1);
            });

            RemoveRotorCommand = new RelayCommand(obj => {
                Enigma.SetRotorCount(Enigma.Components.Configuration.Rotors.Count - 1);
            });


            IncreaseRingCommand = new RelayCommand(obj => {
                int index = (int)obj;

                string ring_str = Enigma.Components.Configuration.Rotors[index].Ring;
                int ring = int.Parse(Regex.Replace(ring_str, "[^0-9]", ""));

                if (ring == 26) {
                    Enigma.SetRotorRing(index, 1);
                }
                else {
                    Enigma.SetRotorRing(index, ++ring);
                }
            });

            DecreaseRingCommand = new RelayCommand(obj => {
                int index = (int)obj;

                string ring_str = Enigma.Components.Configuration.Rotors[index].Ring;
                int ring = int.Parse(Regex.Replace(ring_str, "[^0-9]", ""));

                if (ring == 1) {
                    Enigma.SetRotorRing(index, 26);
                }
                else {
                    Enigma.SetRotorRing(index, --ring);
                }
            });


            RotorComboBoxSelectedItemChanged = new RelayCommand(obj => {
                int index = (int)obj;
                Enigma.SetRotorName(index, Enigma.Components.Configuration.Rotors[index].Name);
            });

            ReflectorComboBoxSelectedItemChanged = new RelayCommand(obj => {
                Enigma.SetReflector(Enigma.Components.Configuration.Reflector);
            });
        }

        // Variables
        public CLI_Enigma Enigma { get; set; }

        // Commands
        public RelayCommand AddRotorCommand { get; set; }
        public RelayCommand RemoveRotorCommand { get; set; }

        public RelayCommand IncreaseRingCommand { get; set; }
        public RelayCommand DecreaseRingCommand { get; set; }

        public RelayCommand RotorComboBoxSelectedItemChanged { get; set; }
        public RelayCommand ReflectorComboBoxSelectedItemChanged { get; set; }
    }

}