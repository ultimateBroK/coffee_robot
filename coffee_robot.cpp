#include "robot.h"
#include <iostream>

// Biến camera
float cameraAngleX = -25.0f;  // Góc nhìn từ trên xuống (âm để nhìn xuống)
float cameraAngleY = 45.0f;   // Góc nhìn chéo 
float cameraDistance = 15.0f; // Khoảng cách phù hợp
bool mousePressed = false;
int lastMouseX, lastMouseY;

// Khởi tạo OpenGL
void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Thiết lập ánh sáng
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Màu nền xanh da trời nhạt
    
    // Khởi tạo robot
    initRobot();
}

// Hàm vẽ
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Thiết lập camera
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    
    // Vẽ môi trường
    drawEnvironment();
    
    // Vẽ robot
    drawRobot();
    
    // Hiển thị trạng thái với background
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Vẽ background cho text
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex3f(-4.0f, 4.5f, 0.0f);
    glVertex3f(4.0f, 4.5f, 0.0f);
    glVertex3f(4.0f, 3.5f, 0.0f);
    glVertex3f(-4.0f, 3.5f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);
    
    // Text trạng thái
    glColor3f(1.0f, 1.0f, 0.0f);  // Màu vàng nổi bật
    glRasterPos3f(-3.8f, 4.2f, 0.0f);
    
    const char* phaseText[] = {
        "IDLE - Press SPACE to start brewing coffee",
        "Phase 1/7: Reaching for cup...",
        "Phase 2/7: Grabbing cup...",
        "Phase 3/7: Turning to coffee machine...",
        "Phase 4/7: Brewing coffee...",
        "Phase 5/7: Returning to table...",
        "Phase 6/7: Placing cup...",
        "Phase 7/7: Retracting arm...",
        "COMPLETE - Press R to reset, SPACE to brew again"
    };
    
    const char* currentText = phaseText[robot.currentPhase];
    for (int i = 0; currentText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, currentText[i]);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glutSwapBuffers();
}

// Xử lý phím
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ': // Space - bắt đầu animation
            if (!robot.isAnimating) {
                startAnimation();
                std::cout << "🤖 Starting coffee brewing sequence..." << std::endl;
            } else {
                std::cout << "⚠️  Animation already in progress. Press R to reset." << std::endl;
            }
            break;
            
        case 'r':
        case 'R': // Reset
            resetRobot();
            std::cout << "🔄 Robot reset to initial position." << std::endl;
            break;
            
        case 27: // ESC - thoát
            std::cout << "👋 Goodbye! Thanks for watching the coffee robot demo." << std::endl;
            exit(0);
            break;
            
        default:
            break;
    }
    glutPostRedisplay();
}

// Xử lý chuột
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

// Xử lý di chuyển chuột
void mouseMotion(int x, int y) {
    if (mousePressed) {
        float deltaX = x - lastMouseX;
        float deltaY = y - lastMouseY;
        
        cameraAngleY += deltaX * 0.5f;
        cameraAngleX += deltaY * 0.5f;
        
        // Giới hạn góc camera
        if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;
        if (cameraAngleX < -90.0f) cameraAngleX = -90.0f;
        
        lastMouseX = x;
        lastMouseY = y;
        
        glutPostRedisplay();
    }
}

// Xử lý lăn chuột (zoom) - Sử dụng phím thay thế
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            cameraDistance -= 1.0f;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;
            break;
        case GLUT_KEY_DOWN:
            cameraDistance += 1.0f;
            if (cameraDistance > 30.0f) cameraDistance = 30.0f;
            break;
    }
    glutPostRedisplay();
}

// Cập nhật animation
void timer(int value) {
    updateAnimation();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Thay đổi kích thước cửa sổ
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Hiển thị hướng dẫn
void printInstructions() {
    std::cout << "\n=== ROBOT COFFEE MAKER DEMO ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  SPACE  - Start coffee brewing animation" << std::endl;
    std::cout << "  R      - Reset robot to initial position" << std::endl;
    std::cout << "  ESC    - Exit program" << std::endl;
    std::cout << "  Mouse  - Left click + drag to rotate camera" << std::endl;
    std::cout << "  UP/DOWN- Zoom in/out" << std::endl;
    std::cout << "\nAnimation Phases:" << std::endl;
    std::cout << "  1. Reach for cup" << std::endl;
    std::cout << "  2. Grab cup" << std::endl;
    std::cout << "  3. Turn to coffee machine" << std::endl;
    std::cout << "  4. Pour coffee" << std::endl;
    std::cout << "  5. Return cup to table" << std::endl;
    std::cout << "  6. Place cup" << std::endl;
    std::cout << "  7. Retract arm" << std::endl;
    std::cout << "================================\n" << std::endl;
}

int main(int argc, char** argv) {
    // Khởi tạo GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Robot Coffee Maker Demo - OpenGL 3D Animation");
    
    // Hiển thị hướng dẫn
    printInstructions();
    
    // Khởi tạo OpenGL
    initGL();
    
    // Đăng ký callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, timer, 0);
    
    // Bắt đầu vòng lặp chính
    glutMainLoop();
    
    return 0;
}
