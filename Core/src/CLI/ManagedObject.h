#pragma once
#include <msclr\marshal_cppstd.h>

namespace CLI {

    template<typename T>
    public ref class ManagedObject {
    public:
        ManagedObject(T* instance)
            : m_Instance(instance) {
        }

        virtual ~ManagedObject() {
            if (m_Instance != nullptr) {
                delete m_Instance;
            }
        }

        !ManagedObject() {
            if (m_Instance != nullptr) {
                delete m_Instance;
            }
        }
    protected:
        T* m_Instance;
    };

    template<typename T2, typename T1>
    T2 ConvertTo(T1 variable) {
        return msclr::interop::marshal_as<T2>(variable);
    }

}