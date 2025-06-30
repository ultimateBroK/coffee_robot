# 🤖 HƯỚNG DẪN CHI TIẾT FILE ROBOT_SIMPLE.CPP

## 📋 Mục lục
- [Tổng quan](#tổng-quan)
- [Cấu trúc file](#cấu-trúc-file)
- [Biến toàn cục](#biến-toàn-cục)
- [Hàm animation](#hàm-animation)
- [Hệ thống cập nhật robot](#hệ-thống-cập-nhật-robot)
- [Hệ thống rendering](#hệ-thống-rendering)
- [Cách thay đổi vị trí và chuyển động](#cách-thay-đổi-vị-trí-và-chuyển-động)
- [Cách thay đổi màu sắc và hình dạng](#cách-thay-đổi-màu-sắc-và-hình-dạng)
- [Ví dụ tùy chỉnh](#ví-dụ-tùy-chỉnh)
- [Troubleshooting](#troubleshooting)

## 🎯 Tổng quan

File `robot_simple.cpp` là **implementation chính** của robot simulation. Nó bao gồm:
- **Animation System**: Chuyển động mượt mà giữa các pha
- **Forward Kinematics**: Tính toán vị trí từ góc khớp
- **Rendering Engine**: Vẽ robot và môi trường 3D
- **State Management**: Quản lý trạng thái robot

## 📁 Cấu trúc file

```
robot_simple.cpp
├── Headers & Global Variables
├── Animation Utilities (smooth function)
├── Robot Control Functions
│   ├── initRobot()
│   ├── startCoffeeSequence()
│   ├── resetRobot()
│   └── updateRobot() ⭐ CORE FUNCTION
├── Basic Drawing Functions
├── Robot Rendering
└── Scene Rendering
```

## 🌐 Biến toàn cục

```cpp
Robot robot;  // Instance duy nhất của robot trong scene
```

**Ý nghĩa**: Đây là biến toàn cục lưu trữ toàn bộ trạng thái robot:
- Góc tất cả khớp
- Pha animation hiện tại
- Vị trí cốc
- Trạng thái gripper

## 🎬 Hàm Animation

### 📊 Hàm smooth()
```cpp
float smooth(float t) {
    return t * t * (3.0f - 2.0f * t);
}
```

**Mục đích**: Tạo đường cong animation mượt mà (ease in/out)
- **Input**: `t` từ 0.0 đến 1.0 (tiến độ tuyến tính)
- **Output**: Giá trị mượt mà từ 0.0 đến 1.0
- **Đặc điểm**: Bắt đầu chậm → tăng tốc → chậm lại ở cuối

### 🔧 Cách thay đổi animation curve:

| Công thức | Hiệu ứng |
|-----------|----------|
| `t` | Tuyến tính (đều đặn) |
| `t * t` | Ease in (bắt đầu chậm) |
| `1 - (1-t)*(1-t)` | Ease out (kết thúc chậm) |
| `t * t * (3.0f - 2.0f * t)` | Ease in/out (hiện tại) |

## 🏗️ Hệ thống cập nhật robot

### 🎮 updateRobot() - Hàm cốt lõi

Đây là **hàm quan trọng nhất**, được gọi mỗi frame để cập nhật animation:

```cpp
void updateRobot() {
    if (!robot.moving) return;
    
    robot.progress += ANIMATION_SPEED;  // Tăng tiến độ
    float t = robot.progress;
    float s = smooth(t);  // Phiên bản mượt
    
    switch (robot.phase) {
        // Xử lý từng pha...
    }
}
```

### 🔄 Chi tiết từng pha:

#### 1️⃣ **REACH_CUP** - Di chuyển tới cốc
```cpp
case REACH_CUP:
    robot.shoulderZ = s * -30.0f;  // Hạ vai xuống
    robot.elbow = s * -50.0f;      // Gập khuỷu tay
    robot.wristZ = s * 40.0f;      // Nâng cổ tay
    robot.shoulderY = s * 10.0f;   // Xoay vai về cốc
    robot.fingers = 90.0f;         // Giữ gripper mở
```

**Cách thay đổi**:
- Tăng `-30.0f` → Robot hạ vai sâu hơn
- Tăng `-50.0f` → Robot gập khuỷu nhiều hơn
- Tăng `40.0f` → Cổ tay nâng cao hơn
- Tăng `10.0f` → Xoay vai nhiều hơn về phía cốc

#### 2️⃣ **GRAB_CUP** - Cầm cốc
```cpp
case GRAB_CUP:
    robot.wristY = s * -90.0f;  // Xoay gripper
    
    if (s < 0.5f) {
        robot.fingers = 90.0f;  // 50% đầu: chỉ xoay
    } else {
        float gripProgress = (s - 0.5f) / 0.5f;
        robot.fingers = 90.0f - gripProgress * 60.0f;  // 50% sau: đóng gripper
    }
    
    if (s > 0.7f) {
        robot.holdingCup = true;  // Bắt đầu cầm cốc
        robot.shoulderZ = -30.0f + (s - 0.7f) / 0.3f * 8.0f;  // Nhấc cốc lên
    }
```

**Cách thay đổi**:
- Thay đổi `0.5f` → Điều chỉnh thời điểm bắt đầu đóng gripper
- Thay đổi `60.0f` → Độ đóng của gripper (tăng = đóng chặt hơn)
- Thay đổi `0.7f` → Thời điểm bắt đầu nhấc cốc
- Thay đổi `8.0f` → Độ cao nhấc cốc

#### 3️⃣ **TURN_TO_MACHINE** - Xoay về máy pha cà phê
```cpp
case TURN_TO_MACHINE:
    robot.body = s * -180.0f;           // Xoay thân 180°
    robot.shoulderY = 10.0f + s * 15.0f; // Điều chỉnh vai ngang
    robot.shoulderZ = -22.0f + s * 17.0f; // Nâng vai lên
    robot.elbow = -50.0f + s * 30.0f;    // Duỗi khuỷu
    robot.wristZ = 40.0f + s * 50.0f;    // Nâng cổ tay
```

**Cách thay đổi**:
- Thay đổi `-180.0f` → Góc xoay thân (có thể là -90°, -270°)
- Các giá trị khác điều chỉnh tư thế tay khi xoay

#### 4️⃣ **POUR_COFFEE** - Pha cà phê
```cpp
case POUR_COFFEE:
    robot.wristZ = 90.0f;   // Cốc thẳng đứng
    robot.wristY = -90.0f;  // Gripper hướng đúng
    
    if (robot.progress >= 1.5f) {  // Đợi lâu hơn
        robot.cupHasCoffee = true;
    }
```

**Cách thay đổi**:
- Thay đổi `1.5f` → Thời gian pha cà phê (tăng = chậm hơn)
- Thay đổi `90.0f` → Góc nghiêng cốc

#### 5️⃣ **RETURN_CUP** - Quay về bàn
```cpp
case RETURN_CUP: {
    float reverse = 1.0f - s;  // Đảo ngược chuyển động
    robot.body = reverse * -180.0f;
    // ... các chuyển động khác đảo ngược
}
```

#### 6️⃣ **PLACE_CUP** - Đặt cốc xuống
```cpp
case PLACE_CUP:
    robot.shoulderZ = -30.0f - s * 8.0f;  // Hạ vai để đặt cốc
    robot.wristZ = 40.0f - s * 10.0f;     // Hạ cổ tay
    robot.wristY = -90.0f + s * 90.0f;    // Xoay gripper về ban đầu
    
    if (s > 0.6f) {
        robot.holdingCup = false;  // Thả cốc
        float releaseProgress = (s - 0.6f) / 0.4f;
        robot.fingers = 30.0f + releaseProgress * 60.0f;  // Mở gripper
    }
```

#### 7️⃣ **GO_HOME** - Về vị trí ban đầu
```cpp
case GO_HOME: {
    float reverse2 = 1.0f - s;
    robot.shoulderY = reverse2 * 10.0f;
    robot.shoulderZ = reverse2 * -38.0f;
    robot.elbow = reverse2 * -50.0f;
    robot.wristZ = reverse2 * 30.0f;
    robot.wristY = 0.0f;
    robot.fingers = 90.0f;
}
```

## 🎨 Hệ thống Rendering

### 🧱 Hàm vẽ cơ bản

#### drawSimpleBox()
```cpp
void drawSimpleBox(float w, float h, float d, float r, float g, float b) {
    glColor3f(r, g, b);  // Đặt màu RGB
    glPushMatrix();
    glScalef(w, h, d);   // Thay đổi kích thước
    glutSolidCube(1.0f); // Vẽ khối lập phương
    glPopMatrix();
}
```

#### drawSimpleCylinder()
```cpp
void drawSimpleCylinder(float radius, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);  // Xoay để đứng thẳng
    gluCylinder(quad, radius, radius, height, 20, 1);
    gluDeleteQuadric(quad);
}
```

### 🤖 Vẽ Robot

#### drawRobotBase() - Đế robot
```cpp
void drawRobotBase() {
    // Đế robot
    glTranslatef(0, -1, 0);
    drawSimpleBox(1.8f, 0.2f, 1.8f, 0.3f, 0.3f, 0.4f);
    
    // Cột trung tâm
    drawSimpleCylinder(0.5f, 1.0f, 0.5f, 0.5f, 0.6f);
}
```

#### drawRobotArm() - Cánh tay robot
```cpp
void drawRobotArm() {
    // Di chuyển đến vị trí vai
    glTranslatef(0, 1, 0);
    glRotatef(robot.shoulderY, 0, 1, 0);  // Xoay vai ngang
    glRotatef(robot.shoulderZ, 0, 0, 1);  // Xoay vai dọc
    
    // Khớp vai
    glutSolidSphere(0.4f, 15, 15);
    
    // Cánh tay trên
    glTranslatef(ARM_UPPER/2, 0, 0);
    drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 0.6f, 0.6f, 0.7f);
    
    // ... tiếp tục với khuỷu tay, cánh tay dưới, cổ tay, gripper
}
```

### 🏠 Vẽ môi trường

#### drawTable() - Mặt bàn
```cpp
void drawTable() {
    drawSimpleBox(10.0f, 0.2f, 8.0f, 0.55f, 0.35f, 0.25f);  // Nâu gỗ
}
```

#### drawCoffeeMachine() - Máy pha cà phê
```cpp
void drawCoffeeMachine() {
    glTranslatef(-4, TABLE_Y + 1.1f, 0);  // Vị trí bên trái bàn
    
    // Thân máy
    drawSimpleBox(1.5f, 2.2f, 1.4f, 0.6f, 0.6f, 0.65f);
    
    // Vòi pha
    glTranslatef(0, -0.2f, 0.8f);
    drawSimpleCylinder(0.1f, 0.5f, 0.3f, 0.3f, 0.3f);
}
```

#### drawCup() - Cốc cà phê
```cpp
void drawCup() {
    if (robot.holdingCup) {
        // Tính toán vị trí cốc theo forward kinematics
        // Áp dụng tất cả transformation từ thân → vai → khuỷu → cổ tay → gripper
    } else {
        // Cốc đặt trên bàn
        glTranslatef(robot.cupX, robot.cupY, robot.cupZ);
    }
    
    // Vẽ cốc bằng cylinder
    gluCylinder(quad, CUP_SIZE, CUP_SIZE, 0.5f, 20, 1);
    
    // Vẽ cà phê nếu có
    if (robot.cupHasCoffee) {
        glColor3f(0.4f, 0.2f, 0.1f);  // Màu nâu cà phê
        gluDisk(quad, 0, CUP_SIZE * 0.9f, 20, 1);
    }
}
```

## 🎮 Cách thay đổi vị trí và chuyển động

### 1. **Thay đổi vị trí ban đầu của cốc**
```cpp
// Trong initRobot()
robot.cupX = 2.2f;  // Gần/xa robot (tăng = xa hơn)
robot.cupY = TABLE_Y + 0.25f;  // Độ cao (cố định trên bàn)
robot.cupZ = 0.0f;  // Trái/phải (tăng = về phía trước)
```

### 2. **Thay đổi tốc độ animation**
```cpp
// Trong updateRobot()
robot.progress += ANIMATION_SPEED;  // Tăng ANIMATION_SPEED = nhanh hơn

// Hoặc thay đổi trong robot.h:
const float ANIMATION_SPEED = 0.03f;  // Nhanh hơn
const float ANIMATION_SPEED = 0.008f; // Chậm hơn
```

### 3. **Thay đổi vị trí máy pha cà phê**
```cpp
// Trong drawCoffeeMachine()
glTranslatef(-4, TABLE_Y + 1.1f, 0);  // (-4 = bên trái, +4 = bên phải)
```

### 4. **Thay đổi độ cao robot**
```cpp
// Trong drawRobot()
glTranslatef(0, 1.1f, 0);  // Tăng 1.1f = robot cao hơn
```

### 5. **Thay đổi pose khởi tạo**
```cpp
// Trong initRobot()
robot.shoulderY = 15.0f;  // Robot bắt đầu xoay sang phải
robot.shoulderZ = -10.0f; // Robot bắt đầu hạ vai
robot.elbow = 30.0f;      // Robot bắt đầu gập khuỷu
```

## 🎨 Cách thay đổi màu sắc và hình dạng

### 🌈 Bảng màu RGB

| Màu | R | G | B | Mô tả |
|-----|---|---|---|-------|
| Đỏ | 1.0f | 0.0f | 0.0f | Màu đỏ thuần |
| Xanh lá | 0.0f | 1.0f | 0.0f | Màu xanh lá thuần |
| Xanh dương | 0.0f | 0.0f | 1.0f | Màu xanh dương thuần |
| Vàng | 1.0f | 1.0f | 0.0f | Đỏ + Xanh lá |
| Tím | 1.0f | 0.0f | 1.0f | Đỏ + Xanh dương |
| Cam | 1.0f | 0.5f | 0.0f | Đỏ + một nửa xanh lá |
| Xám | 0.5f | 0.5f | 0.5f | Tất cả kênh bằng nhau |

### 🎨 Thay đổi màu robot
```cpp
// Trong drawRobotArm() - cánh tay
drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);  // Đỏ
drawSimpleBox(ARM_LOWER, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f);  // Xanh lá

// Trong drawRobotBase() - đế
drawSimpleBox(1.8f, 0.2f, 1.8f, 0.0f, 0.0f, 1.0f);  // Xanh dương
```

### 📏 Thay đổi kích thước robot
```cpp
// Trong drawRobotArm() - cánh tay to hơn
drawSimpleBox(ARM_UPPER * 1.5f, 0.8f, 0.8f, 0.6f, 0.6f, 0.7f);

// Trong drawRobotBase() - đế to hơn  
drawSimpleBox(2.5f, 0.3f, 2.5f, 0.3f, 0.3f, 0.4f);
```

### ☕ Thay đổi cốc
```cpp
// Trong drawCup() - cốc to hơn
const float NEW_CUP_SIZE = 0.4f;  // Thay vì CUP_SIZE
gluCylinder(quad, NEW_CUP_SIZE, NEW_CUP_SIZE, 0.8f, 20, 1);  // Cao hơn

// Màu cốc khác
glColor3f(0.8f, 0.9f, 1.0f);  // Xanh nhạt thay vì trắng
```

## 💡 Ví dụ tùy chỉnh

### Ví dụ 1: Robot chậm và êm ái
```cpp
// Trong robot.h
const float ANIMATION_SPEED = 0.005f;  // Chậm hơn 3 lần

// Trong updateRobot() - REACH_CUP
robot.shoulderZ = s * -20.0f;  // Chuyển động nhẹ nhàng hơn
robot.elbow = s * -30.0f;      // Gập ít hơn
robot.wristZ = s * 25.0f;      // Nâng nhẹ hơn
```

### Ví dụ 2: Robot nhanh và mạnh mẽ
```cpp
// Trong robot.h
const float ANIMATION_SPEED = 0.03f;  // Nhanh gấp đôi

// Trong updateRobot() - REACH_CUP  
robot.shoulderZ = s * -45.0f;  // Chuyển động mạnh mẽ hơn
robot.elbow = s * -70.0f;      // Gập nhiều hơn
robot.wristZ = s * 60.0f;      // Nâng cao hơn
```

### Ví dụ 3: Robot màu sắc
```cpp
// Cánh tay rainbow
void drawRobotArm() {
    // Cánh tay trên - đỏ
    drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
    
    // Cánh tay dưới - xanh lá
    drawSimpleBox(ARM_LOWER, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f);
    
    // Gripper - xanh dương
    drawSimpleBox(0.3f, 0.4f, 0.3f, 0.0f, 0.0f, 1.0f);
}
```

### Ví dụ 4: Cốc khổng lồ
```cpp
// Trong initRobot()
robot.cupX = 1.8f;  // Gần hơn để robot với tới

// Trong drawCup()
const float GIANT_CUP_SIZE = 0.5f;  // Gấp đôi kích thước
gluCylinder(quad, GIANT_CUP_SIZE, GIANT_CUP_SIZE, 1.0f, 20, 1);  // Cao gấp đôi

// Trong updateRobot() - GRAB_CUP
robot.fingers = 90.0f - gripProgress * 80.0f;  // Đóng rộng hơn cho cốc to
```

### Ví dụ 5: Thêm hiệu ứng rung
```cpp
// Trong updateRobot() - POUR_COFFEE
float shake = sin(robot.progress * 50.0f) * 2.0f;  // Rung nhẹ
robot.wristZ = 90.0f + shake;  // Cốc rung khi pha cà phê
```

## ⚠️ Troubleshooting

### 🐛 Vấn đề thường gặp

1. **Robot không chuyển động**
   - Kiểm tra `robot.moving = true` trong `startCoffeeSequence()`
   - Kiểm tra `ANIMATION_SPEED > 0`

2. **Robot chuyển động quá nhanh/chậm**
   - Điều chỉnh `ANIMATION_SPEED` trong `robot.h`
   - Thay đổi `robot.progress +=` trong `updateRobot()`

3. **Robot không với tới cốc**
   - Điều chỉnh `robot.cupX` trong `initRobot()`
   - Tăng độ dài `ARM_UPPER` và `ARM_LOWER`

4. **Cốc không theo robot**
   - Kiểm tra logic trong `drawCup()` khi `robot.holdingCup = true`
   - Đảm bảo forward kinematics đúng

5. **Màu sắc không hiển thị**
   - Kiểm tra giá trị RGB từ 0.0f đến 1.0f
   - Đảm bảo `glColor3f()` được gọi trước `draw...()`

6. **Animation không mượt**
   - Sử dụng hàm `smooth(t)` thay vì `t` trực tiếp
   - Giảm `ANIMATION_SPEED` để chuyển động chậm hơn

### 🔧 Tips debug

```cpp
// Thêm vào updateRobot() để debug
printf("Phase: %d, Progress: %.2f, Body: %.1f\n", 
       robot.phase, robot.progress, robot.body);

// Debug vị trí cốc
printf("Cup: (%.1f, %.1f, %.1f), Holding: %s\n", 
       robot.cupX, robot.cupY, robot.cupZ, 
       robot.holdingCup ? "Yes" : "No");
```

---
📝 **Ghi chú**: File này được đồng bộ với robot_simple.cpp version hiện tại. Kiểm tra code mới nhất khi có thay đổi. 