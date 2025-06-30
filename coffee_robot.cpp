#include "robot.h"
#include <iostream>

//=============================================================================
// BIẾN TOÀN CỤC - Camera và điều khiển chuột
//=============================================================================
float cameraAngleX = -25.0f;  // Góc nhìn từ trên xuống (âm để nhìn xuống)
float cameraAngleY = 45.0f;   // Góc nhìn chéo 
float cameraDistance = 15.0f; // Khoảng cách camera đến robot
bool mousePressed = false;     // Trạng thái chuột có đang được nhấn
int lastMouseX, lastMouseY;    // Vị trí chuột lần cuối

//=============================================================================
// KHỞI TẠO OPENGL - Thiết lập ánh sáng và môi trường 3D
//=============================================================================
void initGL() {
    glEnable(GL_DEPTH_TEST);      // Bật kiểm tra độ sâu (vật gần che vật xa)
    glEnable(GL_LIGHTING);        // Bật hệ thống ánh sáng
    glEnable(GL_LIGHT0);          // Bật đèn số 0
    glEnable(GL_COLOR_MATERIAL);  // Cho phép màu sắc vật liệu
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Thiết lập ánh sáng chính
    GLfloat lightPos[] = {5.0f, 10.0f, 5.0f, 1.0f};      // Vị trí đèn (trên cao, bên phải)
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};     // Màu trắng thuần
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};   // Ánh sáng môi trường nhẹ
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);          // Đặt vị trí đèn
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);         // Đặt màu ánh sáng khuếch tán
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);       // Đặt ánh sáng môi trường
    
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Màu nền xanh da trời nhạt
    
    // Khởi tạo robot về vị trí ban đầu
    initRobot();
}

//=============================================================================
// HÀM VẼ CHÍNH - Render toàn bộ scene 3D
//=============================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Xóa buffer màu và độ sâu
    glLoadIdentity();                                   // Reset ma trận transformation
    
    // Thiết lập camera 3D
    glTranslatef(0.0f, 0.0f, -cameraDistance);         // Di chuyển camera ra xa
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);         // Xoay camera lên/xuống
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);         // Xoay camera trái/phải
    
    // Vẽ toàn bộ scene
    drawScene();    // Bàn, máy pha cà phê, cốc
    drawRobot();    // Robot với tất cả các khớp
    
    // Hiển thị thông tin trạng thái
    glDisable(GL_LIGHTING);    // Tắt ánh sáng để vẽ UI
    glDisable(GL_DEPTH_TEST);  // Tắt depth test để UI luôn hiển thị trên cùng
    
    // Vẽ nền cho text trạng thái
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);  // Màu đen trong suốt 60%
    glEnable(GL_BLEND);                   // Bật chế độ pha trộn màu
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);                   // Vẽ hình chữ nhật làm nền
    glVertex3f(-4.0f, 4.5f, 0.0f);      // Góc trên trái
    glVertex3f(4.0f, 4.5f, 0.0f);       // Góc trên phải
    glVertex3f(4.0f, 3.5f, 0.0f);       // Góc dưới phải
    glVertex3f(-4.0f, 3.5f, 0.0f);      // Góc dưới trái
    glEnd();
    glDisable(GL_BLEND);                 // Tắt chế độ pha trộn
    
    // Vẽ text trạng thái robot
    glColor3f(1.0f, 1.0f, 0.0f);        // Màu vàng cho text
    glRasterPos3f(-3.8f, 4.2f, 0.0f);   // Vị trí bắt đầu vẽ text
    
    const char* statusText[] = {
        "NGHỈ - Nhấn SPACE để bắt đầu pha cà phê",
        "Đang duỗi tay tới cốc...",
        "Đang cầm lấy cốc...", 
        "Đang xoay về phía máy pha cà phê...",
        "Đang rót cà phê...",
        "Đang mang cốc về bàn...",
        "Đang đặt cốc xuống...",
        "Đang về vị trí ban đầu...",
        "HOÀN THÀNH - Nhấn R để reset, SPACE để pha thêm"
    };
    
    const char* currentText = statusText[robot.phase];   // Lấy text tương ứng với pha hiện tại
    for (int i = 0; currentText[i] != '\0'; i++) {      // Vẽ từng ký tự
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, currentText[i]);
    }
    
    glEnable(GL_DEPTH_TEST);    // Bật lại depth test
    glEnable(GL_LIGHTING);      // Bật lại ánh sáng
    
    glutSwapBuffers();          // Hiển thị frame hoàn chỉnh (double buffering)
}

//=============================================================================
// XỬ LÝ BÀN PHÍM - Điều khiển robot và chương trình
//=============================================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ': // Phím Space - bắt đầu quy trình pha cà phê
            if (!robot.moving) {
                startCoffeeSequence();
                std::cout << "🤖 Bắt đầu quy trình pha cà phê..." << std::endl;
            } else {
                std::cout << "⚠️  Robot đang bận. Nhấn R để reset." << std::endl;
            }
            break;
            
        case 'r':
        case 'R': // Phím R - reset robot về vị trí ban đầu
            resetRobot();
            std::cout << "🔄 Robot đã được reset về vị trí ban đầu." << std::endl;
            break;
            
        case 27: // Phím ESC - thoát chương trình
            std::cout << "👋 Tạm biệt!" << std::endl;
            exit(0);
            break;
    }
    glutPostRedisplay(); // Yêu cầu vẽ lại màn hình
}

//=============================================================================
// XỬ LÝ CHUỘT - Điều khiển camera bằng chuột
//=============================================================================
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {  // Chỉ xử lý chuột trái
        if (state == GLUT_DOWN) {      // Khi nhấn chuột xuống
            mousePressed = true;        // Đánh dấu đang kéo chuột
            lastMouseX = x;            // Lưu vị trí X ban đầu
            lastMouseY = y;            // Lưu vị trí Y ban đầu
        } else {                       // Khi thả chuột ra
            mousePressed = false;       // Kết thúc việc kéo chuột
        }
    }
}

//=============================================================================
// XỬ LÝ DI CHUYỂN CHUỘT - Xoay camera khi kéo chuột
//=============================================================================
void mouseMotion(int x, int y) {
    if (mousePressed) {                        // Chỉ xử lý khi đang kéo chuột
        float deltaX = x - lastMouseX;         // Độ di chuyển theo trục X
        float deltaY = y - lastMouseY;         // Độ di chuyển theo trục Y
        
        cameraAngleY += deltaX * 0.5f;         // Xoay camera ngang (tỷ lệ 0.5)
        cameraAngleX += deltaY * 0.5f;         // Xoay camera dọc (tỷ lệ 0.5)
        
        // Giới hạn góc camera để không xoay quá mức
        if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;    // Không nhìn quá cao
        if (cameraAngleX < -90.0f) cameraAngleX = -90.0f;  // Không nhìn quá thấp
        
        lastMouseX = x;                        // Cập nhật vị trí chuột mới
        lastMouseY = y;
        
        glutPostRedisplay();                   // Yêu cầu vẽ lại với góc camera mới
    }
}

//=============================================================================
// XỬ LÝ PHÍM ĐẶC BIỆT - Zoom camera bằng phím mũi tên
//=============================================================================
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:                      // Phím mũi tên lên - zoom in
            cameraDistance -= 1.0f;            // Giảm khoảng cách = zoom in
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;  // Giới hạn zoom in tối đa
            break;
        case GLUT_KEY_DOWN:                    // Phím mũi tên xuống - zoom out
            cameraDistance += 1.0f;            // Tăng khoảng cách = zoom out
            if (cameraDistance > 30.0f) cameraDistance = 30.0f; // Giới hạn zoom out tối đa
            break;
    }
    glutPostRedisplay();                       // Yêu cầu vẽ lại với zoom mới
}

//=============================================================================
// TIMER ANIMATION - Cập nhật animation với tốc độ 60 FPS
//=============================================================================
void timer(int value) {
    updateRobot();                    // Cập nhật trạng thái robot (từ robot_simple.cpp)
    glutPostRedisplay();              // Yêu cầu vẽ lại frame mới
    glutTimerFunc(16, timer, 0);      // Lặp lại sau 16ms (~60 FPS)
}

//=============================================================================
// XỬ LÝ THAY ĐỔI KÍCH THƯỚC CỬA SỔ - Điều chỉnh viewport và perspective
//=============================================================================
void reshape(int width, int height) {
    if (height == 0) height = 1;     // Tránh chia cho 0
    
    glViewport(0, 0, width, height); // Đặt viewport theo kích thước cửa sổ mới
    glMatrixMode(GL_PROJECTION);     // Chuyển sang chế độ projection matrix
    glLoadIdentity();                // Reset projection matrix
    
    // Thiết lập perspective với tỷ lệ khung hình mới
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);      // Chuyển về chế độ modelview matrix
}

//=============================================================================
// HIỂN THỊ HƯỚNG DẪN - In ra console các phím điều khiển
//=============================================================================
void printInstructions() {
    std::cout << "\n=== DEMO ROBOT PHA CÀ PHÊ ===" << std::endl;
    std::cout << "Điều khiển:" << std::endl;
    std::cout << "  SPACE  - Bắt đầu animation pha cà phê" << std::endl;
    std::cout << "  R      - Reset robot về vị trí ban đầu" << std::endl;
    std::cout << "  ESC    - Thoát chương trình" << std::endl;
    std::cout << "  Chuột  - Kéo chuột trái để xoay camera" << std::endl;
    std::cout << "  MŨI TÊN- Lên/Xuống để zoom in/out" << std::endl;
    std::cout << "\nCác pha hoạt động:" << std::endl;
    std::cout << "  1. Duỗi tay tới cốc" << std::endl;
    std::cout << "  2. Cầm lấy cốc" << std::endl;
    std::cout << "  3. Xoay về máy pha cà phê" << std::endl;
    std::cout << "  4. Rót cà phê" << std::endl;
    std::cout << "  5. Mang cốc về bàn" << std::endl;
    std::cout << "  6. Đặt cốc xuống" << std::endl;
    std::cout << "  7. Thu cánh tay về" << std::endl;
    std::cout << "================================\n" << std::endl;
}

//=============================================================================
// HÀM DỌN DẸP - Được gọi khi thoát chương trình
//=============================================================================
void cleanup() {
    std::cout << "🧹 Dọn dẹp hoàn thành." << std::endl;
}

//=============================================================================
// HÀM MAIN - Điểm bắt đầu của chương trình
//=============================================================================
int main(int argc, char** argv) {
    // Đăng ký hàm cleanup để được gọi khi thoát
    atexit(cleanup);
    
    // Khởi tạo GLUT (OpenGL Utility Toolkit)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Double buffer, RGB, depth
    glutInitWindowSize(1024, 768);                            // Kích thước cửa sổ
    glutInitWindowPosition(100, 100);                         // Vị trí cửa sổ trên màn hình
    
    // Tạo cửa sổ OpenGL
    int window = glutCreateWindow("Demo Robot Pha Cà Phê - Animation 3D OpenGL [CẢI TIẾN]");
    if (window == 0) {
        std::cerr << "❌ Không thể tạo cửa sổ OpenGL!" << std::endl;
        return -1;
    }
    
    // Hiển thị hướng dẫn sử dụng
    printInstructions();
    
    // Khởi tạo OpenGL với xử lý lỗi
    try {
        initGL();
    } catch (const std::exception& e) {
        std::cerr << "❌ Lỗi khởi tạo OpenGL: " << e.what() << std::endl;
        return -1;
    }
    
    // Đăng ký các callback functions
    glutDisplayFunc(display);          // Hàm vẽ
    glutReshapeFunc(reshape);          // Hàm xử lý thay đổi kích thước
    glutKeyboardFunc(keyboard);        // Hàm xử lý bàn phím
    glutSpecialFunc(specialKeys);      // Hàm xử lý phím đặc biệt
    glutMouseFunc(mouse);              // Hàm xử lý chuột
    glutMotionFunc(mouseMotion);       // Hàm xử lý di chuyển chuột
    glutTimerFunc(16, timer, 0);       // Hàm timer cho animation
    
    std::cout << "✅ OpenGL khởi tạo thành công. Bắt đầu vòng lặp chính..." << std::endl;
    
    // Bắt đầu vòng lặp chính của GLUT
    glutMainLoop();
    
    return 0;
}
