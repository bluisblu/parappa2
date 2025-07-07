#ifndef NALIB_NAMATRIX_H
#define NALIB_NAMATRIX_H

#include "nalib/navector.h"

template <typename T, int t0, int t1>
class NaMATRIX {
public:
    const NaVECTOR<T, t0>& operator[](int arg0) const {
        return m[arg0];
    }

    bool inl0() const {
        const NaMATRIX<float, 4, 4>& a0 = NaMATRIX<float, 4, 4>::IDENT;
        for (int i = 0; i < t1; i++) {
            if (a0[i].inl0(this->m[i])) {
                return false;
            }
        }

        return true;
    }

    bool inl1() const {
        for (int i = 0; i < t1; i++) {
            if (this->m[i].inl1()) {
                return true;
            }
        }
        
        return false;
    }

private:
    NaVECTOR<T, t0> m[t1];

public:
    static NaMATRIX<float, 4, 4> IDENT;
};

#endif /* NALIB_NAMATRIX_H */
