## **Đề tài: Robot Pha Cà Phê Tự Động**

### 1. **Phát biểu bài toán**

Xây dựng một mô hình robot 3D có khả năng pha cà phê tự động, bao gồm các chuyển động cơ học cơ bản như xoay thân, cử động tay, nhấc cốc lên, rót cà phê và trả về vị trí ban đầu. Toàn bộ quá trình được hiển thị bằng **OpenGL** và áp dụng các **phép biến đổi hình học 3D**, **hoạt cảnh (animation)**, **ánh sáng (lighting)** và **điều khiển camera**.

---

### 2. **Phương án giải quyết bài toán**

#### 2.1. **Thiết kế mô hình**

- **Robot có phân cấp cấu trúc**:

  - **Thân (body)**: Xoay quanh trục Y (0° → -180°)
  - **Vai (shoulder)**: 
    - shoulderY: Xoay ngang (0° → 30°)
    - shoulderZ: Nâng lên/hạ xuống (-35° → -5°)
  - **Khuỷu tay (elbow)**: Gập/duỗi (-45° → -15°)
  - **Cổ tay (wrist)**: 
    - wristZ: Nâng lên/hạ xuống (0° → 55°)
    - wristY: Xoay cổ tay (-90° → 0°)
  - **Gripper (fingers)**: Mở/đóng (90° → 30°)

- **Đối tượng đi kèm**:

  - Bàn đặt cốc (TABLE_Y = 0.1f)
  - Máy pha cà phê (vị trí X = -4.0f)
  - Cốc cà phê (bán kính CUP_SIZE = 0.25f)
  - Hiển thị trạng thái (text overlay)

#### 2.2. **Các công nghệ áp dụng**

- **Phép biến đổi 3D**:

  - `glTranslatef`, `glRotatef`, `glScalef` cho từng khớp tay
  - Phân cấp transformation (hierarchical modeling)

- **Hoạt cảnh (Animation)**:

  - Animation dựa trên phase (8 phase từ IDLE đến FINISHED)
  - Hàm làm mượt `smooth(t)` = t² × (3 - 2t)
  - Nội suy tuyến tính giữa các góc khớp

- **Chiếu sáng (Lighting)**:

  - Ánh sáng khuếch tán từ phía trên phải (5, 10, 5)
  - Ánh sáng môi trường nhẹ (0.3, 0.3, 0.3)
  - Màu sắc vật liệu với glColorMaterial

- **Điều khiển camera**:

  - Xoay camera bằng chuột (cameraAngleX, cameraAngleY)
  - Zoom in/out với phím mũi tên (cameraDistance)
  - Perspective projection với gluPerspective

---

### 3. **Lập trình xử lý, demo kết quả**

#### 3.1. **Cấu trúc mã nguồn**

```bash
robot.h                 # Khai báo cấu trúc và hằng số
robot.cpp               # Triển khai logic robot và animation
coffee_robot.cpp        # Chương trình chính, camera và UI
Visualization/          # Tài liệu hướng dẫn và tài liệu kỹ thuật
Makefile                # Biên dịch project
```

#### 3.2. **Tính năng mô phỏng**

Robot thực hiện quy trình pha cà phê qua 8 phase:

1. **REACH_CUP**: Di chuyển tay đến vị trí cốc
2. **GRAB_CUP**: Nắm lấy cốc và nhấc lên
3. **TURN_TO_MACHINE**: Xoay thân 180° về phía máy pha
4. **POUR_COFFEE**: Giữ cốc dưới vòi pha
5. **RETURN_CUP**: Mang cốc trở về bàn
6. **PLACE_CUP**: Đặt cốc xuống bàn
7. **GO_HOME**: Trở về vị trí ban đầu
8. **FINISHED**: Hoàn thành quy trình

#### 3.3. **Tùy chọn điều khiển**

- **Space**: Bắt đầu quy trình pha cà phê
- **R**: Reset robot về trạng thái ban đầu
- **ESC**: Thoát chương trình
- **Chuột trái**: Kéo để xoay camera
- **Mũi tên lên/xuống**: Zoom in/out

---

### 4. **Trình bày (bảo vệ)**

#### 4.1. **Trình bày trực tiếp**

1. **Mục tiêu đề tài**:
   - Mô phỏng robot pha cà phê với các chuyển động cơ học thực tế
   - Áp dụng kiến thức đồ họa máy tính: phép biến đổi 3D, animation, lighting
   - Xây dựng hệ thống animation dựa trên phase với chuyển động mượt mà

2. **Tư duy thiết kế mô hình phân cấp**:
   - Cấu trúc phân cấp: thân → vai → cánh tay trên → khuỷu → cánh tay dưới → cổ tay → gripper
   - Hệ thống transformation ma trận lồng nhau (nested transformations)
   - Tối ưu hóa phép biến đổi (vị trí cốc theo gripper)

3. **Chi tiết thuật toán điều khiển hoạt cảnh**:
   - Hàm smooth(t) = t² × (3 - 2t) tạo chuyển động mượt mà
   - Nội suy tuyến tính giữa các góc khớp: start + s × (end - start)
   - Phân chia animation thành 8 phase riêng biệt
   - Sử dụng reverse = (1-s) để đảo ngược chuyển động

4. **Kết quả đạt được**:
   - Chuyển động robot mượt mà và tự nhiên
   - Điều khiển camera linh hoạt
   - Hiển thị trạng thái trực quan
   - Tối ưu hóa code với hàm advancePhase() và vẽ cốc

5. **Những vấn đề gặp phải & cách khắc phục**:
   - Vấn đề: Chuyển động giật cục → Giải pháp: Hàm smooth() và easeReverse
   - Vấn đề: Code lặp lại → Giải pháp: Hàm advancePhase()
   - Vấn đề: Hiệu suất vẽ cốc → Giải pháp: Tối ưu phép biến đổi

6. **Hướng mở rộng**:
   - Thêm texture cho các bề mặt
   - Thêm hiệu ứng hạt cà phê và hơi nước
   - Thêm nhiều robot hoạt động đồng thời
   - Thêm tùy chọn điều khiển trực tiếp từng khớp

#### 4.2. **Demo trực tiếp**

- Chạy chương trình với góc nhìn camera bay quanh
- Cho robot chạy các hoạt cảnh
- Phân tích các phép biến đổi đang diễn ra trong khi robot hoạt động

#### 4.3. **Câu hỏi chuẩn bị trước**

- **Robot được thiết kế phân cấp như thế nào?**
  - Robot được thiết kế theo cấu trúc phân cấp: thân → vai → cánh tay trên → khuỷu → cánh tay dưới → cổ tay → gripper.
  - Mỗi khớp kế thừa phép biến đổi của khớp cha, tạo thành chuỗi biến đổi lồng nhau.
  - Phân cấp này cho phép điều khiển từng khớp độc lập nhưng vẫn duy trì mối quan hệ giữa các bộ phận.

- **Sử dụng animation time-based hay event-based?**
  - Kết hợp cả hai: time-based trong mỗi phase (robot.progress tăng theo thời gian) và event-based khi chuyển phase (khi progress >= 1.0).
  - Mỗi frame, tiến độ tăng thêm ANIMATION_SPEED (0.015).
  - Hàm smooth(t) chuyển đổi tiến độ tuyến tính thành chuyển động mượt mà.

- **Nếu yêu cầu đổi tay phải thành tay trái thì bạn sửa phần nào trong code?**
  - Đổi dấu của shoulderY (xoay vai theo chiều ngược lại)
  - Đổi dấu của wristY (xoay cổ tay theo chiều ngược lại)
  - Đổi vị trí cốc sang bên phải (robot.cupX = -2.2f thay vì 2.2f)
  - Đổi vị trí máy pha cà phê sang bên phải (X = 4.0f thay vì -4.0f)
  - Đổi góc xoay thân trong phase TURN_TO_MACHINE (180° thay vì -180°)

- **Hàm smooth() hoạt động như thế nào và tại sao cần nó?**
  - Hàm smooth(t) = t² × (3 - 2t) là một hàm nội suy Hermite bậc 3.
  - Nó chuyển đổi chuyển động tuyến tính thành chuyển động có gia tốc/giảm tốc tự nhiên.
  - Đạo hàm tại t=0 và t=1 đều bằng 0, tạo ra chuyển động mượt mà ở đầu và cuối.
  - Không có smooth(), robot sẽ chuyển động máy móc, giật cục, thiếu tự nhiên.

---

## ✅ **Tóm tắt lợi thế của đề tài**

| Yếu tố                   | Mức độ     | Ghi chú                                                                 |
| ------------------------ | ---------- | ----------------------------------------------------------------------- |
| Tính sáng tạo            | ⭐⭐⭐⭐☆  | Robot ứng dụng thực tế, chưa ai làm                                     |
| Dễ trình bày             | ⭐⭐⭐⭐⭐ | Có cử động cụ thể, dễ diễn giải, có tài liệu hướng dẫn                  |
| Đáp ứng yêu cầu kỹ thuật | ⭐⭐⭐⭐⭐ | Đầy đủ animation, lighting, biến đổi 3D, camera control                 |
| Khả năng mở rộng / sửa   | ⭐⭐⭐⭐⭐ | Cấu trúc modular, dễ thêm tính năng, dễ điều chỉnh animation            |
