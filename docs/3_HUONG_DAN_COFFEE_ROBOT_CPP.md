# 🎮 HƯỚNG DẪN CHI TIẾT FILE COFFEE_ROBOT.CPP

## 📋 Mục lục
- [Tổng quan](#tổng-quan)
- [Cấu trúc file](#cấu-trúc-file)
- [Hệ thống Camera](#hệ-thống-camera)
- [Hệ thống điều khiển](#hệ-thống-điều-khiển)
- [Hệ thống Lighting](#hệ-thống-lighting)
- [Hệ thống Rendering](#hệ-thống-rendering)
- [Animation Loop](#animation-loop)
- [Cách thay đổi camera và góc nhìn](#cách-thay-đổi-camera-và-góc-nhìn)
- [Cách thay đổi controls](#cách-thay-đổi-controls)
- [Cách thay đổi ánh sáng và màu sắc](#cách-thay-đổi-ánh-sáng-và-màu-sắc)
- [Cách thay đổi giao diện](#cách-thay-đổi-giao-diện)
- [Ví dụ tùy chỉnh](#ví-dụ-tùy-chỉnh)
- [Troubleshooting](#troubleshooting)

## 🎯 Tổng quan

File `coffee_robot.cpp` là **main application file** chứa:
- **OpenGL Setup**: Khởi tạo 3D graphics engine
- **Camera System**: Điều khiển góc nhìn 3D
- **Input Handling**: Xử lý bàn phím và chuột
- **Animation Loop**: Vòng lặp cập nhật và vẽ
- **UI System**: Hiển thị trạng thái và hướng dẫn
- **Window Management**: Quản lý cửa sổ OpenGL

## 📁 Cấu trúc file

```
coffee_robot.cpp
├── Global Variables (Camera & Input)
├── OpenGL Initialization
│   ├── initGL() - Setup lighting, depth test
│   └── reshape() - Handle window resize
├── Main Rendering
│   ├── display() - Main draw function
│   └── Text rendering system
├── Input System
│   ├── keyboard() - Handle keys
│   ├── mouse() - Handle mouse clicks
│   ├── mouseMotion() - Handle mouse drag
│   └── specialKeys() - Handle arrow keys
├── Animation System
│   └── timer() - 60 FPS animation loop
├── Utility Functions
│   ├── printInstructions()
│   └── cleanup()
└── main() - Program entry point
```

## 📷 Hệ thống Camera

### 🎛️ Biến camera toàn cục
```cpp
float cameraAngleX = -25.0f;  // Góc nhìn từ trên xuống (âm để nhìn xuống)
float cameraAngleY = 45.0f;   // Góc nhìn chéo 
float cameraDistance = 15.0f; // Khoảng cách phù hợp
bool mousePressed = false;
int lastMouseX, lastMouseY;
```

### 📊 Ý nghĩa từng biến:

| Biến | Mục đích | Giá trị mặc định | Ảnh hưởng |
|------|----------|------------------|-----------|
| `cameraAngleX` | Góc xoay theo trục X (lên/xuống) | -25.0f | Âm = nhìn từ trên xuống |
| `cameraAngleY` | Góc xoay theo trục Y (trái/phải) | 45.0f | Dương = nhìn chéo |
| `cameraDistance` | Khoảng cách từ camera đến robot | 15.0f | Tăng = xa hơn, giảm = gần hơn |
| `mousePressed` | Trạng thái chuột | false | true khi đang kéo chuột |

### 🎮 Cách camera hoạt động trong display():
```cpp
void display() {
    // Setup camera transformation
    glTranslatef(0.0f, 0.0f, -cameraDistance);  // Di chuyển camera ra xa
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f); // Xoay theo X (lên/xuống)
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f); // Xoay theo Y (trái/phải)
}
```

## 🎮 Hệ thống điều khiển

### ⌨️ Keyboard Controls
```cpp
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':    // Space - bắt đầu pha cà phê
        case 'r':    // R - reset robot
        case 'R':    // R - reset robot (chữ hoa)
        case 27:     // ESC - thoát chương trình
    }
}
```

### 🖱️ Mouse Controls
```cpp
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;     // Bắt đầu kéo
            lastMouseX = x;         // Lưu vị trí chuột
            lastMouseY = y;
        } else {
            mousePressed = false;    // Kết thúc kéo
        }
    }
}
```

### 🖱️ Mouse Motion (Kéo chuột để xoay camera)
```cpp
void mouseMotion(int x, int y) {
    if (mousePressed) {
        float deltaX = x - lastMouseX;  // Độ di chuyển ngang
        float deltaY = y - lastMouseY;  // Độ di chuyển dọc
        
        cameraAngleY += deltaX * 0.5f;  // Xoay camera ngang
        cameraAngleX += deltaY * 0.5f;  // Xoay camera dọc
        
        // Giới hạn góc camera
        if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;   // Không xoay quá 90°
        if (cameraAngleX < -90.0f) cameraAngleX = -90.0f; // Không xoay quá -90°
    }
}
```

### 🔼 Special Keys (Phím mũi tên để zoom)
```cpp
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            cameraDistance -= 1.0f;                        // Zoom in
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;  // Giới hạn gần nhất
            break;
        case GLUT_KEY_DOWN:
            cameraDistance += 1.0f;                         // Zoom out
            if (cameraDistance > 30.0f) cameraDistance = 30.0f; // Giới hạn xa nhất
            break;
    }
}
```

## 💡 Hệ thống Lighting

### 🌟 Khởi tạo ánh sáng trong initGL()
```cpp
void initGL() {
    glEnable(GL_DEPTH_TEST);        // Bật depth testing (vật gần che vật xa)
    glEnable(GL_LIGHTING);          // Bật hệ thống ánh sáng
    glEnable(GL_LIGHT0);            // Bật đèn số 0
    glEnable(GL_COLOR_MATERIAL);    // Cho phép màu vật liệu
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Cài đặt material
    
    // Thiết lập ánh sáng
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};      // Vị trí đèn
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};     // Màu đèn (trắng)
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};   // Ánh sáng môi trường
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);          // Đặt vị trí đèn
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);         // Đặt màu khuếch tán
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);       // Đặt ánh sáng môi trường
    
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Màu nền xanh da trời nhạt
}
```

### 🔧 Ý nghĩa từng thông số ánh sáng:

| Thông số | Ý nghĩa | Giá trị | Ảnh hưởng |
|----------|---------|---------|-----------|
| `lightPos` | Vị trí đèn trong không gian 3D | (5, 10, 5) | Đèn ở trên cao, bên phải |
| `lightColor` | Màu ánh sáng chính | (1, 1, 1) | Trắng thuần |
| `ambientLight` | Ánh sáng môi trường | (0.3, 0.3, 0.3) | Sáng nhẹ để không có vùng tối tuyệt đối |
| `glClearColor` | Màu nền | (0.5, 0.7, 1.0) | Xanh da trời nhạt |

## 🖼️ Hệ thống Rendering

### 🎨 Hàm display() - Hàm vẽ chính
```cpp
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Xóa màn hình
    glLoadIdentity();                                   // Reset transformation
    
    // Set up camera
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    
    // Draw everything
    drawScene();    // Bàn, máy pha cà phê, cốc (từ robot_simple.cpp)
    drawRobot();    // Robot (từ robot_simple.cpp)
    
    // Vẽ text status...
    
    glutSwapBuffers(); // Hiển thị frame hoàn chỉnh
}
```

### 📝 Hệ thống hiển thị text
```cpp
// Text background - Nền đen trong suốt
glColor4f(0.0f, 0.0f, 0.0f, 0.6f);        // Đen với độ trong suốt 60%
glEnable(GL_BLEND);                        // Bật blending
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Công thức blending
glBegin(GL_QUADS);
glVertex3f(-4.0f, 4.5f, 0.0f);            // Góc trên trái
glVertex3f(4.0f, 4.5f, 0.0f);             // Góc trên phải  
glVertex3f(4.0f, 3.5f, 0.0f);             // Góc dưới phải
glVertex3f(-4.0f, 3.5f, 0.0f);            // Góc dưới trái
glEnd();

// Status text - Text vàng
glColor3f(1.0f, 1.0f, 0.0f);              // Màu vàng
glRasterPos3f(-3.8f, 4.2f, 0.0f);         // Vị trí text

const char* statusText[] = {
    "IDLE - Press SPACE to start making coffee",
    "Reaching for cup...",
    "Grabbing cup...", 
    "Turning to coffee machine...",
    "Pouring coffee...",
    "Returning to table...",
    "Placing cup down...",
    "Going home...",
    "FINISHED - Press R to reset, SPACE for another coffee"
};

const char* currentText = statusText[robot.phase]; // Lấy text theo pha hiện tại
```

## ⏰ Animation Loop

### 🔄 Hàm timer() - Vòng lặp 60 FPS
```cpp
void timer(int value) {
    updateRobot();                    // Cập nhật animation robot (từ robot_simple.cpp)
    glutPostRedisplay();              // Yêu cầu vẽ lại frame
    glutTimerFunc(16, timer, 0);      // Lặp lại sau 16ms (~60 FPS)
}
```

**Tính toán FPS:**
- 1000ms ÷ 16ms = 62.5 FPS
- Để có FPS khác: `1000 ÷ FPS_mong_muốn = thời_gian_delay`

## 🎮 Cách thay đổi camera và góc nhìn

### 1. **Thay đổi vị trí camera ban đầu**
```cpp
// Trong phần global variables
float cameraAngleX = -45.0f;  // Nhìn từ trên cao hơn
float cameraAngleY = 90.0f;   // Nhìn từ bên cạnh
float cameraDistance = 20.0f; // Xa hơn để nhìn toàn cảnh
```

### 2. **Thay đổi tốc độ xoay camera**
```cpp
// Trong mouseMotion()
cameraAngleY += deltaX * 0.2f;  // Chậm hơn (thay vì 0.5f)
cameraAngleX += deltaY * 0.8f;  // Nhanh hơn (thay vì 0.5f)
```

### 3. **Thay đổi giới hạn camera**
```cpp
// Trong mouseMotion()
if (cameraAngleX > 45.0f) cameraAngleX = 45.0f;   // Không cho nhìn quá cao
if (cameraAngleX < -60.0f) cameraAngleX = -60.0f; // Nhìn sâu hơn từ trên
```

### 4. **Thay đổi zoom range**
```cpp
// Trong specialKeys()
if (cameraDistance < 8.0f) cameraDistance = 8.0f;   // Gần hơn
if (cameraDistance > 50.0f) cameraDistance = 50.0f; // Xa hơn
```

### 5. **Camera tự động xoay**
```cpp
// Thêm vào timer() để camera tự xoay
void timer(int value) {
    updateRobot();
    
    // Camera tự xoay chậm
    cameraAngleY += 0.2f;
    if (cameraAngleY > 360.0f) cameraAngleY -= 360.0f;
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
```

## 🎮 Cách thay đổi controls

### 1. **Thêm phím mới**
```cpp
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ': // Space
            startCoffeeSequence();
            break;
            
        case 'r':
        case 'R': // Reset
            resetRobot();
            break;
            
        // Thêm controls mới
        case 'p':
        case 'P': // Pause animation
            robot.moving = !robot.moving;
            std::cout << (robot.moving ? "Resume" : "Pause") << std::endl;
            break;
            
        case 'f':
        case 'F': // Fast mode
            const float FAST_SPEED = 0.03f;
            // Thay đổi ANIMATION_SPEED trong robot.h
            break;
            
        case 's':
        case 'S': // Slow mode
            const float SLOW_SPEED = 0.005f;
            break;
            
        case 27: // ESC
            exit(0);
            break;
    }
}
```

### 2. **Thêm chuột phải để reset camera**
```cpp
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        // Logic cũ...
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // Reset camera về vị trí ban đầu
        cameraAngleX = -25.0f;
        cameraAngleY = 45.0f;
        cameraDistance = 15.0f;
        std::cout << "📷 Camera reset to default position" << std::endl;
        glutPostRedisplay();
    }
}
```

### 3. **Thêm wheel mouse cho zoom**
```cpp
// Thêm hàm mới
void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        cameraDistance -= 2.0f;  // Zoom in
        if (cameraDistance < 5.0f) cameraDistance = 5.0f;
    } else {
        cameraDistance += 2.0f;  // Zoom out  
        if (cameraDistance > 30.0f) cameraDistance = 30.0f;
    }
    glutPostRedisplay();
}

// Đăng ký trong main()
glutMouseWheelFunc(mouseWheel);
```

## 💡 Cách thay đổi ánh sáng và màu sắc

### 1. **Thay đổi vị trí đèn**
```cpp
// Trong initGL()
GLfloat lightPos[] = {-5.0f, 15.0f, 10.0f, 1.0f}; // Đèn bên trái, cao hơn
GLfloat lightPos[] = {0.0f, 20.0f, 0.0f, 1.0f};    // Đèn treo trên đầu
GLfloat lightPos[] = {10.0f, 5.0f, -5.0f, 1.0f};   // Đèn bên phải, thấp
```

### 2. **Thay đổi màu ánh sáng**
```cpp
// Ánh sáng ấm (vàng)
GLfloat lightColor[] = {1.0f, 0.9f, 0.7f, 1.0f};

// Ánh sáng lạnh (xanh)
GLfloat lightColor[] = {0.8f, 0.9f, 1.0f, 1.0f};

// Ánh sáng đỏ
GLfloat lightColor[] = {1.0f, 0.5f, 0.5f, 1.0f};
```

### 3. **Thay đổi độ sáng môi trường**
```cpp
// Tối hơn (dramatic)
GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};

// Sáng hơn (soft)
GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
```

### 4. **Thay đổi màu nền**
```cpp
// Nền đen (space theme)
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

// Nền trắng (clean theme)
glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

// Nền hoàng hôn
glClearColor(1.0f, 0.6f, 0.2f, 1.0f);

// Nền xanh dương đậm
glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
```

### 5. **Thêm nhiều đèn**
```cpp
void initGL() {
    // Bật nhiều đèn
    glEnable(GL_LIGHT0);  // Đèn chính
    glEnable(GL_LIGHT1);  // Đèn phụ
    
    // Đèn 0 - Đèn chính
    GLfloat light0Pos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    GLfloat light0Color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);
    
    // Đèn 1 - Đèn tạo bóng
    GLfloat light1Pos[] = {-3.0f, 8.0f, -3.0f, 1.0f};
    GLfloat light1Color[] = {0.5f, 0.5f, 0.8f, 1.0f}; // Xanh nhạt
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Color);
}
```

## 🖼️ Cách thay đổi giao diện

### 1. **Thay đổi vị trí text status**
```cpp
// Text ở góc trên trái
glRasterPos3f(-4.8f, 4.8f, 0.0f);

// Text ở giữa màn hình
glRasterPos3f(-2.0f, 0.0f, 0.0f);

// Text ở dưới cùng
glRasterPos3f(-3.8f, -4.0f, 0.0f);
```

### 2. **Thay đổi màu text**
```cpp
// Text đỏ
glColor3f(1.0f, 0.0f, 0.0f);

// Text xanh lá
glColor3f(0.0f, 1.0f, 0.0f);

// Text trắng
glColor3f(1.0f, 1.0f, 1.0f);

// Text với hiệu ứng nhấp nháy
float blink = sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 0.5f + 0.5f;
glColor3f(1.0f, blink, 0.0f); // Vàng nhấp nháy
```

### 3. **Thay đổi font chữ**
```cpp
// Font nhỏ hơn
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, currentText[i]);

// Font to hơn
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_24, currentText[i]);

// Font khác
glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, currentText[i]);
glutBitmapCharacter(GLUT_BITMAP_9_BY_15, currentText[i]);
```

### 4. **Thay đổi nền text**
```cpp
// Nền đỏ trong suốt
glColor4f(1.0f, 0.0f, 0.0f, 0.3f);

// Nền xanh dương đậm
glColor4f(0.0f, 0.0f, 0.5f, 0.8f);

// Không có nền (xóa phần vẽ nền)
// Bỏ qua phần glBegin(GL_QUADS)...glEnd()
```

### 5. **Thêm FPS counter**
```cpp
// Thêm vào display()
static int frameCount = 0;
static int lastTime = 0;
static float fps = 0.0f;

frameCount++;
int currentTime = glutGet(GLUT_ELAPSED_TIME);
if (currentTime - lastTime > 1000) { // Cập nhật mỗi giây
    fps = frameCount * 1000.0f / (currentTime - lastTime);
    frameCount = 0;
    lastTime = currentTime;
}

// Hiển thị FPS
char fpsText[50];
sprintf(fpsText, "FPS: %.1f", fps);
glRasterPos3f(3.0f, 4.2f, 0.0f);
for (int i = 0; fpsText[i] != '\0'; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, fpsText[i]);
}
```

## 💡 Ví dụ tùy chỉnh

### Ví dụ 1: Camera thứ 3 (Third Person)
```cpp
// Camera theo sau robot từ phía sau
void thirdPersonCamera() {
    float robotX = 0.0f; // Vị trí robot
    float robotY = 1.0f;
    float robotZ = 0.0f;
    
    float cameraOffsetX = sin(robot.body * M_PI / 180.0f) * 8.0f;
    float cameraOffsetZ = cos(robot.body * M_PI / 180.0f) * 8.0f;
    
    glTranslatef(-cameraOffsetX, -3.0f, -cameraOffsetZ);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f); // Nhìn hơi xuống
    glRotatef(-robot.body, 0.0f, 1.0f, 0.0f); // Xoay theo robot
}
```

### Ví dụ 2: Chế độ debug với nhiều thông tin
```cpp
void drawDebugInfo() {
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 1.0f, 0.0f); // Xanh lá
    
    char debugText[200];
    sprintf(debugText, "Phase: %d | Progress: %.2f | Body: %.1f°", 
            robot.phase, robot.progress, robot.body);
    
    glRasterPos3f(-4.8f, -4.5f, 0.0f);
    for (int i = 0; debugText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, debugText[i]);
    }
    
    glEnable(GL_LIGHTING);
}

// Thêm vào display()
drawDebugInfo();
```

### Ví dụ 3: Night mode
```cpp
void setupNightMode() {
    // Nền đen
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    
    // Ánh sáng yếu, màu xanh
    GLfloat lightColor[] = {0.3f, 0.4f, 0.6f, 1.0f};
    GLfloat ambientLight[] = {0.05f, 0.05f, 0.1f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
}
```

### Ví dụ 4: Cinematic camera
```cpp
void cinematicCamera() {
    static float cinematicTime = 0.0f;
    cinematicTime += 0.02f;
    
    // Camera quay quanh robot
    float radius = 12.0f + sin(cinematicTime * 0.5f) * 3.0f; // Zoom in/out
    float angle = cinematicTime * 20.0f; // Xoay chậm
    float height = 8.0f + cos(cinematicTime * 0.3f) * 2.0f; // Lên xuống
    
    float camX = sin(angle * M_PI / 180.0f) * radius;
    float camZ = cos(angle * M_PI / 180.0f) * radius;
    
    glTranslatef(-camX, -height, -camZ);
    glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-angle, 0.0f, 1.0f, 0.0f);
}
```

### Ví dụ 5: Multiple viewports
```cpp
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Viewport 1 - Camera chính (nửa trái)
    glViewport(0, 0, 512, 768);
    glLoadIdentity();
    // Setup camera bình thường...
    drawScene();
    drawRobot();
    
    // Viewport 2 - Camera từ trên (nửa phải)
    glViewport(512, 0, 512, 768);
    glLoadIdentity();
    glTranslatef(0.0f, -15.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Nhìn từ trên xuống
    drawScene();
    drawRobot();
    
    glutSwapBuffers();
}
```

## ⚠️ Troubleshooting

### 🐛 Vấn đề thường gặp

1. **Màn hình đen khi chạy**
   ```cpp
   // Kiểm tra:
   - glClearColor() có được gọi không
   - Lighting có được setup đúng không
   - Camera distance có quá xa không (> 50)
   ```

2. **Camera không di chuyển được**
   ```cpp
   // Kiểm tra:
   - mouseMotion() có được đăng ký không: glutMotionFunc(mouseMotion)
   - mousePressed có được set true không
   - deltaX, deltaY có giá trị không
   ```

3. **Text không hiển thị**
   ```cpp
   // Kiểm tra:
   - glDisable(GL_LIGHTING) trước khi vẽ text
   - glColor3f() có được gọi không
   - glRasterPos3f() trong phạm vi nhìn thấy
   ```

4. **Robot không chuyển động**
   ```cpp
   // Kiểm tra:
   - timer() có được đăng ký: glutTimerFunc(16, timer, 0)
   - updateRobot() có được gọi trong timer()
   - robot.moving có = true không
   ```

5. **Tốc độ khung hình thấp**
   ```cpp
   // Giải pháp:
   - Giảm resolution: glutInitWindowSize(800, 600)
   - Giảm complexity: Ít polygon hơn
   - Tăng timer delay: glutTimerFunc(33, timer, 0) // 30 FPS
   ```

6. **Ánh sáng không hoạt động**
   ```cpp
   // Kiểm tra:
   glEnable(GL_LIGHTING);     // Bật lighting
   glEnable(GL_LIGHT0);       // Bật đèn
   glEnable(GL_COLOR_MATERIAL); // Bật material
   glLightfv() được gọi sau glEnable()
   ```

7. **Window không tạo được**
   ```cpp
   // Kiểm tra:
   - glutInit() được gọi đầu tiên
   - glutInitDisplayMode() có đủ flags
   - OpenGL driver có được cài đặt không
   ```

### 🔧 Debug tips

```cpp
// Debug camera
printf("Camera: AngleX=%.1f, AngleY=%.1f, Distance=%.1f\n", 
       cameraAngleX, cameraAngleY, cameraDistance);

// Debug mouse
printf("Mouse: Pressed=%s, X=%d, Y=%d\n", 
       mousePressed ? "Yes" : "No", lastMouseX, lastMouseY);

// Debug OpenGL
GLenum error = glGetError();
if (error != GL_NO_ERROR) {
    printf("OpenGL Error: %d\n", error);
}

// Performance monitoring
static int frameCount = 0;
frameCount++;
if (frameCount % 60 == 0) {
    printf("60 frames rendered\n");
}
```

### 🎯 Performance tips

```cpp
// Giảm chi phí rendering
1. Sử dụng displayLists cho vật tĩnh
2. Giảm số polygon của sphere, cylinder
3. Tắt lighting khi không cần: glDisable(GL_LIGHTING)
4. Sử dụng culling: glEnable(GL_CULL_FACE)
5. Giảm resolution window nếu cần
```

---
📝 **Ghi chú**: File này được đồng bộ với coffee_robot.cpp version hiện tại. Kiểm tra code mới nhất khi có thay đổi. 