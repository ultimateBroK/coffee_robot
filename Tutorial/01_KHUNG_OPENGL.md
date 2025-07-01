# BƯỚC 1: KHUNG CHƯƠNG TRÌNH OPENGL CỞ BẢN

## 🎯 MỤC TIÊU BƯỚC NÀY
Tạo một cửa sổ OpenGL đơn giản có thể:
- Mở cửa sổ 3D 
- Có background màu xanh da trời
- Vẽ một hình lập phương đơn giản để test
- Thoát được bằng phím ESC

## 📁 TẠO FILES

Tạo folder mới và các files:
```bash
mkdir coffee_robot_tutorial
cd coffee_robot_tutorial
```

### 📄 File 1: `main.cpp` (khung cơ bản)

```cpp
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>

//=============================================================================
// BIẾN TOÀN CỤC ĐƠN GIẢN
//=============================================================================
float cameraDistance = 10.0f;  // Khoảng cách camera

//=============================================================================
// KHỞI TẠO OPENGL
//=============================================================================
void initGL() {
    // Bật kiểm tra độ sâu để vật gần che vật xa
    glEnable(GL_DEPTH_TEST);
    
    // Bật hệ thống ánh sáng cơ bản
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Thiết lập ánh sáng đơn giản
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    
    // Màu nền xanh da trời nhạt
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
}

//=============================================================================
// HÀM VẼ CHÍNH
//=============================================================================
void display() {
    // Xóa buffer màu và độ sâu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Đặt camera
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    
    // Vẽ một hình lập phương test
    glColor3f(1.0f, 0.0f, 0.0f);  // Màu đỏ
    glutSolidCube(2.0f);
    
    // Hiển thị (double buffering)
    glutSwapBuffers();
}

//=============================================================================
// XỬ LÝ BÀN PHÍM
//=============================================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Phím ESC
            std::cout << "Thoát chương trình..." << std::endl;
            exit(0);
            break;
    }
    glutPostRedisplay(); // Yêu cầu vẽ lại
}

//=============================================================================
// XỬ LÝ THAY ĐỔI KÍCH THƯỚC CỬA SỔ
//=============================================================================
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

//=============================================================================
// HÀM MAIN
//=============================================================================
int main(int argc, char** argv) {
    // Khởi tạo GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    
    // Tạo cửa sổ
    int window = glutCreateWindow("Coffee Robot Tutorial - Step 1");
    if (window == 0) {
        std::cerr << "Không thể tạo cửa sổ OpenGL!" << std::endl;
        return -1;
    }
    
    std::cout << "=== COFFEE ROBOT TUTORIAL - STEP 1 ===" << std::endl;
    std::cout << "ESC - Thoát chương trình" << std::endl;
    std::cout << "Nếu thấy một hình lập phương đỏ, bạn đã thành công!" << std::endl;
    
    // Khởi tạo OpenGL
    initGL();
    
    // Đăng ký callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    // Bắt đầu vòng lặp chính
    glutMainLoop();
    
    return 0;
}
```

### 📄 File 2: `Makefile` (để compile dễ dàng)

```makefile
CC = g++
CFLAGS = -Wall -std=c++11
LIBS = -lGL -lGLU -lglut

TARGET = coffee_robot_step1
SOURCES = main.cpp

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
```

## 🔧 COMPILE VÀ CHẠY

```bash
# Compile
make

# Hoặc compile thủ công:
g++ -Wall -std=c++11 -o coffee_robot_step1 main.cpp -lGL -lGLU -lglut

# Chạy
./coffee_robot_step1
```

## ✅ KIỂM TRA KẾT QUẢ

Nếu mọi thứ OK, bạn sẽ thấy:
- Cửa sổ màu xanh da trời nhạt
- Một hình lập phương đỏ ở giữa màn hình
- Có thể thoát bằng phím ESC

## 🔍 GIẢI THÍCH CODE CHI TIẾT

### 🏗️ Cấu trúc chương trình:
1. **Include headers** - GL/gl.h, GL/glu.h, GL/glut.h cho OpenGL
2. **initGL()** - Setup ánh sáng, depth test, màu nền
3. **display()** - Hàm vẽ chính, được gọi mỗi frame
4. **keyboard()** - Xử lý input từ bàn phím
5. **reshape()** - Xử lý khi thay đổi kích thước cửa sổ
6. **main()** - Khởi tạo GLUT, tạo cửa sổ, bắt đầu main loop

### 🎨 OpenGL concepts được dùng:
- **Double buffering**: `GLUT_DOUBLE` - vẽ mượt hơn
- **Depth testing**: `GL_DEPTH_TEST` - vật gần che vật xa
- **Lighting**: `GL_LIGHTING` - ánh sáng cơ bản
- **Perspective projection**: `gluPerspective()` - tạo hiệu ứng 3D

### 💡 Tại sao làm thế này?
- **Đơn giản nhất có thể** - chỉ những gì cần thiết
- **Có thể test ngay** - thấy kết quả visual instant
- **Foundation vững chắc** - mọi bước sau đều build trên này

## 🚀 BƯỚC TIẾP THEO

Khi đã chạy được bước 1, đọc file `02_SHAPES_CO_BAN.md` để học cách vẽ nhiều hình khác nhau!

## 🐛 TROUBLESHOOTING

### ❌ Lỗi compile:
```bash
# Nếu thiếu OpenGL libraries:
sudo apt-get install freeglut3-dev  # Ubuntu
sudo yum install freeglut-devel     # Fedora
```

### ❌ Màn hình đen:
- Check GPU drivers
- Thử thay `glutSolidCube(2.0f)` thành `glutWireCube(2.0f)`

### ❌ Không thoát được:
- Đảm bảo cửa sổ đang focus
- Thử click vào cửa sổ rồi nhấn ESC

---
> 💡 **Mẹo**: Thử thay đổi giá trị `cameraDistance` hoặc màu `glColor3f()` để hiểu chúng làm gì! 