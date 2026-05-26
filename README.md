# Bộ mã hóa mã vòng sử dụng thuật toán nhân (Cyclic Code Encoder)

Triển khai C++17 của bộ mã hóa mã vòng tuyến tính C(l, k) với các phép toán đa thức trên trường GF(2).

## Tác dụng chính

- Thực hiện các phép tính toán trong trường GF(2)
- Mã hóa bản tin thành từ mã hợp lệ
- Biểu diễn hoạt động mạch mã hóa từng xung nhịp

## Các thành phần

| Tệp/Thư mục | Mục Đích |
|-----|---------|
| `cyclic-code.html` | Chương trình mã hóa và in ra từng bước làm để có được đáp án cuối cùng với giao diện trực quan, sinh động |
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


## Yêu Cầu
C++17 trở lên (GCC 7+, Clang 5+, MSVC 2017+)

---
