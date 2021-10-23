using EnigmaApp.Core;
using CLI_Enigma = CLI.ENIGMA.Enigma;

namespace EnigmaApp.ViewModel {

    class EncryptionViewModel : ObservableObject {

        public EncryptionViewModel(CLI_Enigma enigma) {
            Enigma = enigma;

            InputTextBoxChangedCommand = new RelayCommand(obj => {
                Enigma.EncryptMessage(Enigma.Message.Input);
            });
        }

        // Variables
        public CLI_Enigma Enigma { get; set; }

        // Commands
        public RelayCommand InputTextBoxChangedCommand { get; set; }

    }

}