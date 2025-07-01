# BƯỚC 3: ĐỊNH NGHĨA CẤU TRÚC ROBOT

## 🎯 MỤC TIÊU BƯỚC NÀY
Tổ chức code thành nhiều files và tạo cấu trúc dữ liệu cho robot:
- Tạo `robot.h` - Header file với struct Robot và constants
- Tạo `robot.cpp` - Implementation các hàm robot
- Chia nhỏ `main.cpp` để dễ quản lý
- Định nghĩa các giai đoạn hoạt động của robot

## 📁 TỔNG TỔ CHỨC FILES

Từ bước này, chúng ta sẽ có 3 files:
```
coffee_robot_tutorial/
├── robot.h        # Header - definitions, structs, constants
├── robot.cpp      # Robot logic - drawing, animation  
├── main.cpp       # Main program - OpenGL setup, events
└── Makefile       # Build script
```

## 📄 File 1: `robot.h` (Header file)

Tạo file mới `robot.h`:

```cpp
#ifndef ROBOT_H
#define ROBOT_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>

//=============================================================================
// CONSTANTS - Các hằng số dễ điều chỉnh
//=============================================================================
const float ANIMATION_SPEED = 0.02f;   // Tốc độ animation
const float ARM_UPPER = 1.6f;          // Chiều dài cánh tay trên
const float ARM_LOWER = 1.4f;          // Chiều dài cánh tay dưới
const float CUP_SIZE = 0.25f;          // Bán kính cốc
const float TABLE_Y = 0.1f;            // Độ cao bàn

//=============================================================================
// ROBOT PHASES - Các giai đoạn hoạt động
//=============================================================================
enum RobotPhase {
    IDLE = 0,           // Đứng yên
    REACH_CUP,          // Với tới cốc
    GRAB_CUP,           // Cầm cốc
    TURN_TO_MACHINE,    // Xoay về máy pha cà phê
    POUR_COFFEE,        // Pha cà phê
    RETURN_CUP,         // Mang cốc về
    PLACE_CUP,          // Đặt cốc xuống
    GO_HOME,            // Về vị trí ban đầu
    FINISHED            // Hoàn thành
};

//=============================================================================
// ROBOT STRUCT - Trạng thái của robot
//=============================================================================
struct Robot {
    // Góc các khớp (degrees)
    float shoulderY, shoulderZ;     // Xoay vai (Y=ngang, Z=dọc)
    float elbow;                    // Gập khuỷu tay
    float wristZ, wristY;          // Xoay cổ tay
    float fingers;                  // Mở/đóng gripper (0=đóng, 90=mở)
    float body;                     // Xoay thân robot
    
    // Animation state
    RobotPhase phase;               // Giai đoạn hiện tại
    float progress;                 // Tiến độ từ 0.0 đến 1.0
    bool moving;                    // Có đang chuyển động không
    
    // Cup state
    float cupX, cupY, cupZ;         // Vị trí cốc
    bool holdingCup;                // Có đang cầm cốc không
    bool cupHasCoffee;              // Cốc có cà phê không
};

//=============================================================================
// FUNCTION DECLARATIONS - Khai báo các hàm
//=============================================================================

// Basic drawing functions
void drawBox(float width, float height, float depth, float r, float g, float b);
void drawCylinder(float radius, float height, float r, float g, float b);
void drawSphere(float radius, float r, float g, float b);

// Robot functions
void initRobot();               // Khởi tạo robot
void updateRobot();             // Cập nhật animation
void startCoffeeSequence();     // Bắt đầu pha cà phê
void resetRobot();              // Reset robot

void drawRobot();               // Vẽ robot
void drawScene();               // Vẽ scene (bàn, máy, cốc)

//=============================================================================
// GLOBAL ROBOT INSTANCE
//=============================================================================
extern Robot robot;             // Robot toàn cục

#endif // ROBOT_H
```

## 📄 File 2: `robot.cpp` (Implementation)

Tạo file mới `robot.cpp`:

```cpp
#include "robot.h"
#include <cstdio>

//=============================================================================
// GLOBAL ROBOT INSTANCE
//=============================================================================
Robot robot;

//=============================================================================
// BASIC DRAWING FUNCTIONS - Di chuyển từ main.cpp
//=============================================================================
void drawBox(float width, float height, float depth, float r, float g, float b) {
    glColor3f(r, g, b);
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawCylinder(float radius, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, radius, radius, height, 20, 1);
    gluDeleteQuadric(quad);
}

void drawSphere(float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glutSolidSphere(radius, 15, 15);
}

//=============================================================================
// ROBOT CONTROL FUNCTIONS
//=============================================================================
void initRobot() {
    // Đặt robot về pose ban đầu
    robot.shoulderY = robot.shoulderZ = 0.0f;
    robot.elbow = 0.0f;
    robot.wristZ = robot.wristY = 0.0f;
    robot.fingers = 90.0f;  // Gripper mở
    robot.body = 0.0f;
    
    // Animation state
    robot.phase = IDLE;
    robot.progress = 0.0f;
    robot.moving = false;
    
    // Cup state  
    robot.cupX = 2.2f;      // Cốc ở bên phải
    robot.cupY = TABLE_Y + 0.25f;
    robot.cupZ = 0.0f;
    robot.holdingCup = false;
    robot.cupHasCoffee = false;
}

void startCoffeeSequence() {
    if (robot.moving) return;
    
    robot.moving = true;
    robot.phase = REACH_CUP;
    robot.progress = 0.0f;
    printf("🤖 Bắt đầu pha cà phê...\n");
}

void resetRobot() {
    initRobot();
    printf("🔄 Robot đã reset\n");
}

void updateRobot() {
    if (!robot.moving) return;
    
    // Tăng progress
    robot.progress += ANIMATION_SPEED;
    if (robot.progress >= 1.0f) {
        robot.progress = 1.0f;
        
        // Chuyển sang phase tiếp theo
        switch (robot.phase) {
            case REACH_CUP:
                robot.phase = GRAB_CUP;
                robot.progress = 0.0f;
                printf("📋 Đang cầm cốc...\n");
                break;
                
            case GRAB_CUP:
                robot.phase = FINISHED;  // Tạm thời dừng ở đây
                robot.moving = false;
                printf("✅ Phase đầu hoàn thành!\n");
                break;
                
            default:
                robot.moving = false;
                break;
        }
    }
    
    // Cập nhật robot joints dựa trên phase
    float t = robot.progress;
    
    switch (robot.phase) {
        case REACH_CUP:
            // Di chuyển tay về phía cốc
            robot.shoulderZ = t * -30.0f;
            robot.elbow = t * -40.0f;
            robot.wristZ = t * 30.0f;
            robot.shoulderY = t * 10.0f;
            break;
            
        case GRAB_CUP:
            // Đóng gripper
            robot.fingers = 90.0f - t * 60.0f;  // Từ 90 xuống 30
            break;
            
        default:
            break;
    }
}

//=============================================================================
// DRAWING FUNCTIONS
//=============================================================================
void drawRobotBase() {
    // Đế robot
    glPushMatrix();
    glTranslatef(0, -1, 0);
    drawBox(1.8f, 0.2f, 1.8f, 0.3f, 0.3f, 0.4f);
    glPopMatrix();
    
    // Cột trung tâm
    glPushMatrix();
    drawCylinder(0.5f, 1.0f, 0.5f, 0.5f, 0.6f);
    glPopMatrix();
}

void drawRobotArm() {
    glPushMatrix();
    
    // Di chuyển đến vai
    glTranslatef(0, 1, 0);
    glRotatef(robot.shoulderY, 0, 1, 0);  // Xoay vai ngang
    glRotatef(robot.shoulderZ, 0, 0, 1);  // Xoay vai dọc
    
    // Khớp vai
    drawSphere(0.4f, 0.7f, 0.7f, 0.8f);
    
    // Cánh tay trên
    glTranslatef(ARM_UPPER/2, 0, 0);
    drawBox(ARM_UPPER, 0.5f, 0.5f, 0.6f, 0.6f, 0.7f);
    
    // Khớp khuỷu
    glTranslatef(ARM_UPPER/2, 0, 0);
    glRotatef(robot.elbow, 0, 0, 1);
    drawSphere(0.3f, 0.7f, 0.7f, 0.8f);
    
    // Cánh tay dưới
    glTranslatef(ARM_LOWER/2, 0, 0);
    drawBox(ARM_LOWER, 0.4f, 0.4f, 0.6f, 0.6f, 0.7f);
    
    // Cổ tay
    glTranslatef(ARM_LOWER/2, 0, 0);
    glRotatef(robot.wristZ, 0, 0, 1);
    glRotatef(robot.wristY, 0, 1, 0);
    drawSphere(0.2f, 0.7f, 0.7f, 0.8f);
    
    // Gripper đơn giản
    glTranslatef(0.2f, 0, 0);
    drawBox(0.3f, 0.4f, 0.3f, 0.4f, 0.4f, 0.5f);
    
    // Ngón gripper
    glPushMatrix();
    glTranslatef(0.15f, 0.15f, 0);
    glRotatef(-robot.fingers, 0, 0, 1);
    glTranslatef(0.15f, 0, 0);
    drawBox(0.3f, 0.08f, 0.2f, 0.5f, 0.5f, 0.6f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.15f, -0.15f, 0);
    glRotatef(robot.fingers, 0, 0, 1);
    glTranslatef(0.15f, 0, 0);
    drawBox(0.3f, 0.08f, 0.2f, 0.5f, 0.5f, 0.6f);
    glPopMatrix();
    
    glPopMatrix();
}

void drawRobot() {
    glPushMatrix();
    glTranslatef(0, 1.1f, 0);  // Nâng robot lên trên bàn
    
    drawRobotBase();
    
    glPushMatrix();
    glRotatef(robot.body, 0, 1, 0);  // Xoay thân
    
    // Thân robot
    glPushMatrix();
    glTranslatef(0, 0.7f, 0);
    drawBox(1.2f, 1.4f, 1.2f, 0.8f, 0.8f, 0.9f);
    glPopMatrix();
    
    drawRobotArm();
    
    glPopMatrix();
    glPopMatrix();
}

void drawScene() {
    // Bàn
    glPushMatrix();
    drawBox(10.0f, 0.2f, 8.0f, 0.6f, 0.4f, 0.2f);  // Nâu
    glPopMatrix();
    
    // Cốc đơn giản (tạm thời)
    glPushMatrix();
    glTranslatef(robot.cupX, robot.cupY, robot.cupZ);
    drawCylinder(CUP_SIZE, 0.5f, 1.0f, 1.0f, 1.0f);  // Trắng
    glPopMatrix();
}
```

## 📄 File 3: `main.cpp` (Simplified)

Cập nhật `main.cpp` để sử dụng robot.h:

```cpp
#include "robot.h"
#include <iostream>

//=============================================================================
// CAMERA VARIABLES
//=============================================================================
float cameraDistance = 15.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

//=============================================================================
// OPENGL SETUP
//=============================================================================
void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    
    // Khởi tạo robot
    initRobot();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Camera
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    
    // Vẽ scene và robot
    drawScene();
    drawRobot();
    
    glutSwapBuffers();
}

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
            std::cout << "Thoát..." << std::endl;
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void timer(int value) {
    updateRobot();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // ~60 FPS
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    
    int window = glutCreateWindow("Coffee Robot Tutorial - Step 3");
    if (window == 0) {
        std::cerr << "Không thể tạo cửa sổ!" << std::endl;
        return -1;
    }
    
    std::cout << "=== COFFEE ROBOT TUTORIAL - STEP 3 ===" << std::endl;
    std::cout << "SPACE - Bắt đầu animation" << std::endl;
    std::cout << "R     - Reset robot" << std::endl;
    std::cout << "ESC   - Thoát" << std::endl;
    
    initGL();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);  // Start animation timer
    
    glutMainLoop();
    return 0;
}
```

## 📄 File 4: `Makefile` (Updated)

```makefile
CC = g++
CFLAGS = -Wall -std=c++11
LIBS = -lGL -lGLU -lglut

TARGET = coffee_robot_step3
SOURCES = main.cpp robot.cpp

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
```

## 🔧 COMPILE VÀ TEST

```bash
# Compile
make

# Chạy
./coffee_robot_step3
```

## ✅ KẾT QUẢ MONG ĐỢI

- Thấy robot đứng trên bàn nâu
- Có một cốc trắng ở bên phải
- Nhấn SPACE: robot bắt đầu duỗi tay về phía cốc
- Nhấn R: robot reset về pose ban đầu

## 🔍 NHỮNG GÌ ĐÃ HỌC

### 🗂️ Code Organization:
- **Header file** (.h) cho declarations
- **Implementation file** (.cpp) cho actual code
- **Separation of concerns** - main.cpp chỉ lo OpenGL setup

### 🤖 Robot Structure:
- **Joint angles** - các góc khớp để định pose
- **Animation state** - phase hiện tại, progress
- **Object state** - vị trí cốc, trạng thái gripper

### ⚡ Animation System:
- **Progress-based** - từ 0.0 đến 1.0
- **Phase-based** - mỗi giai đoạn có hành vi riêng
- **Smooth transitions** - dùng progress để interpolate

## 🚀 BƯỚC TIẾP THEO

Đọc file `04_VE_ROBOT_TINH.md` để hoàn thiện việc vẽ robot và thêm nhiều chi tiết hơn!

---
> 💡 **Quan trọng**: Từ bước này trở đi, bạn đã có foundation vững chắc để build các tính năng phức tạp hơn! 