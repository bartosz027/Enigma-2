using EnigmaApp.Core;
using CLI_Enigma = CLI.ENIGMA.Enigma;

namespace EnigmaApp.ViewModel {

    class RotorViewModel : ObservableObject {
        public RotorViewModel(CLI_Enigma enigma) {
            Enigma = enigma;

            RotorKeyIncreaseCommand = new RelayCommand(obj => {
                int index = (int)obj;
                char key = Enigma.Components.Configuration.Rotors[index].Key;

                if (key == 'Z') {
                    Enigma.SetRotorKey(index, 'A');
                }
                else {
                    Enigma.SetRotorKey(index, ++key);
                }
            });

            RotorKeyDecreaseCommand = new RelayCommand(obj => {
                int index = (int)obj;
                char key = Enigma.Components.Configuration.Rotors[index].Key;

                if (key == 'A') {
                    Enigma.SetRotorKey(index, 'Z');
                }
                else {
                    Enigma.SetRotorKey(index, --key);
                }
            });
        }

        // Variables
        public CLI_Enigma Enigma { get; set; }

        // Commands
        public RelayCommand RotorKeyIncreaseCommand { get; set; }
        public RelayCommand RotorKeyDecreaseCommand { get; set; }
    }

}