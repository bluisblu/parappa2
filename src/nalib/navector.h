#ifndef NALIB_NAVECTOR_H
#define NALIB_NAVECTOR_H

template <typename T, int t0>
class NaVECTOR {
public:
    T operator[](int arg0) const {
        return v[arg0];
    }

    bool inl0(const NaVECTOR<T, t0>& arg0) const {
        for (int i = 0; i < t0; i++) {
            if (arg0[i] != v[i]) {
                return true;
            }
        }
        
        return false;
    }

    bool inl1() const {
        for (int i = 0; i < t0; i++) {
            if (v[i] != 0.0f) {
                return true;
            }
        }
        
        return false;
    }

private:
    T v[t0];
};

#endif