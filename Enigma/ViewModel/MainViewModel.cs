using System;
using System.IO;
using System.Windows;

using EnigmaApp.Core;
using EnigmaApp.Controls;

using Microsoft.Win32;
using CLI_Enigma = CLI.ENIGMA.Enigma;

namespace EnigmaApp.ViewModel {

    class MainViewModel : ObservableObject {
        public MainViewModel() {
            try {
                Enigma = new CLI_Enigma("Rotors", "Reflectors");

                RotorVM = new RotorViewModel(Enigma);
                EncryptionVM = new EncryptionViewModel(Enigma);
                PlugboardVM = new PlugboardViewModel(Enigma);

                SettingsVM = new SettingsViewModel(Enigma);
            }
            catch (Exception e) {
                string error = "Message:\n" + e.Message;
                MessageBox.Show(error, "Error");

                Environment.Exit(-1);
            }

            SaveSettingsCommand = new RelayCommand(obj => {
                var saveFileDialog = new SaveFileDialog();
                saveFileDialog.InitialDirectory = Directory.GetCurrentDirectory();

                saveFileDialog.Title = "Save File";
                saveFileDialog.Filter = "STG files|*.stg";

                bool? dialogStatus = saveFileDialog.ShowDialog();

                if(dialogStatus == true) {
                    var window = new PasswordBoxWindow();
                    window.ShowDialog();

                    if (window.DialogResult == true) {
                        Enigma.SaveSettings(saveFileDialog.FileName, window.Password_TextBox.Password);
                    }
                }
            });

            LoadSettingsCommand = new RelayCommand(obj => {
                var openFileDialog = new OpenFileDialog();
                openFileDialog.InitialDirectory = Directory.GetCurrentDirectory();

                openFileDialog.Title = "Load File";
                openFileDialog.Filter = "STG files|*.stg";
                openFileDialog.FilterIndex = 1;

                bool? dialogStatus = openFileDialog.ShowDialog();

                if (dialogStatus == true) {
                    var window = new PasswordBoxWindow();
                    window.ShowDialog();

                    if (window.DialogResult == true) {
                        try {
                            Enigma.LoadSettings(openFileDialog.FileName, window.Password_TextBox.Password);
                            PlugboardVM.UpdateButtons();
                        }
                        catch (Exception e) {
                            string error = "Message:\n" + e.Message;
                            MessageBox.Show(error, "Error");
                        }
                    }
                }
            });
        }

        // Variables
        CLI_Enigma Enigma { get; set; }

        // Main VM's
        public RotorViewModel RotorVM { get; set; }
        public EncryptionViewModel EncryptionVM { get; set; }
        public PlugboardViewModel PlugboardVM { get; set; }

        // Settings VM
        public SettingsViewModel SettingsVM { get; set; }

        // Commands
        public RelayCommand SaveSettingsCommand { get; set; }
        public RelayCommand LoadSettingsCommand { get; set; }
    }

}