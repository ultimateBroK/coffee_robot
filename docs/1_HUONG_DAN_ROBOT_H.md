# 🤖 HƯỚNG DẪN CHI TIẾT FILE ROBOT.H

## 📋 Mục lục
- [Tổng quan](#tổng-quan)
- [Cấu trúc file](#cấu-trúc-file) 
- [Các hằng số quan trọng](#các-hằng-số-quan-trọng)
- [Enum RobotPhase](#enum-robotphase)
- [Struct Robot](#struct-robot)
- [Cách thay đổi vị trí robot](#cách-thay-đổi-vị-trí-robot)
- [Các hàm chính](#các-hàm-chính)
- [Ví dụ thực tế](#ví-dụ-thực-tế)

## 🎯 Tổng quan

File `robot.h` là **header file** định nghĩa một robot simulation 6-DOF (6 bậc tự do) để pha cà phê. Robot này sử dụng:
- **Forward Kinematics**: Tính toán vị trí từ góc khớp
- **State Machine**: Chuyển đổi giữa các pha hoạt động
- **OpenGL Rendering**: Vẽ 3D graphics

## 📁 Cấu trúc file

```
robot.h
├── Headers & Dependencies
├── Hằng số cơ bản
├── Enum RobotPhase (các pha hoạt động)
├── Struct Robot (trạng thái robot)
├── Function declarations
└── Global robot instance
```

## ⚙️ Các hằng số quan trọng

### 🔧 Thông số cơ bản
```cpp
const float ANIMATION_SPEED = 0.015f;  // Tốc độ animation
const float ARM_UPPER = 1.6f;          // Chiều dài cánh tay trên
const float ARM_LOWER = 1.4f;          // Chiều dài cánh tay dưới  
const float CUP_SIZE = 0.25f;          // Bán kính cốc
const float TABLE_Y = 0.1f;            // Độ cao bàn
```

### 📊 Cách thay đổi thông số

| Thông số | Ý nghĩa | Tăng giá trị | Giảm giá trị |
|----------|---------|-------------|-------------|
| `ANIMATION_SPEED` | Tốc độ chuyển động | Robot di chuyển nhanh hơn | Robot di chuyển chậm hơn |
| `ARM_UPPER` | Cánh tay trên | Tầm với xa hơn | Tầm với gần hơn |
| `ARM_LOWER` | Cánh tay dưới | Linh hoạt hơn | Cứng nhắc hơn |
| `CUP_SIZE` | Kích thước cốc | Cốc to hơn | Cốc nhỏ hơn |
| `TABLE_Y` | Độ cao bàn | Bàn cao hơn | Bàn thấp hơn |

## 🔄 Enum RobotPhase

```cpp
enum RobotPhase {
    IDLE = 0,           // Chờ đợi
    REACH_CUP,          // Di chuyển tới cốc
    GRAB_CUP,           // Cầm lấy cốc
    TURN_TO_MACHINE,    // Quay về phía máy pha cà phê
    POUR_COFFEE,        // Pha cà phê
    RETURN_CUP,         // Mang cốc trở về bàn
    PLACE_CUP,          // Đặt cốc xuống
    GO_HOME,            // Quay về vị trí ban đầu
    FINISHED            // Hoàn thành
};
```

### 🔁 Luồng hoạt động
```
IDLE → REACH_CUP → GRAB_CUP → TURN_TO_MACHINE → 
POUR_COFFEE → RETURN_CUP → PLACE_CUP → GO_HOME → FINISHED
```

## 🤖 Struct Robot

### 🦾 Góc các khớp (Joint Angles)
```cpp
float shoulderY, shoulderZ;     // Xoay vai (Y=ngang, Z=dọc)
float elbow;                    // Gập khuỷu tay (0-180°)
float wristZ, wristY;          // Xoay cổ tay
float fingers;                  // Mở/đóng gripper (0-1)
float body;                     // Xoay thân (-180° đến 180°)
```

### 📈 Trạng thái Animation
```cpp
RobotPhase phase;              // Pha hiện tại
float progress;                // Tiến độ pha (0.0-1.0)
bool moving;                   // Có đang di chuyển không
```

### ☕ Trạng thái cốc
```cpp
float cupX, cupY, cupZ;        // Vị trí cốc trong không gian
bool holdingCup;               // Robot có đang cầm cốc không
bool cupHasCoffee;             // Cốc có cà phê không
```

## 🎮 Cách thay đổi vị trí robot

### 1. Thay đổi góc vai (Shoulder)

```cpp
// Xoay vai theo trục Y (ngang)
robot.shoulderY = 45.0f;   // Xoay 45° sang phải
robot.shoulderY = -30.0f;  // Xoay 30° sang trái

// Xoay vai theo trục Z (dọc)  
robot.shoulderZ = 60.0f;   // Nâng vai lên 60°
robot.shoulderZ = -20.0f;  // Hạ vai xuống 20°
```

### 2. Thay đổi góc khuỷu tay (Elbow)

```cpp
robot.elbow = 0.0f;    // Duỗi thẳng
robot.elbow = 90.0f;   // Gập 90°
robot.elbow = 135.0f;  // Gập gấp
```

### 3. Thay đổi cổ tay (Wrist)

```cpp
// Xoay cổ tay lên/xuống
robot.wristZ = 45.0f;   // Cổ tay hướng lên
robot.wristZ = -45.0f;  // Cổ tay hướng xuống

// Xoắn cổ tay
robot.wristY = 180.0f;  // Xoắn 180°
```

### 4. Điều khiển gripper

```cpp
robot.fingers = 0.0f;   // Đóng hoàn toàn
robot.fingers = 0.5f;   // Mở một nửa
robot.fingers = 1.0f;   // Mở hoàn toàn
```

### 5. Xoay thân robot

```cpp
robot.body = 0.0f;     // Hướng thẳng
robot.body = 90.0f;    // Quay 90° sang phải
robot.body = -90.0f;   // Quay 90° sang trái
robot.body = 180.0f;   // Quay ngược lại
```

## 🛠️ Các hàm chính

### Khởi tạo và điều khiển
```cpp
void initRobot();               // Đưa robot về vị trí khởi tạo
void resetRobot();              // Reset về trạng thái ban đầu
void startCoffeeSequence();     // Bắt đầu quy trình pha cà phê
void updateRobot();             // Cập nhật animation (gọi mỗi frame)
```

### Rendering
```cpp
void drawRobot();               // Vẽ robot
void drawScene();               // Vẽ bàn, máy pha cà phê, v.v.
```

## 💡 Ví dụ thực tế

### Ví dụ 1: Tạo pose "chào"
```cpp
void createGreetingPose() {
    robot.shoulderY = 0.0f;     // Vai thẳng
    robot.shoulderZ = 90.0f;    // Nâng vai lên
    robot.elbow = 45.0f;        // Gập khuỷu nhẹ
    robot.wristZ = 0.0f;        // Cổ tay thẳng
    robot.wristY = 0.0f;        // Không xoắn
    robot.fingers = 0.7f;       // Mở gripper
    robot.body = 0.0f;          // Thân thẳng
}
```

### Ví dụ 2: Tạo pose "nghỉ"
```cpp
void createRestPose() {
    robot.shoulderY = 0.0f;     // Vai thẳng
    robot.shoulderZ = -30.0f;   // Hạ vai xuống
    robot.elbow = 120.0f;       // Gập khuỷu nhiều
    robot.wristZ = -45.0f;      // Cổ tay hướng xuống
    robot.wristY = 0.0f;        // Không xoắn
    robot.fingers = 0.0f;       // Đóng gripper
    robot.body = 0.0f;          // Thân thẳng
}
```

### Ví dụ 3: Điều chỉnh tốc độ
```cpp
// Robot di chuyển chậm hơn
const float ANIMATION_SPEED = 0.005f;

// Robot di chuyển nhanh hơn  
const float ANIMATION_SPEED = 0.03f;
```

### Ví dụ 4: Tăng tầm với của robot
```cpp
// Cánh tay dài hơn
const float ARM_UPPER = 2.0f;
const float ARM_LOWER = 1.8f;

// Cánh tay ngắn hơn
const float ARM_UPPER = 1.2f;  
const float ARM_LOWER = 1.0f;
```

## ⚠️ Lưu ý quan trọng

1. **Giới hạn góc**: Một số khớp có giới hạn góc tự nhiên:
   - Khuỷu tay: 0° đến 180°
   - Gripper: 0.0 đến 1.0

2. **Collision Detection**: Code hiện tại không kiểm tra va chạm, cần cẩn thận khi đặt góc.

3. **Animation Smooth**: Sử dụng `progress` để tạo chuyển động mượt mà giữa các pose.

4. **Coordinate System**: 
   - X: Trái/Phải
   - Y: Lên/Xuống  
   - Z: Trước/Sau

## 🎯 Mẹo sử dụng

- **Debug**: In ra góc các khớp để kiểm tra vị trí
- **Testing**: Thử từng khớp một để hiểu ảnh hưởng
- **Animation**: Sử dụng interpolation cho chuyển động mượt
- **Backup**: Lưu lại pose tốt để sử dụng sau

---
📝 **Ghi chú**: File này sẽ được cập nhật khi có thay đổi trong robot.h 