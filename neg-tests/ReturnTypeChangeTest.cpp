#include "ReturnTypeChange.hpp"

struct StruktWrapper {
    // if Get return type is changed to return by value, then this method
    //  would end up returning a reference to a temporary => compile error
    float& GetMemF() { return m_s.GetMemF(); }
    const float& GetMemF() const { return m_s.GetMemF(); }
    void SetMemF(const float& memF) { m_s.SetMemF(memF); }

private:
    Strukt m_s{};
};

int main()
{
    StruktWrapper s_mut;
    {
        int iVal = static_cast<int>(s_mut.GetMemF());
        float val = s_mut.GetMemF();
        s_mut.SetMemF(s_mut.GetMemF());
        float& valRef = s_mut.GetMemF();
        const float& constValRef = s_mut.GetMemF();
    }

    {
        const StruktWrapper s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        s_mut.SetMemF(s.GetMemF());
        float& valRef = s_mut.GetMemF();
        const float& constValRef = s.GetMemF();
    }
}
