using System;
using System.IO;

using System.Collections.Generic;
using System.Linq;

using System.Text;
using System.Text.RegularExpressions;

using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;

using Microsoft.Win32;
using CLI_AES = CLI.AES.AES;

namespace Element_Creator {

    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();

            _WireInputs = new List<char>(26) {
                'A','B','C','D','E','F','G','H','I',
                'J','K','L','M','N','O','P','Q','R',
                'S','T','U','V','W','X','Y','Z'
            };

            _WireOutputs = new List<TextBox>(26) {
                WireOutput1_Textbox,  WireOutput2_Textbox,
                WireOutput3_Textbox,  WireOutput4_Textbox,
                WireOutput5_Textbox,  WireOutput6_Textbox,
                WireOutput7_Textbox,  WireOutput8_Textbox,
                WireOutput9_Textbox,  WireOutput10_Textbox,
                WireOutput11_Textbox, WireOutput12_Textbox,
                WireOutput13_Textbox, WireOutput14_Textbox,
                WireOutput15_Textbox, WireOutput16_Textbox,
                WireOutput17_Textbox, WireOutput18_Textbox,
                WireOutput19_Textbox, WireOutput20_Textbox,
                WireOutput21_Textbox, WireOutput22_Textbox,
                WireOutput23_Textbox, WireOutput24_Textbox,
                WireOutput25_Textbox, WireOutput26_Textbox
            };
        }

        // Events
        private void ElementName_TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e) {
            var regex = new Regex("[^A-Za-z]+");
            e.Handled = regex.IsMatch(e.Text);
        }

        private void ElementType_ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            var comboBox = sender as ComboBox;

            if (comboBox != null && TurnoverPosition_TextBox != null) {
                TurnoverPosition_TextBox.IsEnabled = (comboBox.SelectedIndex == 0);
            }
        }

        private void WireOutput_Textbox_PreviewKeyDown(object sender, KeyEventArgs e) {
            char keyPressed = (char)KeyInterop.VirtualKeyFromKey(e.Key);

            if (keyPressed >= 'A' && keyPressed <= 'Z') {
                int index = _WireOutputs.FindIndex(x => x.Text.Contains(keyPressed));

                var textBox1 = _WireOutputs[index];
                var textBox2 = sender as TextBox;

                char prevText = char.Parse(textBox1.Text);
                textBox1.Text = textBox2.Text;
                textBox2.Text = prevText.ToString();
            }

            e.Handled = e.Key != Key.Tab;
        }

        private void TurnoverPosition_TextBox_PreviewKeyDown(object sender, KeyEventArgs e) {
            char keyPressed = (char)KeyInterop.VirtualKeyFromKey(e.Key);
            var textBox = sender as TextBox;

            if (keyPressed >= 'A' && keyPressed <= 'Z') {
                textBox.Text = keyPressed.ToString();
            }

            e.Handled = e.Key != Key.Tab;
        }

        private void Convert_Button_Click(object sender, RoutedEventArgs e) {
            string wires = WiresFromString_TextBox.Text;

            if (wires.Length != 26) {
                return;
            }

            for (int i = 0; i < 26; i++) {
                for (int j = i + 1; j < 26; j++) {
                    if (wires[i] == wires[j]) {
                        return;
                    }
                }
            }

            for (int i = 0; i < _WireOutputs.Count; i++) {
                _WireOutputs[i].Text = wires[i].ToString();
            }
        }

        private void LoadFile_Button_Click(object sender, RoutedEventArgs e) {
            var openFileDialog = new OpenFileDialog();

            openFileDialog.InitialDirectory = Directory.GetCurrentDirectory();
            openFileDialog.Title = "Load File";

            switch (ElementType_ComboBox.SelectedIndex) {
                case 0: {
                        openFileDialog.Filter = "ROT files|*.rot";
                        break;
                    }
                case 1: {
                        openFileDialog.Filter = "REF files|*.ref";
                        break;
                    }
            }

            bool? dialogStatus = openFileDialog.ShowDialog();

            if(dialogStatus == true) {
                string hex_data = File.ReadAllText(openFileDialog.FileName);

                string bytes = CLI_AES.HexToBytes(hex_data);
                bytes = RemoveRandomBytesFromString(bytes);

                string[] data = bytes.Split(' ');
                ElementName_TextBox.Text = data[0];

                for (int i = 0; i < _WireOutputs.Count; i++) {
                    _WireOutputs[i].Text = data[2][i].ToString();
                }

                if (Path.GetExtension(openFileDialog.FileName) == ".rot") {
                    ElementType_ComboBox.SelectedIndex = 0;
                    TurnoverPosition_TextBox.Text = data[3];
                }
                else if (Path.GetExtension(openFileDialog.FileName) == ".ref") {
                    ElementType_ComboBox.SelectedIndex = 1;
                }
            }
        }

        private void SaveFile_Button_Click(object sender, RoutedEventArgs e) {
            if (ElementName_TextBox.Text == string.Empty) {
                MessageBox.Show("Name field cannot be empty!", "Error");
                return;
            }

            var saveFileDialog = new SaveFileDialog();
            saveFileDialog.InitialDirectory = Directory.GetCurrentDirectory();

            saveFileDialog.FileName = ElementName_TextBox.Text;
            saveFileDialog.Title = "Save File";

            saveFileDialog.FileOk += (obj, eventArgs) => {
                string filename = Path.GetFileNameWithoutExtension(saveFileDialog.FileName);

                if(filename.All(c => char.IsLetter(c) && char.IsUpper(c)) == false) {
                    eventArgs.Cancel = true;
                    MessageBox.Show("Filename can only contain uppercase letters!", "Error");
                }
            };

            switch (ElementType_ComboBox.SelectedIndex) {
                case 0: {
                    saveFileDialog.Filter = "ROT files|*.rot";
                    break;
                }
                case 1: {
                    saveFileDialog.Filter = "REF files|*.ref";
                    break;
                }
            }

            bool? dialogStatus = saveFileDialog.ShowDialog();

            if(dialogStatus == true) {
                string data = ElementName_TextBox.Text + ' ';

                foreach (var wireInput in _WireInputs) {
                    data += wireInput;
                }

                data += ' ';

                foreach (var wireOutput in _WireOutputs) {
                    data += wireOutput.Text;
                }

                if (ElementType_ComboBox.SelectedIndex == 0) {
                    data += ' ' + TurnoverPosition_TextBox.Text;
                }

                string hex_data = AddRandomBytesToString(data);
                File.WriteAllText(saveFileDialog.FileName, hex_data);
            }
        }

        // Private Methods
        private string AddRandomBytesToString(string data) {
            byte[] bytes = new byte[192 - data.Length];

            var random = new Random();
            var possibleBytes = new List<int>();

            possibleBytes.AddRange(Enumerable.Range(0, 32).ToList());
            possibleBytes.AddRange(Enumerable.Range(33, 32).ToList());
            possibleBytes.AddRange(Enumerable.Range(91, 165).ToList());

            for (int i = 0; i < bytes.Length; i++) {
                bytes[i] = Convert.ToByte(possibleBytes[random.Next(0, possibleBytes.Count)]);
            }

            foreach (var character in Encoding.Default.GetString(bytes)) {
                data = data.Insert(random.Next(0, data.Length), character.ToString());
            }

            string hex_data = CLI_AES.BytesToHex(data);
            hex_data = Regex.Replace(hex_data, ".{32}", "$0\n");
            hex_data = Regex.Replace(hex_data, ".{2}", "$0 ");

            return hex_data;
        }

        private string RemoveRandomBytesFromString(string data) {
            return Regex.Replace(data, "[^A-Z ]", "");
        }

        // Private Variables
        private List<char> _WireInputs;
        private List<TextBox> _WireOutputs;

    }

}