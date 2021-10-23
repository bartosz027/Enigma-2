using System.Windows;

namespace EnigmaApp.Controls {

    public partial class PasswordBoxWindow : Window {
        public PasswordBoxWindow() {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e) {
            if (Password_TextBox.Password != "") {
                DialogResult = true;
            }
            else {
                MessageBox.Show("Password field cannot be empty!", "Error");
                this.Show();
            }
        }
    }

}