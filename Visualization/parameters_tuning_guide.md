# ⚙️ HƯỚNG DẪN TINH CHỈNH THAM SỐ

## 🎯 Mục đích
Hướng dẫn này giúp bạn hiểu cách thay đổi các tham số trong chương trình robot và quan sát hiệu ứng trực quan.

---

## 🔧 CÁC THAM SỐ CHÍNH

### 🔴 **1. Tốc độ Animation (robot.h)**
```cpp
const float ANIMATION_SPEED = 0.015f;
```

| Giá trị | Hiệu ứng | Thích hợp cho |
|---------|----------|---------------|
| 0.005f | 🐌 Rất chậm | Demo chi tiết, debug |
| 0.010f | 🚶 Chậm | Presentation |
| 0.015f | 🏃 Bình thường | **Mặc định** |
| 0.030f | 🏃‍♂️ Nhanh | Demo nhanh |
| 0.050f | 🚀 Rất nhanh | Test nhanh |

**🎮 Cách test:**
1. Thay đổi giá trị trong `robot.h`
2. Compile: `make`
3. Chạy và nhấn SPACE để quan sát

---

### 🔴 **2. Kích thước Robot (robot.h)**
```cpp
const float ARM_UPPER = 1.6f;    // Cánh tay trên
const float ARM_LOWER = 1.4f;    // Cánh tay dưới
```

#### **🎮 Thử nghiệm - Robot khổng lồ:**
```cpp
const float ARM_UPPER = 2.5f;    // +56% larger
const float ARM_LOWER = 2.2f;    // +57% larger
```
**Hiệu ứng:** Robot với xa hơn, có thể nắm cốc dễ dàng

#### **🎮 Thử nghiệm - Robot tí hon:**
```cpp
const float ARM_UPPER = 1.0f;    // -37% smaller  
const float ARM_LOWER = 0.8f;    // -43% smaller
```
**Hiệu ứng:** Robot nhỏ hơn, có thể không với tới cốc

---

### 🟡 **3. Vị trí Objects**

#### **Vị trí cốc (robot.cpp, hàm initRobot()):**
```cpp
robot.cupX = 2.2f;               // Khoảng cách từ robot
robot.cupY = TABLE_Y + 0.25f;    // Độ cao trên bàn  
robot.cupZ = 0.0f;               // Độ sâu
```

| Tham số | Thay đổi | Hiệu ứng |
|---------|----------|----------|
| `cupX = 1.5f` | Gần robot hơn | ✅ Dễ nắm |
| `cupX = 3.0f` | Xa robot hơn | ❌ Khó nắm |
| `cupY += 0.3f` | Cao hơn | Robot phải nâng tay cao |
| `cupZ = 1.0f` | Lệch sang bên | Robot phải xoay nhiều |

---

## 🎨 TINH CHỈNH VISUAL

### 🔴 **1. Màu sắc Robot**

#### **Đế robot (robot.cpp, hàm drawRobotBase()):**
```cpp
// Màu mặc định:
drawSimpleBox(1.8f, 0.2f, 1.8f, 0.3f, 0.3f, 0.4f);

// 🎮 Thử nghiệm màu sắc:
drawSimpleBox(1.8f, 0.2f, 1.8f, 1.0f, 0.0f, 0.0f); // Đỏ
drawSimpleBox(1.8f, 0.2f, 1.8f, 0.0f, 1.0f, 0.0f); // Xanh lá
drawSimpleBox(1.8f, 0.2f, 1.8f, 0.0f, 0.0f, 1.0f); // Xanh dương
drawSimpleBox(1.8f, 0.2f, 1.8f, 1.0f, 1.0f, 0.0f); // Vàng
```

#### **Cánh tay robot (robot.cpp, hàm drawRobotArm()):**
```cpp
// Tìm dòng:
drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 0.6f, 0.6f, 0.7f);

// 🎮 Thay thành:
drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 0.8f, 0.2f, 0.2f); // Đỏ cam
```

### 🟡 **2. Kích thước Gripper**
```cpp
// Trong drawRobotArm(), tìm phần gripper:

// Ngón tay trên
drawSimpleBox(0.3f, 0.08f, 0.2f, 0.5f, 0.5f, 0.6f);

// 🎮 Làm ngón tay to hơn:
drawSimpleBox(0.5f, 0.12f, 0.3f, 0.5f, 0.5f, 0.6f);
```

---

## 🎭 TINH CHỈNH ANIMATION

### 🔴 **1. Thay đổi góc chuyển động**

#### **Làm robot hạ vai sâu hơn (REACH_CUP):**
```cpp
// Trong updateRobot(), case REACH_CUP:
robot.shoulderZ = s * -45.0f;  // Từ -30° → -45°
robot.elbow = s * -70.0f;      // Từ -50° → -70°
```

#### **Làm robot xoay thân nhiều hơn (TURN_TO_MACHINE):**
```cpp
// case TURN_TO_MACHINE:
robot.body = s * -270.0f;      // Từ -180° → -270° (xoay 3/4 vòng)
```

### 🟡 **2. Thay đổi thời gian từng giai đoạn**

#### **Pha cà phê lâu hơn:**
```cpp
case POUR_COFFEE:
    if (robot.progress >= 3.0f) {  // Từ 1.5f → 3.0f (lâu gấp đôi)
        robot.cupHasCoffee = true;
        robot.phase = RETURN_CUP;
        robot.progress = 0.0f;
    }
    break;
```

#### **Nắm cốc nhanh hơn:**
```cpp
case GRAB_CUP:
    // Thay đổi logic thời gian:
    if (s < 0.3f) {        // Từ 0.5f → 0.3f
        robot.fingers = 90.0f;
    } else {
        float gripProgress = (s - 0.3f) / 0.7f; // Từ 0.5f → 0.7f
        robot.fingers = 90.0f - gripProgress * 60.0f;
    }
```

---

## 🌍 TINH CHỈNH MÔI TRƯỜNG

### 🔴 **1. Kích thước bàn**
```cpp
// Trong drawTable():
drawSimpleBox(10.0f, 0.2f, 8.0f, 0.25f, 0.15f, 0.10f);

// 🎮 Bàn lớn hơn:
drawSimpleBox(15.0f, 0.2f, 12.0f, 0.25f, 0.15f, 0.10f);

// 🎮 Bàn cao hơn:
drawSimpleBox(10.0f, 0.5f, 8.0f, 0.25f, 0.15f, 0.10f);
```

### 🟡 **2. Vị trí máy pha cà phê**
```cpp
// Trong drawCoffeeMachine():
glTranslatef(-4, TABLE_Y + 1.1f, 0);

// 🎮 Gần robot hơn:
glTranslatef(-2, TABLE_Y + 1.1f, 0);

// 🎮 Cao hơn:
glTranslatef(-4, TABLE_Y + 1.5f, 0);
```

### 🟢 **3. Ánh sáng**
```cpp
// Trong initGL():
GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};

// 🎮 Ánh sáng từ bên trái:
GLfloat lightPos[] = {-5.0f, 10.0f, 5.0f, 1.0f};

// 🎮 Ánh sáng yếu hơn:
GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Từ 0.3f
```

---

## 🎮 CAMERA & UI

### 🔴 **1. Vị trí camera mặc định**
```cpp
// Trong coffee_robot.cpp:
float cameraDistance = 15.0f;

// 🎮 Gần hơn để xem chi tiết:
float cameraDistance = 8.0f;

// 🎮 Xa hơn để xem tổng thể:
float cameraDistance = 25.0f;
```

### 🟡 **2. Tốc độ FPS**
```cpp
// Trong timer():
glutTimerFunc(16, timer, 0);  // 16ms = ~60 FPS

// 🎮 Mượt hơn:
glutTimerFunc(8, timer, 0);   // 8ms = ~120 FPS

// 🎮 Tiết kiệm pin:
glutTimerFunc(33, timer, 0);  // 33ms = ~30 FPS
```

---

## 🧪 THỰC HÀNH STEP-BY-STEP

### **Bài tập 1: Robot to hơn và nhanh hơn**
1. Mở `robot.h`
2. Thay đổi:
   ```cpp
   const float ANIMATION_SPEED = 0.025f;  // Nhanh hơn
   const float ARM_UPPER = 2.0f;          // Cánh tay dài hơn
   const float ARM_LOWER = 1.8f;          // Cánh tay dài hơn
   ```
3. Compile và test: `make && ./coffee_robot`

### **Bài tập 2: Robot màu đỏ**
1. Mở `robot.cpp`
2. Trong `drawRobotBase()`, thay:
   ```cpp
   drawSimpleBox(1.8f, 0.2f, 1.8f, 1.0f, 0.2f, 0.2f);
   ```
3. Trong `drawRobotArm()`, thay tất cả màu thành đỏ:
   ```cpp
   drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 0.8f, 0.2f, 0.2f);
   ```

### **Bài tập 3: Cốc gần robot hơn**
1. Mở `robot.cpp`
2. Trong `initRobot()`, thay:
   ```cpp
   robot.cupX = 1.8f;  // Từ 2.2f → 1.8f
   ```
3. Test xem robot có nắm được cốc dễ hơn không

---

## 🐛 TROUBLESHOOTING

### **❌ Robot không với tới cốc:**
- Kiểm tra `robot.cupX` có quá xa không
- Tăng `ARM_UPPER` và `ARM_LOWER`
- Kiểm tra góc `shoulderZ` trong `REACH_CUP`

### **❌ Animation quá nhanh/chậm:**
- Điều chỉnh `ANIMATION_SPEED` từ từ (±0.005f mỗi lần)
- Thay đổi interval trong `timer()` function

### **❌ Màu sắc không đúng:**
- Đảm bảo các giá trị RGB trong khoảng 0.0f-1.0f
- Kiểm tra lighting có bật không (`glEnable(GL_LIGHTING)`)

### **❌ Robot biến mất:**
- Kiểm tra kích thước có quá lớn không
- Điều chỉnh `cameraDistance` để nhìn rõ hơn

---

## 🎯 **Tóm tắt:**
- **Tham số dễ thay đổi nhất:** `ANIMATION_SPEED`, màu sắc, vị trí cốc
- **Hiệu ứng thú vị nhất:** Thay đổi kích thước robot và góc chuyển động
- **Test ngay lập tức:** Mỗi thay đổi chỉ cần `make && ./coffee_robot` 