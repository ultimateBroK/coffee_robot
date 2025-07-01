#include "robot.h"
#include <cstdio>

//=============================================================================
// BIẾN TOÀN CỤC - Robot instance duy nhất
//=============================================================================
Robot robot;

//=============================================================================
// HÀM TIỆN ÍCH - Hỗ trợ animation mượt mà
//=============================================================================
float smooth(float t) {
    // Đường cong animation mượt (ease in/out)
    // Bắt đầu chậm, tăng tốc, rồi chậm lại ở cuối
    return t * t * (3.0f - 2.0f * t);
}

//=============================================================================
// KHỞI TẠO ROBOT - Đặt về vị trí ban đầu
//=============================================================================
void initRobot() {
    // Đặt lại tất cả góc khớp về vị trí trung tính
    robot.shoulderY = robot.shoulderZ = 0.0f;
    robot.elbow = 0.0f;
    robot.wristZ = robot.wristY = 0.0f;
    robot.fingers = 90.0f;  // Bắt đầu với gripper mở
    robot.body = 0.0f;
    
    // Đặt lại trạng thái animation
    robot.phase = IDLE;
    robot.progress = 0.0f;
    robot.moving = false;
    
    // Cốc bắt đầu trên bàn (vị trí gần robot hơn để dễ nắm)
    robot.cupX = 2.2f;  // Gần robot hơn để dễ với tới
    robot.cupY = TABLE_Y + 0.25f;  // Đặt trên mặt bàn
    robot.cupZ = 0.0f;
    robot.holdingCup = false;
    robot.cupHasCoffee = false;
}

//=============================================================================
// ĐIỀU KHIỂN ANIMATION - Bắt đầu và reset robot
//=============================================================================
void startCoffeeSequence() {
    if (robot.moving) return;  // Không bắt đầu nếu đang chuyển động
    
    robot.moving = true;
    robot.phase = REACH_CUP;
    robot.progress = 0.0f;
    printf("🤖 Bat dau quy trinh pha ca phe...\n");
}

void resetRobot() {
    initRobot();
    printf("🔄 Robot da duoc reset ve vi tri ban dau\n");
}

//=============================================================================
// CẬP NHẬT ANIMATION - Được gọi mỗi frame để cập nhật chuyển động robot
//=============================================================================
void updateRobot() {
    if (!robot.moving) return;  // Không cập nhật nếu robot không chuyển động
    
    // Tăng tiến độ animation
    robot.progress += ANIMATION_SPEED;
    float t = robot.progress;
    if (t > 1.0f) t = 1.0f;
    float s = smooth(t);  // Phiên bản mượt của t
    
    // Cập nhật robot dựa trên giai đoạn hiện tại
    switch (robot.phase) {
        case REACH_CUP:
            // Di chuyển cánh tay về phía cốc
            robot.shoulderZ = s * -30.0f;  // Hạ vai xuống nhiều hơn
            robot.elbow = s * -50.0f;      // Gập khuỷu tay nhiều hơn
            robot.wristZ = s * 40.0f;      // Nâng cổ tay lên
            robot.shoulderY = s * 10.0f;   // Xoay vai một chút để hướng về cốc
            robot.fingers = 90.0f;         // Giữ gripper mở
            
            if (robot.progress >= 1.0f) {
                robot.phase = GRAB_CUP;
                robot.progress = 0.0f;
                printf("📋 Dang nam coc...\n");
            }
            break;
            
        case GRAB_CUP:
            // Xoay gripper và đóng ngón tay để nắm cốc
            robot.wristY = s * -90.0f;  // Xoay để thẳng hàng với cốc
            
            // Đóng gripper từ từ để nắm chặt cốc
            if (s < 0.5f) {
                // 50% đầu: chỉ xoay gripper
                robot.fingers = 90.0f;
            } else {
                // 50% sau: đóng gripper để nắm cốc
                float gripProgress = (s - 0.5f) / 0.5f;
                robot.fingers = 90.0f - gripProgress * 60.0f;  // Đóng chặt hơn
            }
            
            if (s > 0.7f) {
                robot.holdingCup = true;  // Bắt đầu cầm cốc
                // Nhấc cốc lên một chút để rời khỏi bàn
                robot.shoulderZ = -30.0f + (s - 0.7f) / 0.3f * 8.0f;
            }
            
            if (robot.progress >= 1.0f) {
                robot.phase = TURN_TO_MACHINE;
                robot.progress = 0.0f;
                printf("🔄 Dang xoay ve phia may pha ca phe...\n");
            }
            break;
            
        case TURN_TO_MACHINE:
            // Xoay thân và định vị cánh tay về phía máy pha cà phê
            robot.body = s * -180.0f;           // Xoay thân 180 độ
            robot.shoulderY = 10.0f + s * 15.0f; // Điều chỉnh vai ngang
            robot.shoulderZ = -22.0f + s * 17.0f; // Nâng vai lên
            robot.elbow = -50.0f + s * 30.0f;    // Duỗi khuỷu tay
            robot.wristZ = 40.0f + s * 50.0f;    // Nâng cổ tay lên cao
            robot.wristY = -90.0f;               // Giữ gripper thẳng
            
            if (robot.progress >= 1.0f) {
                robot.phase = POUR_COFFEE;
                robot.progress = 0.0f;
                printf("☕ Dang rot ca phe...\n");
            }
            break;
            
        case POUR_COFFEE:
            // Giữ cốc ổn định dưới vòi pha cà phê
            robot.wristZ = 90.0f;   // Cốc thẳng đứng
            robot.wristY = -90.0f;  // Gripper hướng đúng
            
            if (robot.progress >= 1.5f) {  // Đợi lâu hơn để pha cà phê
                robot.cupHasCoffee = true;
                robot.phase = RETURN_CUP;
                robot.progress = 0.0f;
                printf("↩️ Dang quay ve ban...\n");
            }
            break;
            
        case RETURN_CUP: {
            // Đảo ngược chuyển động để quay về bàn
            float reverse = 1.0f - s;
            robot.body = reverse * -180.0f;           // Xoay thân về vị trí ban đầu
            robot.shoulderY = 10.0f + reverse * 15.0f; // Điều chỉnh vai ngang
            robot.shoulderZ = -22.0f + reverse * 17.0f; // Hạ vai xuống
            robot.elbow = -50.0f + reverse * 30.0f;    // Gập khuỷu tay lại
            robot.wristZ = 40.0f + reverse * 50.0f;    // Hạ cổ tay xuống
            robot.wristY = -90.0f;                     // Giữ gripper thẳng
            
            if (robot.progress >= 1.0f) {
                robot.phase = PLACE_CUP;
                robot.progress = 0.0f;
                printf("📍 Dang dat coc xuong...\n");
            }
            break;
        }
            
        case PLACE_CUP:
            // Hạ cốc xuống và thả ra
            robot.shoulderZ = -30.0f - s * 8.0f;  // Hạ vai xuống để đặt cốc
            robot.wristZ = 40.0f - s * 10.0f;     // Hạ cổ tay xuống
            robot.wristY = -90.0f + s * 90.0f;    // Xoay gripper về vị trí ban đầu
            
            if (s > 0.6f) {
                robot.holdingCup = false;  // Thả cốc ra
                // Mở gripper từ từ
                float releaseProgress = (s - 0.6f) / 0.4f;
                robot.fingers = 30.0f + releaseProgress * 60.0f;  // Mở gripper
            }
            
            if (robot.progress >= 1.0f) {
                robot.phase = GO_HOME;
                robot.progress = 0.0f;
                printf("🏠 Dang ve vi tri ban dau...\n");
            }
            break;
            
        case GO_HOME: {
            // Đưa tất cả khớp về vị trí trung tính
            float reverse2 = 1.0f - s;
            robot.shoulderY = reverse2 * 10.0f;   // Đưa vai về giữa
            robot.shoulderZ = reverse2 * -38.0f;  // Đưa vai về vị trí ban đầu
            robot.elbow = reverse2 * -50.0f;      // Duỗi khuỷu tay
            robot.wristZ = reverse2 * 30.0f;      // Đưa cổ tay về vị trí ban đầu
            robot.wristY = 0.0f;                  // Gripper thẳng
            robot.fingers = 90.0f;                // Gripper mở
            
            if (robot.progress >= 1.0f) {
                robot.phase = FINISHED;
                robot.moving = false;
                printf("✅ Ca phe da san sang! Nhan R de reset, SPACE de pha them.\n");
            }
            break;
        }
            
        case IDLE:
        case FINISHED:
            // Không làm gì - chờ lệnh từ người dùng
            break;
            
        default:
            break;
    }
}

//=============================================================================
// HÀM VẼ CƠ BẢN - Đơn giản và sạch sẽ
//=============================================================================

void drawSimpleBox(float w, float h, float d, float r, float g, float b) {
    // Vẽ hình hộp với kích thước và màu sắc cho trước
    glColor3f(r, g, b);
    glPushMatrix();
    glScalef(w, h, d);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawSimpleCylinder(float radius, float height, float r, float g, float b) {
    // Vẽ hình trụ với bán kính, chiều cao và màu sắc cho trước
    glColor3f(r, g, b);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, radius, radius, height, 20, 1);
    gluDeleteQuadric(quad);
}

//=============================================================================
// VẼ CÁC THÀNH PHẦN ROBOT - Chia nhỏ thành từng phần dễ hiểu
//=============================================================================

void drawRobotBase() {
    // Đế robot - phần cố định
    glPushMatrix();
    glTranslatef(0, -1, 0);
    drawSimpleBox(1.8f, 0.2f, 1.8f, 0.3f, 0.3f, 0.4f);  // Đế màu xám đậm
    glPopMatrix();
    
    // Cột trung tâm - nối đế với thân robot
    glPushMatrix();
    drawSimpleCylinder(0.5f, 1.0f, 0.5f, 0.5f, 0.6f);  // Cột màu xám xanh
    glPopMatrix();
}

void drawRobotArm() {
    glPushMatrix();
    
    // Di chuyển đến vị trí vai
    glTranslatef(0, 1, 0);
    glRotatef(robot.shoulderY, 0, 1, 0);  // Xoay vai ngang
    glRotatef(robot.shoulderZ, 0, 0, 1);  // Xoay vai dọc
    
    // Khớp vai - hình cầu
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.4f, 15, 15);
    
    // Cánh tay trên
    glTranslatef(ARM_UPPER/2, 0, 0);
    drawSimpleBox(ARM_UPPER, 0.5f, 0.5f, 0.6f, 0.6f, 0.7f);  // Màu xám xanh
    
    // Khớp khuỷu tay - hình cầu
    glTranslatef(ARM_UPPER/2, 0, 0);
    glRotatef(robot.elbow, 0, 0, 1);  // Gập khuỷu tay
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.3f, 12, 12);
    
    // Cánh tay dưới
    glTranslatef(ARM_LOWER/2, 0, 0);
    drawSimpleBox(ARM_LOWER, 0.4f, 0.4f, 0.6f, 0.6f, 0.7f);  // Màu xám xanh
    
    // Khớp cổ tay - hình cầu
    glTranslatef(ARM_LOWER/2, 0, 0);
    glRotatef(robot.wristZ, 0, 0, 1);  // Gập cổ tay lên/xuống
    glRotatef(robot.wristY, 0, 1, 0);  // Xoay cổ tay trái/phải
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.2f, 10, 10);
    
    // Gripper - tay kẹp
    glTranslatef(0.2f, 0, 0);
    
    // Thân gripper
    drawSimpleBox(0.3f, 0.4f, 0.3f, 0.4f, 0.4f, 0.5f);  // Màu xám đậm
    
    // Ngón tay trên
    glPushMatrix();
    glTranslatef(0.15f, 0.15f, 0);
    glRotatef(-robot.fingers, 0, 0, 1);  // Đóng/mở ngón trên
    glTranslatef(0.15f, 0, 0);
    drawSimpleBox(0.3f, 0.08f, 0.2f, 0.5f, 0.5f, 0.6f);  // Ngón màu xám nhạt
    glPopMatrix();
    
    // Ngón tay dưới
    glPushMatrix();
    glTranslatef(0.15f, -0.15f, 0);
    glRotatef(robot.fingers, 0, 0, 1);   // Đóng/mở ngón dưới
    glTranslatef(0.15f, 0, 0);
    drawSimpleBox(0.3f, 0.08f, 0.2f, 0.5f, 0.5f, 0.6f);  // Ngón màu xám nhạt
    glPopMatrix();
    
    glPopMatrix();
}

//=============================================================================
// VẼ ROBOT HOÀN CHỈNH - Kết hợp tất cả các thành phần
//=============================================================================

void drawRobot() {
    glPushMatrix();
    glTranslatef(0, 1.1f, 0);  // Nâng robot lên trên mặt bàn
    
    drawRobotBase();  // Vẽ đế robot
    
    // Thân trên có thể xoay
    glPushMatrix();
    glRotatef(robot.body, 0, 1, 0);  // Xoay thân robot
    
    // Thân robot
    glPushMatrix();
    glTranslatef(0, 0.7f, 0);
    drawSimpleBox(1.2f, 1.4f, 1.2f, 0.8f, 0.8f, 0.9f);  // Thân màu xám nhạt
    glPopMatrix();
    
    drawRobotArm();  // Vẽ cánh tay robot
    
    glPopMatrix();
    glPopMatrix();
}

//=============================================================================
// VẼ MÔI TRƯỜNG - Bàn, máy pha cà phê và cốc
//=============================================================================

void drawTable() {
    // Vẽ mặt bàn với màu nâu đậm, tối để làm nổi bật cánh tay robot
    glPushMatrix();
    drawSimpleBox(10.0f, 0.2f, 8.0f, 0.25f, 0.15f, 0.10f);  // Màu nâu đậm, tối hơn để nhìn rõ robot
    glPopMatrix();
}

void drawCoffeeMachine() {
    // Vẽ máy pha cà phê ở bên trái bàn
    glPushMatrix();
    glTranslatef(-4, TABLE_Y + 1.1f, 0);
    
    // Thân máy pha cà phê
    drawSimpleBox(1.5f, 2.2f, 1.4f, 0.6f, 0.6f, 0.65f);  // Màu xám kim loại
    
    // Vòi pha cà phê
    glPushMatrix();
    glTranslatef(0, -0.2f, 0.8f);
    drawSimpleCylinder(0.1f, 0.5f, 0.3f, 0.3f, 0.3f);  // Vòi màu đen
    glPopMatrix();
    
    glPopMatrix();
}

void drawCup() {
    glPushMatrix();
    
    if (robot.holdingCup) {
        // Cốc theo chính xác vị trí gripper
        glTranslatef(0, 1.1f, 0);                    // Nâng lên độ cao robot
        glRotatef(robot.body, 0, 1, 0);              // Xoay theo thân robot
        glTranslatef(0, 1, 0);                       // Di chuyển đến vai
        glRotatef(robot.shoulderY, 0, 1, 0);         // Xoay vai ngang
        glRotatef(robot.shoulderZ, 0, 0, 1);         // Xoay vai dọc
        glTranslatef(ARM_UPPER, 0, 0);               // Di chuyển đến khuỷu tay
        glRotatef(robot.elbow, 0, 0, 1);             // Gập khuỷu tay
        glTranslatef(ARM_LOWER, 0, 0);               // Di chuyển đến cổ tay
        glRotatef(robot.wristZ, 0, 0, 1);            // Gập cổ tay
        glRotatef(robot.wristY, 0, 1, 0);            // Xoay cổ tay
        glTranslatef(0.4f, 0, 0);                    // Vị trí trong gripper (gần hơn)
    } else {
        // Cốc đặt trên bàn
        glTranslatef(robot.cupX, robot.cupY, robot.cupZ);
    }
    
    // Vẽ cốc cà phê
    glColor3f(0.95f, 0.95f, 0.95f);  // Màu trắng kem, không quá chói
    glRotatef(-90, 1, 0, 0);         // Xoay để cốc đứng thẳng
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, CUP_SIZE, CUP_SIZE, 0.5f, 20, 1);  // Thành cốc
    gluDisk(quad, 0, CUP_SIZE, 20, 1);                   // Đáy cốc
    
    // Vẽ cà phê nếu có
    if (robot.cupHasCoffee) {
        glColor3f(0.4f, 0.2f, 0.1f);  // Màu nâu cà phê
        glTranslatef(0, 0, 0.4f);      // Nâng lên gần miệng cốc
        gluDisk(quad, 0, CUP_SIZE * 0.9f, 20, 1);  // Mặt cà phê
    }
    
    gluDeleteQuadric(quad);
    glPopMatrix();
}

//=============================================================================
// VẼ TOÀN BỘ SCENE - Kết hợp tất cả các đối tượng
//=============================================================================

void drawScene() {
    // Vẽ toàn bộ scene: bàn, máy pha cà phê, và cốc
    drawTable();         // Mặt bàn
    drawCoffeeMachine(); // Máy pha cà phê
    drawCup();           // Cốc cà phê
}