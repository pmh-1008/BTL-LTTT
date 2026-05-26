/*
    THUAT TOAN NHAN  (tao tu ma he thong tu h(x))

        Ky hieu:
            l      -- do dai tu ma
            k      -- so bit ban tin,   r = l-k bit kiem tra
            a_i    -- bit thu i cua ban tin  (i = 0..k-1)
                      a_i la he so x^i cua m(x)
            h_j    -- he so x^j cua h(x)    (j = 0..k-1)
                      (bo qua h_k = 1 vi no khong anh huong)
            c_i    -- bit thu i cua tu ma   (i = 0..l-1)
    -----------------------------------------------------------------
 */

#include "Bit.hpp"
#include "BinaryVector.hpp"
#include "GF2Poly.hpp"
#include "Helper.hpp"

#include <bits/stdc++.h>
using namespace std;

bool checkInput(int l, int k, string hStr, string mStr)
{
    if (k >= l)
    {
        cout << "Vi pham rang buoc: l > k";
        return false;
    }

    if (hStr.size() != k + 1)
    {
        cout << "Vi pham rang buoc: hStr phai co k + 1 bit";
        return false;
    }

    GF2Poly hx = GF2Poly::fromBitString(hStr);
    GF2Poly xl1 = GF2Poly::monomial(l) + GF2Poly({1});
    auto [g, rem] = GF2Poly::divmod(xl1, hx);
    if (!rem.isZero())
    {
        cout << "Vi pham rang buoc: x^l+1 chia het cho h(x)";
        return false;
    }

    if (mStr.size() != k)
    {
        cout << "Vi pham rang buoc: mStr phai co k + 1 bit";
        return false;
    }
    
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int l, k;
    string hStr, mStr;
    cin >> l >> k >> hStr >> mStr;

    if (!checkInput(l, k, hStr, mStr))
        return 0;

    const int r = l - k;
    GF2Poly hx = GF2Poly::fromBitString(hStr);

    BinaryVector c = VectorHelper::computeCodeword_mul(l, k, hx, mStr);
    cout << "c = [" << c << "]\n";

    PrintHelper::printStateTable(l, k, hx, mStr);

    return 0;
}
