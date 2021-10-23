using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace EnigmaApp.Controls {

    public partial class ComboBoxWithCommand : ComboBox {
        public ComboBoxWithCommand() {
            InitializeComponent();
        }

        // Event
        public static readonly DependencyProperty EventNameProperty = DependencyProperty.Register("EventName", typeof(string), typeof(ComboBoxWithCommand));
        public string EventName {
            get { return (string)GetValue(EventNameProperty); }
            set { SetValue(EventNameProperty, value);  }
        }

        // Command
        public static readonly DependencyProperty CommandProperty = DependencyProperty.Register("Command", typeof(ICommand), typeof(ComboBoxWithCommand));
        public ICommand Command {
            get { return (ICommand)GetValue(CommandProperty); }
            set { SetValue(CommandProperty, value); }
        }

        public static readonly DependencyProperty CommandParameterProperty = DependencyProperty.Register("CommandParameter", typeof(object), typeof(ComboBoxWithCommand));
        public object CommandParameter {
            get { return GetValue(CommandProperty); }
            set { SetValue(CommandProperty, value); }
        }
    }

}