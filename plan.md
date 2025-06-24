## **Đề tài: Robot Pha Cà Phê Tự Động**

### 1. **Phát biểu bài toán**

Xây dựng một mô hình robot 3D có khả năng pha cà phê tự động, bao gồm các chuyển động cơ học cơ bản như xoay thân, cử động tay, nhấc cốc lên, rót cà phê và trả về vị trí ban đầu. Toàn bộ quá trình được hiển thị bằng **OpenGL** và áp dụng các **phép biến đổi hình học 3D**, **hoạt cảnh (animation)**, **ánh sáng (lighting)** và **texture mapping**.

---

### 2. **Phương án giải quyết bài toán**

#### 2.1. **Thiết kế mô hình**

- **Robot có phân cấp cấu trúc**:

  - **Thân** cố định
  - **Cánh tay dưới**: xoay quanh trục vai
  - **Cánh tay trên**: nâng lên / hạ xuống
  - **Bàn tay**: mở ra kẹp ly và quay về

- **Đối tượng đi kèm**:

  - Bàn đặt ly
  - Máy pha cà phê (texture panel)
  - Cốc cà phê (glutSolidCylinder + texture "coffee surface")
  - Hơi nước (mô phỏng bằng hình nón bán trong suốt + animation)

#### 2.2. **Các công nghệ áp dụng**

- **Phép biến đổi 3D**:

  - `glTranslatef`, `glRotatef`, `glScalef` cho từng khớp tay

- **Hoạt cảnh (Animation)**:

  - Tay robot hoạt động theo từng giai đoạn (state-based)
  - Mỗi phần chuyển động theo thời gian (deltaTime)

- **Chiếu sáng (Lighting)**:

  - Một nguồn sáng chiếu từ phía trần, tạo hiệu ứng bóng nhẹ
  - Gợi cảm giác ánh sáng phòng bếp

- **Texture Mapping**:

  - Dán ảnh cho máy pha cà phê, bàn, cốc
  - Logo thương hiệu cà phê lên tường

---

### 3. **Lập trình xử lý, demo kết quả**

#### 3.1. **Cấu trúc mã nguồn**

```bash
main.cpp               # Chương trình chính
robot.cpp/.h           # Cấu trúc và hoạt cảnh robot
object.cpp/.h          # Các đối tượng phụ: bàn, máy pha, cốc
texture.cpp/.h         # Load và áp dụng texture
animation.cpp/.h       # Hệ thống điều khiển hoạt cảnh theo thời gian
light.cpp/.h           # Thiết lập và điều chỉnh ánh sáng
```

#### 3.2. **Tính năng mô phỏng**

1. Robot quay về phía cốc
2. Hạ tay, nhấc cốc
3. Xoay thân qua máy pha
4. Hạ tay rót cà phê
5. Quay lại trả cốc về bàn

#### 3.3. **Tùy chọn điều khiển**

- Nhấn phím **Space** để bắt đầu chuỗi pha cà phê
- Nhấn **R** để reset về trạng thái ban đầu
- Chuột để xoay góc nhìn xung quanh robot

---

### 4. **Trình bày (bảo vệ)**

#### 4.1. **Slides trình bày**

1. **Mục tiêu đề tài**
2. **Tư duy thiết kế mô hình phân cấp**
3. **Chi tiết thuật toán điều khiển hoạt cảnh**
4. **Kết quả đạt được (ảnh động, quay video nếu có)**
5. **Những vấn đề gặp phải & cách khắc phục**
6. **Hướng mở rộng**:

   - Pha trà
   - Mô phỏng quán cà phê nhỏ (nhiều robot)

#### 4.2. **Demo trực tiếp**

- Chạy chương trình với góc nhìn camera bay quanh
- Cho robot chạy các hoạt cảnh
- Phân tích các phép biến đổi đang diễn ra trong khi robot hoạt động

#### 4.3. **Câu hỏi chuẩn bị trước**

- Robot được thiết kế phân cấp như thế nào?
- Sử dụng animation time-based hay event-based?
- Texture bạn dùng là loại nào? File ảnh có xử lý gì đặc biệt không?
- Nếu yêu cầu đổi tay phải thành tay trái thì bạn sửa phần nào trong code?

---

## ✅ **Tóm tắt lợi thế của đề tài**

| Yếu tố                   | Mức độ     | Ghi chú                                                                 |
| ------------------------ | ---------- | ----------------------------------------------------------------------- |
| Tính sáng tạo            | ⭐⭐⭐⭐☆  | Robot ứng dụng thực tế, chưa ai làm                                     |
| Dễ trình bày             | ⭐⭐⭐⭐☆  | Có cử động cụ thể, dễ diễn giải                                         |
| Đáp ứng yêu cầu kỹ thuật | ⭐⭐⭐⭐⭐ | Đầy đủ texture, animation, lighting, biến đổi 3D                        |
| Khả năng mở rộng / sửa   | ⭐⭐⭐⭐⭐ | Đổi animation hoặc tay cầm rất dễ, thuận lợi khi thầy yêu cầu chỉnh sửa |
