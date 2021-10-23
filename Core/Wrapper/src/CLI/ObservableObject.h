#pragma once
#using <System.dll>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Runtime::CompilerServices;

namespace CLI {

	public ref class ObservableObject : INotifyPropertyChanged {
    public:
        virtual event PropertyChangedEventHandler^ PropertyChanged;

        void OnPropertyChanged(String^ info) {
            PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
        }
	};

}