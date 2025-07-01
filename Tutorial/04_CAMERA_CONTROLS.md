# BƯỚC 4: CAMERA CONTROLS VÀ INTERACTION

## 🎯 MỤC TIÊU BƯỚC NÀY
Thêm khả năng điều khiển camera để có thể xem robot từ nhiều góc độ:
- Kéo chuột để xoay camera quanh robot
- Zoom in/out bằng phím mũi tên
- Smooth camera movement
- UI hiển thị trạng thái robot

## 📝 CODE UPDATES

### 🔄 Cập nhật `main.cpp` - Thêm mouse controls

Thêm variables và functions mới vào `main.cpp`:

```cpp
#include "robot.h"
#include <iostream>

//=============================================================================
// CAMERA VARIABLES
//=============================================================================
float cameraDistance = 15.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

// Mouse control variables
bool mousePressed = false;
int lastMouseX, lastMouseY;

//=============================================================================
// OPENGL SETUP - Same as before
//=============================================================================
void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    
    initRobot();
}

//=============================================================================
// DISPLAY FUNCTION - Thêm UI
//=============================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Camera positioning
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    
    // Draw scene and robot
    drawScene();
    drawRobot();
    
    // Display status UI
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Status background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex3f(-4.0f, 4.5f, 0.0f);
    glVertex3f(4.0f, 4.5f, 0.0f);
    glVertex3f(4.0f, 3.5f, 0.0f);
    glVertex3f(-4.0f, 3.5f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);
    
    // Status text
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos3f(-3.8f, 4.2f, 0.0f);
    
    const char* statusTexts[] = {
        "IDLE - Nhan SPACE de bat dau",
        "Dang duoi tay toi coc...",
        "Dang cam coc...",
        "Dang xoay ve may pha ca phe...",
        "Dang rot ca phe...",
        "Dang mang coc ve...",
        "Dang dat coc xuong...",
        "Dang ve vi tri ban dau...",
        "HOAN THANH - Nhan R de reset"
    };
    
    const char* currentText = statusTexts[robot.phase];
    for (int i = 0; currentText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, currentText[i]);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glutSwapBuffers();
}

//=============================================================================
// KEYBOARD CONTROLS
//=============================================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':  // Space - start animation
            startCoffeeSequence();
            break;
        case 'r':
        case 'R':  // Reset
            resetRobot();
            break;
        case 27:   // ESC
            std::cout << "Thoát chương trình..." << std::endl;
            exit(0);
            break;
    }
    glutPostRedisplay();
}

//=============================================================================
// SPECIAL KEYS - Arrow keys for zoom
//=============================================================================
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    // Zoom in
            cameraDistance -= 1.0f;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;
            break;
        case GLUT_KEY_DOWN:  // Zoom out
            cameraDistance += 1.0f;
            if (cameraDistance > 30.0f) cameraDistance = 30.0f;
            break;
    }
    glutPostRedisplay();
}

//=============================================================================
// MOUSE CONTROLS
//=============================================================================
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            mousePressed = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        float deltaX = x - lastMouseX;
        float deltaY = y - lastMouseY;
        
        cameraAngleY += deltaX * 0.5f;     // Horizontal rotation
        cameraAngleX += deltaY * 0.5f;     // Vertical rotation
        
        // Limit vertical rotation
        if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;
        if (cameraAngleX < -90.0f) cameraAngleX = -90.0f;
        
        lastMouseX = x;
        lastMouseY = y;
        
        glutPostRedisplay();
    }
}

//=============================================================================
// TIMER FUNCTION
//=============================================================================
void timer(int value) {
    updateRobot();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // ~60 FPS
}

//=============================================================================
// RESHAPE FUNCTION
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
// MAIN FUNCTION
//=============================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);  // Larger window
    glutInitWindowPosition(100, 100);
    
    int window = glutCreateWindow("Coffee Robot Tutorial - Step 4");
    if (window == 0) {
        std::cerr << "Không thể tạo cửa sổ!" << std::endl;
        return -1;
    }
    
    std::cout << "=== COFFEE ROBOT TUTORIAL - STEP 4 ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  SPACE     - Bắt đầu animation pha cà phê" << std::endl;
    std::cout << "  R         - Reset robot về vị trí ban đầu" << std::endl;
    std::cout << "  ESC       - Thoát chương trình" << std::endl;
    std::cout << "  MOUSE     - Kéo chuột trái để xoay camera" << std::endl;
    std::cout << "  ARROW UP  - Zoom in" << std::endl;
    std::cout << "  ARROW DOWN- Zoom out" << std::endl;
    
    initGL();
    
    // Register all callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);      // NEW - for arrow keys
    glutMouseFunc(mouse);              // NEW - for mouse clicks
    glutMotionFunc(mouseMotion);       // NEW - for mouse movement
    glutTimerFunc(16, timer, 0);
    
    glutMainLoop();
    return 0;
}
```

## 🆕 NHỮNG GÌ ĐÃ THÊM

### 🖱️ Mouse Controls:
```cpp
void mouse(int button, int state, int x, int y)     // Bắt sự kiện click chuột
void mouseMotion(int x, int y)                      // Bắt sự kiện di chuyển chuột
```

### ⌨️ Special Keys:
```cpp
void specialKeys(int key, int x, int y)             // Xử lý phím mũi tên
```

### 🎨 UI Status Display:
- Background đen trong suốt cho text
- Hiển thị trạng thái hiện tại của robot
- Text màu vàng dễ đọc

### 📹 Camera Improvements:
- **Horizontal rotation**: Xoay quanh robot
- **Vertical rotation**: Nhìn từ trên xuống hoặc từ dưới lên  
- **Zoom controls**: Phím mũi tên để zoom in/out
- **Smooth movement**: Camera di chuyển mượt mà

## 🔧 COMPILE VÀ TEST

```bash
# Compile (same as before)
make

# Run
./coffee_robot_step4
```

## ✅ TEST CAMERA CONTROLS

### 🖱️ Mouse Controls Test:
1. **Click và kéo chuột** → Camera quay quanh robot
2. **Di chuyển nhanh** → Camera theo kịp
3. **Thả chuột** → Camera dừng lại

### ⌨️ Keyboard Test:
1. **Phím mũi tên lên** → Zoom in (robot to hơn)
2. **Phím mũi tên xuống** → Zoom out (thấy toàn bộ scene)
3. **SPACE** → Animation vẫn hoạt động bình thường

### 🎯 UI Test:
1. **Nhìn góc trên-trái** → Thấy status text màu vàng
2. **Nhấn SPACE** → Text thay đổi theo phase
3. **Background** → Đen trong suốt, không che khuất robot

## 🔍 GIẢI THÍCH CHI TIẾT

### 🖱️ Mouse Control Logic:
```cpp
// Khi nhấn chuột → lưu vị trí bắt đầu
mousePressed = true;
lastMouseX = x; lastMouseY = y;

// Khi di chuyển → tính độ chênh lệch
float deltaX = x - lastMouseX;
float deltaY = y - lastMouseY;

// Chuyển đổi thành góc xoay camera
cameraAngleY += deltaX * 0.5f;  // Factor 0.5 để smooth hơn
cameraAngleX += deltaY * 0.5f;
```

### 📹 Camera Matrix:
```cpp
glTranslatef(0, 0, -cameraDistance);    // 1. Di chuyển ra xa
glRotatef(cameraAngleX, 1, 0, 0);       // 2. Xoay lên/xuống  
glRotatef(cameraAngleY, 0, 1, 0);       // 3. Xoay trái/phải
```

### 🎨 UI Rendering:
```cpp
glDisable(GL_LIGHTING);      // Tắt ánh sáng cho UI
glDisable(GL_DEPTH_TEST);    // UI luôn ở trên cùng

// Vẽ UI elements...

glEnable(GL_DEPTH_TEST);     // Bật lại cho 3D objects  
glEnable(GL_LIGHTING);
```

## 💡 WHY THESE FEATURES?

### 🎯 Better Learning Experience:
- **Interactive** → Có thể khám phá robot từ mọi góc
- **Visual feedback** → Biết robot đang làm gì
- **Professional feel** → Giống các ứng dụng 3D thật

### 🛠️ Development Benefits:
- **Debug easier** → Nhìn robot từ góc khác để thấy vấn đề
- **Understanding** → Hiểu 3D transformations tốt hơn
- **Extensible** → Dễ thêm features khác sau này

## 🚀 EXPERIMENT IDEAS

### 🔧 Thử thay đổi:
```cpp
// Camera sensitivity
cameraAngleY += deltaX * 0.2f;  // Chậm hơn
cameraAngleY += deltaX * 1.0f;  // Nhanh hơn

// Zoom limits
if (cameraDistance < 3.0f) cameraDistance = 3.0f;   // Gần hơn
if (cameraDistance > 50.0f) cameraDistance = 50.0f; // Xa hơn

// UI position
glRasterPos3f(-2.0f, 3.0f, 0.0f);  // Di chuyển text
```

### 🎨 Thêm features:
- **Right-click menu** → Chọn camera presets
- **Keyboard shortcuts** → 1,2,3 cho different views
- **Status bar** → Hiển thị joint angles

## 🚀 BƯỚC TIẾP THEO

Đọc file `05_ANIMATION_SYSTEM.md` để tạo animation system hoàn chỉnh với tất cả các phases!

## 🐛 TROUBLESHOOTING

### ❌ Mouse không hoạt động:
- Check xem có register `glutMouseFunc()` và `glutMotionFunc()` không
- Thử comment mouse code để test từng phần

### ❌ UI không hiển thị:
- Check thứ tự `glDisable/glEnable` lighting
- Đảm bảo `glRasterPos3f()` trong viewport

### ❌ Camera jerky:
- Giảm sensitivity (0.5f → 0.2f)
- Check deltaX/deltaY calculations

---
> 💡 **Pro tip**: Thử nhấn giữ chuột và di chuyển để cảm nhận camera response! 