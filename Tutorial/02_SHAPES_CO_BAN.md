# BƯỚC 2: VẼ CÁC SHAPES CƠ BẢN

## 🎯 MỤC TIÊU BƯỚC NÀY
Tạo các hàm vẽ shapes cơ bản làm "building blocks" cho robot:
- Hình hộp (box) với kích thước và màu tùy chỉnh
- Hình trụ (cylinder) với bán kính, chiều cao và màu
- Test các shapes này trước khi dùng để vẽ robot

## 📝 CODE UPDATES

### 🔄 Cập nhật `main.cpp` (thêm vào bước 1)

Thêm các hàm này **trước** hàm `display()`:

```cpp
//=============================================================================
// HÀM VẼ SHAPES CƠ BẢN - Building blocks cho robot
//=============================================================================

void drawBox(float width, float height, float depth, float r, float g, float b) {
    // Vẽ hình hộp với kích thước và màu cho trước
    glColor3f(r, g, b);  // Đặt màu
    glPushMatrix();      // Lưu trạng thái transformation hiện tại
    glScalef(width, height, depth);  // Scale cube để có kích thước mong muốn
    glutSolidCube(1.0f);             // Vẽ cube với kích thước 1x1x1
    glPopMatrix();       // Khôi phục trạng thái transformation
}

void drawCylinder(float radius, float height, float r, float g, float b) {
    // Vẽ hình trụ với bán kính, chiều cao và màu cho trước
    glColor3f(r, g, b);
    GLUquadric* quad = gluNewQuadric();  // Tạo quadric object để vẽ cylinder
    glRotatef(-90, 1, 0, 0);             // Xoay để cylinder đứng dọc
    gluCylinder(quad, radius, radius, height, 20, 1);  // Vẽ cylinder
    gluDeleteQuadric(quad);              // Xóa quadric object
}

void drawSphere(float radius, float r, float g, float b) {
    // Vẽ hình cầu với bán kính và màu cho trước  
    glColor3f(r, g, b);
    glutSolidSphere(radius, 15, 15);  // 15x15 là độ chi tiết của sphere
}
```

### 🔄 Cập nhật hàm `display()` để test shapes

Thay thế hàm `display()` cũ bằng cái này:

```cpp
void display() {
    // Xóa buffer màu và độ sâu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Đặt camera
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    
    // TEST CÁC SHAPES - sắp xếp thành hàng ngang
    
    // Box 1 - màu đỏ
    glPushMatrix();
    glTranslatef(-4.0f, 0.0f, 0.0f);
    drawBox(1.5f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f);  // Đỏ
    glPopMatrix();
    
    // Cylinder - màu xanh lá
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawCylinder(0.5f, 2.0f, 0.0f, 1.0f, 0.0f);  // Xanh lá
    glPopMatrix();
    
    // Sphere - màu xanh dương
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, 0.0f);
    drawSphere(0.8f, 0.0f, 0.0f, 1.0f);  // Xanh dương
    glPopMatrix();
    
    // Box 2 - màu vàng
    glPushMatrix();
    glTranslatef(5.0f, 0.0f, 0.0f);
    drawBox(1.0f, 1.0f, 2.5f, 1.0f, 1.0f, 0.0f);  // Vàng
    glPopMatrix();
    
    // Hiển thị (double buffering)
    glutSwapBuffers();
}
```

### 🔄 Cập nhật instructions trong main()

```cpp
std::cout << "=== COFFEE ROBOT TUTORIAL - STEP 2 ===" << std::endl;
std::cout << "ESC - Thoát chương trình" << std::endl;
std::cout << "Bạn sẽ thấy: Box đỏ, Cylinder xanh, Sphere xanh dương, Box vàng" << std::endl;
```

## 🔧 COMPILE VÀ TEST

```bash
# Compile
g++ -Wall -std=c++11 -o coffee_robot_step2 main.cpp -lGL -lGLU -lglut

# Chạy
./coffee_robot_step2
```

## ✅ KẾT QUẢ MONG ĐỢI

Bạn sẽ thấy 4 shapes xếp thành hàng ngang:
1. **Box đỏ** (hình chữ nhật cao)
2. **Cylinder xanh lá** (hình trụ)  
3. **Sphere xanh dương** (hình cầu)
4. **Box vàng** (hình hộp dài)

## 🔍 GIẢI THÍCH CHI TIẾT

### 🎨 Hàm `drawBox()`:
```cpp
void drawBox(float width, float height, float depth, float r, float g, float b)
```
- **Parameters**: kích thước (W×H×D) và màu RGB (0.0-1.0)
- **Cách hoạt động**: Scale một cube 1×1×1 thành kích thước mong muốn
- **`glPushMatrix()/glPopMatrix()`**: Lưu/khôi phục transformation để không ảnh hưởng objects khác

### 🏗️ Hàm `drawCylinder()`:
```cpp
void drawCylinder(float radius, float height, float r, float g, float b)
```
- **GLUquadric**: OpenGL utility object để vẽ shapes phức tạp
- **`glRotatef(-90, 1, 0, 0)`**: Xoay 90 độ quanh trục X để cylinder đứng dọc
- **`gluCylinder()`**: Vẽ cylinder với 20 segments (độ mượt)

### ⚪ Hàm `drawSphere()`:
```cpp
void drawSphere(float radius, float r, float g, float b)
```
- **`glutSolidSphere(radius, slices, stacks)`**: 15×15 = đủ mượt cho robot
- Đơn giản nhất trong 3 hàm

### 🔄 Matrix Transformations:
```cpp
glPushMatrix();     // Lưu transformation state
glTranslatef(...);  // Di chuyển đến vị trí mong muốn
drawSomething();    // Vẽ object tại vị trí đó
glPopMatrix();      // Khôi phục state ban đầu
```

## 💡 TẠI SAO CẦN CÁC HÀM NÀY?

### 🤖 Cho robot:
- **`drawBox()`** → Thân robot, cánh tay, gripper fingers
- **`drawCylinder()`** → Khớp nối, cột trung tâm  
- **`drawSphere()`** → Các khớp tròn (vai, khuỷu, cổ tay)

### 🏗️ Cho environment:
- **`drawBox()`** → Bàn, máy pha cà phê
- **`drawCylinder()`** → Cốc cà phê, vòi nước

### ✨ Ưu điểm:
- **Reusable** - dùng lại nhiều lần với tham số khác nhau
- **Clean code** - main drawing logic sẽ rất dễ đọc
- **Flexible** - dễ thay đổi màu sắc, kích thước

## 🚀 EXPERIMENT SUGGESTIONS

Thử thay đổi một số values để hiểu cách chúng hoạt động:

```cpp
// Thử thay đổi camera distance
float cameraDistance = 15.0f;  // Xa hơn để thấy toàn bộ

// Thử colors khác nhau
drawBox(2.0f, 1.0f, 1.0f, 0.8f, 0.2f, 0.9f);  // Màu tím

// Thử kích thước khác
drawCylinder(1.0f, 3.0f, 1.0f, 0.5f, 0.0f);   // Cylinder to và cao
```

## 🚀 BƯỚC TIẾP THEO

Khi đã hiểu cách vẽ shapes cơ bản, đọc file `03_ROBOT_STRUCT.md` để tạo cấu trúc dữ liệu cho robot!

## 🐛 TROUBLESHOOTING

### ❌ Cylinder không hiển thị:
- Check xem có `#include <GL/glu.h>` không
- Thử thay `gluCylinder` bằng `glutSolidCone` để test

### ❌ Shapes bị méo:
- Check tỷ lệ window trong `reshape()` function
- Thử thay đổi `cameraDistance`

### ❌ Màu sắc không đúng:
- Đảm bảo lighting được setup trong `initGL()`
- Values RGB phải từ 0.0 đến 1.0, không phải 0-255

---
> 💡 **Mẹo**: Thử comment/uncomment từng object để hiểu transformation matrix hoạt động thế nào! 