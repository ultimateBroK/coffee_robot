# 🤖 HƯỚNG DẪN TRỰC QUAN: robot.cpp

## 🎯 Tổng quan
File `robot.cpp` chứa **logic điều khiển robot** - bao gồm animation, chuyển động qua từng giai đoạn và các hàm vẽ 3D.

---

## 📊 Sơ đồ kiến trúc tổng thể

```
robot.cpp
├── 🌍 BIẾN TOÀN CỤC
│   └── Robot robot → Instance duy nhất
│
├── ⚡ HÀM TIỆN ÍCH
│   ├── smooth() → Làm mượt animation
│   └── advancePhase() → Chuyển pha animation
│
├── 🔧 KHỞI TẠO & ĐIỀU KHIỂN
│   ├── initRobot() → Reset về vị trí ban đầu
│   ├── startCoffeeSequence() → Bắt đầu pha cà phê
│   ├── resetRobot() → Reset robot
│   └── updateRobot() → Logic animation chính
│
├── 🎨 HÀM VẼ CƠ BẢN
│   ├── drawSimpleBox() → Vẽ hình hộp
│   └── drawSimpleCylinder() → Vẽ hình trụ
│
├── 🤖 VẼ THÀNH PHẦN ROBOT
│   ├── drawRobotBase() → Vẽ đế robot
│   ├── drawRobotArm() → Vẽ cánh tay robot
│   └── drawRobot() → Kết hợp toàn bộ robot
│
└── 🌎 VẼ MÔI TRƯỜNG
    ├── drawTable() → Vẽ bàn
    ├── drawCoffeeMachine() → Vẽ máy pha cà phê
    ├── drawCup() → Vẽ cốc (tối ưu hóa)
    └── drawScene() → Kết hợp toàn bộ môi trường
```

---

## ⚡ Hàm smooth() - Làm mượt animation

### 💡 **Mục đích:**
Chuyển đổi animation tuyến tính thành chuyển động mượt mà với gia tốc/giảm tốc tự nhiên.

### 📊 **Đồ thị so sánh:**
```
Animation tuyến tính:     Animation mượt (smooth):
  
Tốc độ                    Tốc độ
  ↑                        ↑    
  |████████████            |    ╭─────╮
  |████████████            |   ╱       ╲
  |████████████            |  ╱         ╲
  |████████████            | ╱           ╲
  └─────────────→          └╱─────────────╲──→
  Thời gian                 Thời gian

- Bắt đầu đột ngột         - Bắt đầu từ từ
- Tốc độ đều               - Tăng tốc dần
- Kết thúc đột ngột        - Chậm lại ở cuối
```

### 🔴 **Code và giải thích:**
```cpp
float smooth(float t) {
    return t * t * (3.0f - 2.0f * t);
}
```

| Input (t) | Output | Mô tả |
|-----------|--------|-------|
| 0.0 | 0.0 | Bắt đầu - tốc độ = 0 |
| 0.25 | 0.156 | Tăng tốc chậm |
| 0.5 | 0.5 | Tốc độ tối đa |
| 0.75 | 0.844 | Giảm tốc |
| 1.0 | 1.0 | Kết thúc - tốc độ = 0 |

---

## 🔧 Các hàm điều khiển robot

### 📊 **Luồng hoạt động:**
```
startCoffeeSequence()
        ↓
    robot.moving = true
    robot.phase = REACH_CUP
        ↓
updateRobot() [được gọi 60 lần/giây]
        ↓
    Kiểm tra robot.moving?
        ↓ (true)
    Tăng robot.progress
        ↓
    Switch robot.phase:
    ├── REACH_CUP → Di chuyển tới cốc
    ├── GRAB_CUP → Nắm cốc
    ├── TURN_TO_MACHINE → Quay về máy
    ├── POUR_COFFEE → Pha cà phê
    ├── RETURN_CUP → Quay về bàn
    ├── PLACE_CUP → Đặt cốc
    ├── GO_HOME → Về vị trí ban đầu
    └── FINISHED → Dừng animation
        ↓
    Hoàn thành phase? → advancePhase() → Phase tiếp theo
```

---

## 🎭 Chi tiết từng giai đoạn animation

### 🔴 **GIAI ĐOẠN 1: REACH_CUP**
```
Mục tiêu: Di chuyển cánh tay robot tới vị trí cốc

Animation:
shoulderZ: 0° → -25°     (Hạ vai xuống)
elbow:     0° → -45°     (Gập khuỷu tay)  
wristZ:    0° → +45°     (Nâng cổ tay lên)
shoulderY: 0° → +12°     (Xoay vai về phía cốc)
fingers:   90° → 85°     (Chuẩn bị gripper)

Hình ảnh minh họa:
     Vị trí ban đầu         →        Vị trí cuối
         🤖                           🤖
         ║                            ║ \
         ║                            ║  ╲
         ▼                            ▼   ╲
       [Thẳng]                    [Duỗi về cốc]
                                        ☕
```

### 🔴 **GIAI ĐOẠN 2: GRAB_CUP**
```
Mục tiêu: Xoay gripper và đóng để nắm chặt cốc

Animation:
wristY: 0° → -90°        (Xoay gripper thẳng hàng với cốc)
fingers: 85° → 62°       (Đóng gripper để nắm cốc)
shoulderZ: -25° → -15°   (Nhấc cốc lên khỏi bàn)
elbow: -45° → -40°       (Điều chỉnh khuỷu tay)

Logic đặc biệt:
- 40% đầu: Chuẩn bị gripper (85° → 81°)
- 60% sau: Đóng gripper (81° → 62°)
- 50% cuối: Bắt đầu nhấc cốc (holdingCup = true)
```

### 🟡 **GIAI ĐOẠN 3: TURN_TO_MACHINE**
```
Mục tiêu: Xoay robot 180° về phía máy pha cà phê

Animation:
body: 0° → -180°                    (Xoay thân robot)
shoulderY: 12° → 30°                (Điều chỉnh vai ngang)
shoulderZ: -15° → -5°               (Nâng vai lên)
elbow: -40° → -15°                  (Duỗi khuỷu tay)
wristZ: 45° → 55°                   (Nâng cổ tay lên cao)
wristY: -90° → -85°                 (Điều chỉnh nhẹ gripper)

Hình ảnh:
Ban đầu:               Cuối:
  🤖 → [máy pha]        [máy pha] ← 🤖
       ☕                   ☕
```

### 🟡 **GIAI ĐOẠN 4: POUR_COFFEE**
```
Mục tiêu: Giữ cốc ổn định dưới vòi pha cà phê

Animation:
wristZ:  55°               (Điều chỉnh cốc thẳng đứng)
wristY: -85° → -90°        (Tinh chỉnh gripper)
Thời gian: 1.5 lần thường (để pha cà phê)

Kết quả: cupHasCoffee = true
```

### 🟡 **GIAI ĐOẠN 5: RETURN_CUP**
```
Mục tiêu: Đảo ngược chuyển động để quay về bàn

Animation (đảo ngược TURN_TO_MACHINE):
reverse = 1.0 - s
body: reverse * -180°               (Xoay thân về 0°)
shoulderY: 12° + reverse * 18°      (Đưa vai về vị trí)
shoulderZ: -15° + reverse * 10°     (Hạ vai xuống)
elbow: -40° + reverse * 25°         (Gập khuỷu tay)
wristZ: 45° + reverse * 10°         (Hạ cổ tay)
wristY: -90° + reverse * 5°         (Điều chỉnh gripper)
```

### 🟢 **GIAI ĐOẠN 6: PLACE_CUP**
```
Mục tiêu: Hạ cốc xuống bàn và thả ra

Animation:
shoulderZ: -25° → -35°              (Hạ vai để đặt cốc)
elbow: -40° → -45°                  (Điều chỉnh khuỷu tay)
wristZ: 45° → 30°                   (Hạ cổ tay)
wristY: -90° → 0°                   (Xoay gripper về vị trí ban đầu)

Logic thả cốc:
- 50% đầu: Chỉ hạ cốc
- 50% cuối: holdingCup = false, fingers: 30° → 90° (mở mượt mà)
```

### 🟢 **GIAI ĐOẠN 7: GO_HOME**
```
Mục tiêu: Đưa tất cả khớp về vị trí trung tính (0°)

Animation (đảo ngược tất cả):
reverse2 = 1.0 - s
easeReverse = smooth(reverse2);      (Làm mượt đường về)
shoulderY: easeReverse * 12°        (Về 0°)
shoulderZ: easeReverse * -35°       (Về 0°)
elbow: easeReverse * -45°           (Về 0°)
wristZ: easeReverse * 30°           (Về 0°)
wristY: easeReverse * -5°           (Về 0°)
fingers: 90° - easeReverse * 5°     (Về 90°)

Kết quả: robot.moving = false, phase = FINISHED
```

---

## 🎨 Hệ thống vẽ 3D

### 📊 **Hierarchy vẽ robot:**
```
drawRobot()
└── glTranslatef(0, 1.1f, 0)          [Nâng robot lên bàn]
    ├── drawRobotBase()
    │   ├── glTranslatef(0, -1, 0)     [Đế robot]
    │   │   └── drawSimpleBox(1.8, 0.2, 1.8) [Đế]
    │   └── drawSimpleCylinder(0.5, 1.0) [Cột trung tâm]
    │
    └── glRotatef(robot.body, 0,1,0)   [Xoay thân]
        ├── drawSimpleBox(1.2, 1.4, 1.2) [Thân robot]
        └── drawRobotArm()
            ├── glTranslatef(0, 1, 0)           [Di chuyển tới vai]
            ├── glRotatef(robot.shoulderY, 0,1,0) [Xoay vai ngang]
            ├── glRotatef(robot.shoulderZ, 0,0,1) [Xoay vai dọc]
            ├── glutSolidSphere(0.4)            [Khớp vai]
            ├── drawSimpleBox(ARM_UPPER, ...)   [Cánh tay trên]
            ├── glRotatef(robot.elbow, 0,0,1)   [Gập khuỷu]
            ├── glutSolidSphere(0.3)            [Khớp khuỷu]
            ├── drawSimpleBox(ARM_LOWER, ...)   [Cánh tay dưới]
            ├── glRotatef(robot.wristZ, 0,0,1)  [Gập cổ tay]
            ├── glRotatef(robot.wristY, 0,1,0)  [Xoay cổ tay]
            ├── glutSolidSphere(0.2)            [Khớp cổ tay]
            └── [Gripper]
                ├── drawSimpleBox(0.3, 0.4, 0.3) [Thân gripper]
                ├── [Ngón trên] glRotatef(-robot.fingers, 0,0,1)
                └── [Ngón dưới] glRotatef(robot.fingers, 0,0,1)
```

### 🔴 **Tọa độ và transformation:**

| Thành phần | Transformation | Mô tả |
|------------|----------------|-------|
| **Đế robot** | `translateY(-1)` | Đặt dưới mặt đất |
| **Thân robot** | `rotateY(body)` | Xoay theo biến body |
| **Vai** | `rotateY(shoulderY) + rotateZ(shoulderZ)` | Xoay 2 trục |
| **Khuỷu tay** | `rotateZ(elbow)` | Chỉ gập lên/xuống |
| **Cổ tay** | `rotateZ(wristZ) + rotateY(wristY)` | Xoay 2 trục |
| **Gripper** | `rotateZ(±fingers)` | Ngón trên/dưới ngược chiều |

---

## 🌎 Hệ thống vẽ môi trường

### 📊 **Layout scene:**
```
        [Máy pha cà phê]
              ⇅ 
    ╔═══════════════════════╗
    ║                       ║
    ║       ☕              ║ ← Bàn (TABLE_Y = 0.1f)
    ║        ↖️              ║
    ║          🤖           ║ ← Robot (Y = 1.1f)
    ║                       ║
    ╚═══════════════════════╝
         ↑
    Kích thước: 10×8×0.2
```

### 🔴 **Vị trí các đối tượng:**

| Đối tượng | Vị trí (X, Y, Z) | Kích thước | Màu sắc |
|-----------|------------------|------------|---------|
| **Bàn** | (0, 0, 0) | 10×0.2×8 | Nâu đậm (0.25, 0.15, 0.10) |
| **Robot** | (0, 1.1, 0) | Tùy theo các khớp | Xám xanh |
| **Máy pha cà phê** | (-4, TABLE_Y+1.1, 0) | 1.5×2.2×1.4 | Xám kim loại |
| **Cốc** | (2.2, TABLE_Y+0.25, 0) | Bán kính 0.25 | Trắng kem |

---

## ⚙️ Tối ưu hóa và cải tiến

### 🔄 **Hàm advancePhase()**
```cpp
// Hàm tiện ích để chuyển pha animation, giảm mã lặp
void advancePhase(RobotPhase newPhase, const char* message) {
    robot.phase = newPhase;
    robot.progress = 0.0f;
    printf("%s\n", message);
}
```
- **Tối ưu**: Giảm lặp lại code khi chuyển phase
- **Dễ bảo trì**: Thay đổi logic chuyển phase tại một nơi duy nhất

### 🔄 **Tối ưu vẽ cốc**
```cpp
// Tận dụng lại các phép biến đổi từ drawRobotArm để tối ưu hóa
// Di chuyển đến vị trí gripper (dùng ít phép biến đổi hơn)
```
- **Hiệu suất**: Giảm số lượng phép biến đổi ma trận
- **Dễ đọc**: Tổ chức code thành các khối logic rõ ràng

### 🔄 **Làm mượt chuyển động**
```cpp
// Trong GO_HOME:
float easeReverse = smooth(reverse2); // Áp dụng hàm smooth cho chuyển động về
```
- **Chuyển động tự nhiên hơn**: Áp dụng hàm smooth nhiều lần
- **Tinh chỉnh**: Các thông số góc được điều chỉnh để chuyển động mượt mà

---

## 🐛 Các lỗi thường gặp và cách fix

### ❌ **Robot không chuyển động:**
```cpp
// Kiểm tra:
if (!robot.moving) return;  // Có thể robot.moving = false

// Fix: Gọi startCoffeeSequence() hoặc kiểm tra logic trong updateRobot()
```

### ❌ **Animation bị giật cục:**
```cpp
// Nguyên nhân: ANIMATION_SPEED quá lớn
const float ANIMATION_SPEED = 0.005f;  // Giảm từ 0.015f

// Hoặc: Không dùng smooth()
float s = smooth(t);  // Thay vì dùng t trực tiếp
```

### ❌ **Robot không với tới cốc:**
```cpp
// Kiểm tra vị trí cốc:
robot.cupX = 2.2f;  // Có thể quá xa
robot.cupY = TABLE_Y + 0.25f;  // Có thể quá cao/thấp

// Hoặc kiểm tra chiều dài cánh tay:
const float ARM_UPPER = 1.6f;
const float ARM_LOWER = 1.4f;
// Tổng với được: ~3.0f, cốc ở (2.2, ?, 0) = khả thi
```

---

## 🎯 **Tóm tắt:**
- `robot.cpp` chứa **toàn bộ logic robot** - từ animation đến vẽ 3D
- **updateRobot()** là trái tim của hệ thống animation
- **Hệ thống vẽ** sử dụng OpenGL transformation hierarchy
- **Dễ dàng tùy chỉnh** bằng cách thay đổi các tham số trong switch cases
- **Tối ưu hóa** với hàm advancePhase() và cải tiến vẽ cốc 