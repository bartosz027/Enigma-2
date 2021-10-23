using System;

using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Media;

using EnigmaApp.Core;
using CLI_Enigma = CLI.ENIGMA.Enigma;

namespace EnigmaApp.ViewModel {

    class PlugboardViewModel : ObservableObject {

        public PlugboardViewModel(CLI_Enigma enigma) {
            Enigma = enigma;

            _DefaultColor = Brushes.DarkGray.GetValue(SolidColorBrush.ColorProperty).ToString();

            _AvailableColors = new List<string>() {
                "#FFFF4040", "#FFFFD100",
                "#FFAEFF00", "#FFECFF00",
                "#FF00A2FF", "#FF23CE4D",
                "#FF00FF80", "#FF8756D3",
                "#FF1745FF", "#FFB34FA3",
                "#FF6100FF", "#FFFF59E9",
                "#FF00FFFB"
            };

            ButtonColors = new ObservableCollection<SolidColorBrush>() {
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray,
                Brushes.DarkGray, Brushes.DarkGray
            };

            _CharToIndexConverter = new List<Tuple<string, int>>() {
                new Tuple<string, int>("Q", 0),  new Tuple<string, int>("W", 1),
                new Tuple<string, int>("E", 2),  new Tuple<string, int>("R", 3),
                new Tuple<string, int>("T", 4),  new Tuple<string, int>("Z", 5),
                new Tuple<string, int>("U", 6),  new Tuple<string, int>("I", 7),
                new Tuple<string, int>("O", 8),  new Tuple<string, int>("A", 9),
                new Tuple<string, int>("S", 10), new Tuple<string, int>("D", 11),
                new Tuple<string, int>("F", 12), new Tuple<string, int>("G", 13),
                new Tuple<string, int>("H", 14), new Tuple<string, int>("J", 15),
                new Tuple<string, int>("K", 16), new Tuple<string, int>("P", 17),
                new Tuple<string, int>("Y", 18), new Tuple<string, int>("X", 19),
                new Tuple<string, int>("C", 20), new Tuple<string, int>("V", 21),
                new Tuple<string, int>("B", 22), new Tuple<string, int>("N", 23),
                new Tuple<string, int>("M", 24), new Tuple<string, int>("L", 25)
            };

            PlugboardButtonCommand = new RelayCommand(obj => {
                var converter = new BrushConverter();
                var button = obj as Button;

                string buttonID = button.Name[0].ToString();
                string buttonColor = button.Background.GetValue(SolidColorBrush.ColorProperty).ToString();

                List<SolidColorBrush> colors = ButtonColors.Where(p => buttonColor == p.GetValue(SolidColorBrush.ColorProperty).ToString()).ToList();

                if (buttonID != _PrevButtonID && buttonColor == _DefaultColor) {
                    if (_PrevButtonID != null) {
                        string plugs = "" + _PrevButtonID + buttonID;
                        Enigma.AddPlugboardConnection(plugs);

                        int index = _CharToIndexConverter.Find(p => p.Item1 == buttonID).Item2;
                        ButtonColors[index] = (SolidColorBrush)converter.ConvertFrom(_AvailableColors[0]);

                        _AvailableColors.RemoveAt(0);
                        _PrevButtonID = null;
                    }
                    else {
                        int index = _CharToIndexConverter.Find(p => p.Item1 == buttonID).Item2;
                        ButtonColors[index] = (SolidColorBrush)converter.ConvertFrom(_AvailableColors[0]);

                        _PrevButtonID = buttonID;
                    }
                }
                else if (colors.Count == 2) {
                    var button_index_1 = ButtonColors.IndexOf(colors[0]);
                    var button_index_2 = ButtonColors.IndexOf(colors[1]);

                    string plugs = "" + _CharToIndexConverter.Find(p => p.Item2 == button_index_1).Item1 + _CharToIndexConverter.Find(p => p.Item2 == button_index_2).Item1;
                    Enigma.RemovePlugboardConnection(plugs);

                    _AvailableColors.Add(colors[0].GetValue(SolidColorBrush.ColorProperty).ToString());

                    ButtonColors[button_index_1] = (SolidColorBrush)converter.ConvertFrom(_DefaultColor);
                    ButtonColors[button_index_2] = (SolidColorBrush)converter.ConvertFrom(_DefaultColor);
                }
                else {
                    int index = _CharToIndexConverter.Find(p => p.Item1 == buttonID).Item2;
                    ButtonColors[index] = (SolidColorBrush)converter.ConvertFrom(_DefaultColor);

                    _PrevButtonID = null;
                }
            });
        }

        public void UpdateButtons() {
            var converter = new BrushConverter();

            for(int i = 0; i < ButtonColors.Count; i++) {
                var color = ButtonColors[i].GetValue(SolidColorBrush.ColorProperty).ToString();

                if(color != _DefaultColor) {
                    _AvailableColors.Add(color);
                }

                ButtonColors[i] = (SolidColorBrush)converter.ConvertFrom(_DefaultColor);
            }

            _AvailableColors = _AvailableColors.Distinct().ToList();

            foreach (var connection in Enigma.PlugboardConnections) {
                var element1 = _CharToIndexConverter.Find(p => p.Item1 == connection[0].ToString());
                int index1 = element1.Item2;

                var element2 = _CharToIndexConverter.Find(p => p.Item1 == connection[1].ToString());
                int index2 = element2.Item2;

                ButtonColors[index1] = (SolidColorBrush)converter.ConvertFrom(_AvailableColors[0]);
                ButtonColors[index2] = (SolidColorBrush)converter.ConvertFrom(_AvailableColors[0]);

                _AvailableColors.RemoveAt(0);
            }
        }

        // Variables
        public CLI_Enigma Enigma { get; set; }
        public ObservableCollection<SolidColorBrush> ButtonColors { get; set; }

        private string _DefaultColor;
        private List<string> _AvailableColors;

        private string _PrevButtonID;
        private List<Tuple<string, int>> _CharToIndexConverter;

        // Commands
        public RelayCommand PlugboardButtonCommand { get; set; }

    }

}