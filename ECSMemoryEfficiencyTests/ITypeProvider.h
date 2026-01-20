#pragma once
#include <vector>

class ITypeProvider {
public:
    virtual ~ITypeProvider() = default;
    virtual void Cleanup(void* elements) = 0;
};

template<typename T>
concept Disposable = requires(T t) {
    t.Dispose();
};

template<Disposable T>
class C : public ITypeProvider {
public:
    void Cleanup(void* elements) override {
        auto& vec = *static_cast<std::vector<T>*>(elements);
        for (auto& e : vec) {
            e.Dispose();
        }
    }
};

