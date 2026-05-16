# Bộ Mã Hóa Mã Vòng (Cyclic Code Encoder)

Triển khai C++17 của bộ mã hóa mã vòng tuyến tính C(l, k) với các phép toán đa thức trên trường GF(2).

## Tác dụng chính

- Thực hiện các phép tính toán trong trường GF(2)
- Mã hóa bản tin thành từ mã hợp lệ
- Biểu diễn hoạt động mạch LFSR (Linear Feedback Shift Register) từng xung nhịp

## Thuật toán mã hóa

Cho bản tin **m** độ dài k, đa thức kiểm tra **h(x)** độ dài l-k+1:

```
1. Tính g(x) = (x^l + 1) / h(x)           [đa thức sinh]
2. Tính p(x) = (x^r · m(x)) mod g(x)      [bit parity, r=l-k]
3. v(x) = x^r · m(x) + p(x)               [từ mã]
4. Kiểm: v(x) mod g(x) = 0                [xác minh]
```

**Kết quả**: Từ mã v = [m | p] có độ dài l bit, chia hết cho g(x).

## Các thành phần

| Tệp/Thư mục | Mục Đích |
|-----|---------|
| `Bit.hpp` | Tạo lớp Bit đại diện cho 1 phần tử trong trường GF(2) và định nghĩa các thao tác trên Bit |
| `BinaryVector.hpp` | Tạo lớp BinaryVector đại diện cho vector nhị phân độ dài cố định trên GF(2), đùng để biểu diễn bản tin, từ mã, ... |
| `GF2Poly.hpp` | Tạo lớp GF2Poly đại diện cho đa thức trên GF(2) |
| `Helper.hpp` | Cung cấp các hàm hỗ trợ tính toán & in kết quả |
| `main.cpp` | Chương trình chính: mã hóa và in kết quả |
| `tests/` | Thư mục gồm 50 bộ test (mỗi bộ test gồm có *.in và *.out). Các file *.in chứa dữ liệu đầu vào đảm bảo thỏa mãn các điều kiện và tính chất của mã vòng tuyến tính|

## Cách sử dụng

#### Biên dịch
```bash
g++ -std=c++17 -O2 -o encoder main.cpp
```

#### Chạy chương trình
```bash
./encoder
```

#### Định dạng đầu vào
```
l k
h(x)
m
```

- `l`: Độ dài từ mã (bit)
- `k`: Độ dài bản tin (bit)
- `h(x)`: Đa thức kiểm tra (xâu nhị phân MSB-first, l-k+1 bit)
- `m`: Bản tin (xâu nhị phân MSB-first, k bit)

#### Đầu ra
1. Bảng hoạt động LFSR (từng xung nhịp)
2. g(x) = đa thức sinh
3. p = bit parity
4. v = từ mã
5. Xác minh v % g(x) = 0


## Yêu Cầu
C++17 trở lên (GCC 7+, Clang 5+, MSVC 2017+)

---
