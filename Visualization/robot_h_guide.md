# 📋 HƯỚNG DẪN TRỰC QUAN: robot.h

## 🎯 Tổng quan
File `robot.h` là **header file** chứa định nghĩa các cấu trúc dữ liệu, hằng số và enum cho toàn bộ chương trình robot.

---

## 📊 Sơ đồ kiến trúc tổng thể

```
robot.h
├── 🔧 CÁC HẰNG SỐ (Constants)
│   ├── ANIMATION_SPEED → Tốc độ animation
│   ├── ARM_UPPER → Chiều dài cánh tay trên
│   ├── ARM_LOWER → Chiều dài cánh tay dưới
│   ├── CUP_SIZE → Kích thước cốc
│   └── TABLE_Y → Độ cao bàn
│
├── 🔄 ENUM ROBOTPHASE (Các giai đoạn)
│   ├── IDLE → Chờ đợi
│   ├── REACH_CUP → Đưa tay tới cốc
│   ├── GRAB_CUP → Nắm cốc
│   ├── TURN_TO_MACHINE → Quay về máy pha
│   ├── POUR_COFFEE → Pha cà phê
│   ├── RETURN_CUP → Mang cốc về
│   ├── PLACE_CUP → Đặt cốc xuống
│   ├── GO_HOME → Về vị trí ban đầu
│   └── FINISHED → Hoàn thành
│
├── 🤖 STRUCT ROBOT (Trạng thái robot)
│   ├── Góc khớp (Joint Angles)
│   ├── Trạng thái Animation
│   └── Thông tin cốc
│
└── 🔗 CÁC HÀM EXTERN (Function Declarations)
    ├── Điều khiển robot
    └── Vẽ và hiển thị
```

---

## 🔧 Chi tiết các HẰNG SỐ

### 🔴 **Quan trọng nhất**

| Hằng số | Giá trị | Mô tả | Tác động khi thay đổi |
|---------|---------|-------|----------------------|
| `ANIMATION_SPEED` | 0.015f | Tốc độ animation | ⬆️ Tăng = robot nhanh hơn<br/>⬇️ Giảm = robot chậm hơn |
| `ARM_UPPER` | 1.6f | Chiều dài cánh tay trên | ⬆️ Tăng = robot với xa hơn<br/>⬇️ Giảm = robot với gần hơn |
| `ARM_LOWER` | 1.4f | Chiều dài cánh tay dưới | ⬆️ Tăng = robot linh hoạt hơn<br/>⬇️ Giảm = robot cứng hơn |

### 🟡 **Quan trọng**

| Hằng số | Giá trị | Mô tả | Tác động khi thay đổi |
|---------|---------|-------|----------------------|
| `CUP_SIZE` | 0.25f | Bán kính cốc | ⬆️ Tăng = cốc to hơn<br/>⬇️ Giảm = cốc nhỏ hơn |
| `TABLE_Y` | 0.1f | Độ cao mặt bàn | ⬆️ Tăng = bàn cao hơn<br/>⬇️ Giảm = bàn thấp hơn |

---

## 🔄 Enum RobotPhase - Các giai đoạn hoạt động

### 📊 Biểu đồ luồng giai đoạn:

```
START
  ↓
[IDLE] ←──────────────┐
  ↓ (nhấn SPACE)      │
[REACH_CUP]           │
  ↓                   │
[GRAB_CUP]            │
  ↓                   │
[TURN_TO_MACHINE]     │
  ↓                   │
[POUR_COFFEE]         │
  ↓                   │
[RETURN_CUP]          │
  ↓                   │
[PLACE_CUP]           │
  ↓                   │
[GO_HOME]             │
  ↓                   │
[FINISHED] ───────────┘
  (nhấn R để reset)
```

### 💡 **Giải thích từng giai đoạn:**

| Giai đoạn | Mô tả | Thời gian ước tính | Hoạt động chính |
|-----------|-------|-------------------|-----------------|
| **IDLE** | 🛑 Chờ đợi lệnh | Vô hạn | Không chuyển động |
| **REACH_CUP** | 🤲 Đưa tay tới cốc | ~1.5s | Di chuyển cánh tay |
| **GRAB_CUP** | ✊ Nắm lấy cốc | ~1.5s | Đóng gripper |
| **TURN_TO_MACHINE** | 🔄 Quay về máy pha | ~1.5s | Xoay thân robot |
| **POUR_COFFEE** | ☕ Pha cà phê | ~2.5s | Giữ cốc ổn định |
| **RETURN_CUP** | ↩️ Mang cốc về | ~1.5s | Xoay thân ngược lại |
| **PLACE_CUP** | 📍 Đặt cốc xuống | ~1.5s | Hạ cốc và mở gripper |
| **GO_HOME** | 🏠 Về vị trí ban đầu | ~1.5s | Reset tất cả khớp |
| **FINISHED** | ✅ Hoàn thành | Vô hạn | Chờ lệnh mới |

---

## 🤖 Struct Robot - Trạng thái robot

### 📊 Sơ đồ cấu trúc Robot:

```
Robot {
  ┌─────────────────────────────────┐
  │        JOINT ANGLES             │
  │  ┌─────────────┬─────────────┐  │
  │  │ shoulderY   │ shoulderZ   │  │ ← Xoay vai
  │  ├─────────────┼─────────────┤  │
  │  │ elbow       │             │  │ ← Gập khuỷu tay
  │  ├─────────────┼─────────────┤  │
  │  │ wristZ      │ wristY      │  │ ← Xoay cổ tay
  │  ├─────────────┼─────────────┤  │
  │  │ fingers     │ body        │  │ ← Gripper & thân
  │  └─────────────┴─────────────┘  │
  └─────────────────────────────────┘
  ┌─────────────────────────────────┐
  │       ANIMATION STATE           │
  │  ┌─────────────┬─────────────┐  │
  │  │ phase       │ progress    │  │ ← Giai đoạn hiện tại
  │  ├─────────────┼─────────────┤  │
  │  │ moving      │             │  │ ← Có đang chuyển động?
  │  └─────────────┴─────────────┘  │
  └─────────────────────────────────┘
  ┌─────────────────────────────────┐
  │         CUP STATE               │
  │  ┌─────────────┬─────────────┐  │
  │  │ cupX,Y,Z    │ holdingCup  │  │ ← Vị trí & trạng thái cốc
  │  ├─────────────┼─────────────┤  │
  │  │ cupHasCoffee│             │  │ ← Cốc có cà phê chưa?
  │  └─────────────┴─────────────┘  │
  └─────────────────────────────────┘
}
```

### 🔴 **Góc khớp (Joint Angles) - Quan trọng nhất:**

| Biến | Đơn vị | Phạm vi | Mô tả |
|------|--------|---------|-------|
| `shoulderY` | Độ | -180° ~ +180° | 🔄 Xoay vai trái/phải |
| `shoulderZ` | Độ | -90° ~ +90° | ⬆️⬇️ Nâng/hạ vai |
| `elbow` | Độ | -150° ~ 0° | 💪 Gập khuỷu tay |
| `wristZ` | Độ | -90° ~ +90° | 🤲 Gập cổ tay lên/xuống |
| `wristY` | Độ | -180° ~ +180° | 🔄 Xoay cổ tay trái/phải |
| `fingers` | Độ | 0° ~ 90° | ✋ Mở/đóng gripper |
| `body` | Độ | -180° ~ +180° | 🔄 Xoay thân robot |

### 🟡 **Trạng thái Animation:**

| Biến | Kiểu | Mô tả |
|------|------|-------|
| `phase` | RobotPhase | Giai đoạn hiện tại |
| `progress` | float (0.0-1.0) | Tiến độ trong giai đoạn |
| `moving` | bool | Robot có đang chuyển động? |

### 🟢 **Thông tin cốc:**

| Biến | Kiểu | Mô tả |
|------|------|-------|
| `cupX, cupY, cupZ` | float | Vị trí cốc trong không gian 3D |
| `holdingCup` | bool | Robot có đang cầm cốc? |
| `cupHasCoffee` | bool | Cốc có chứa cà phê? |

---

## 🔗 Các hàm chính (Function Declarations)

### 📊 Sơ đồ mối quan hệ các hàm:

```
main() [coffee_robot.cpp]
  ↓
initRobot() ←─────┐
  ↓               │
glutMainLoop()    │
  ↓               │
timer()           │
  ↓               │
updateRobot() ────┤
  ↓               │
display()         │
  ↓               │
drawRobot()       │
drawScene()       │
  ↓               │
keyboard('R') ────┘
keyboard(' ') → startCoffeeSequence()
```

### 🔴 **Hàm quan trọng nhất:**

| Hàm | Mục đích | Được gọi khi nào |
|-----|----------|------------------|
| `initRobot()` | Khởi tạo robot về vị trí ban đầu | Lúc bắt đầu chương trình |
| `updateRobot()` | Cập nhật animation robot | Mỗi frame (~60 lần/giây) |
| `startCoffeeSequence()` | Bắt đầu quy trình pha cà phê | Nhấn phím SPACE |

### 🟡 **Hàm vẽ:**

| Hàm | Mục đích | Được gọi khi nào |
|-----|----------|------------------|
| `drawRobot()` | Vẽ robot lên màn hình | Mỗi frame |
| `drawScene()` | Vẽ bàn, máy pha cà phê, cốc | Mỗi frame |

---

## ⚙️ Hướng dẫn thay đổi tham số cơ bản

### 🎮 **Làm robot chuyển động nhanh hơn:**
```cpp
const float ANIMATION_SPEED = 0.030f;  // Tăng từ 0.015f
```

### 🎮 **Làm robot có cánh tay dài hơn:**
```cpp
const float ARM_UPPER = 2.0f;   // Tăng từ 1.6f
const float ARM_LOWER = 1.8f;   // Tăng từ 1.4f
```

### 🎮 **Làm cốc to hơn:**
```cpp
const float CUP_SIZE = 0.35f;   // Tăng từ 0.25f
```

---

## 🎯 **Tóm tắt:**
- `robot.h` định nghĩa **cấu trúc dữ liệu** cho toàn bộ chương trình
- **RobotPhase** điều khiển luồng hoạt động của robot
- **Struct Robot** chứa tất cả thông tin trạng thái
- **Các hằng số** dễ dàng điều chỉnh để tùy chỉnh hành vi robot 