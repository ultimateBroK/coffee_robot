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
    robot.cupY = TABLE_Y + 0.0f;  // Đặt trên mặt bàn
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
            // Di chuyển cánh tay về phía cốc - chuyển động mượt mà hơn
            robot.shoulderZ = s * -25.0f;  // Hạ vai xuống với biên độ lớn hơn
            robot.elbow = s * -45.0f;      // Gập khuỷu tay với góc tự nhiên hơn
            robot.wristZ = s * 45.0f;      // Nâng cổ tay lên cao hơn để tiếp cận tốt
            robot.shoulderY = s * 12.0f;   // Xoay vai mượt mà để hướng chính xác về cốc
            robot.fingers = 90.0f - s * 5.0f; // Bắt đầu chuẩn bị gripper từ từ
            
            if (robot.progress >= 1.0f) {
                advancePhase(GRAB_CUP, "📋 Dang nam coc...");
            }
            break;
            
        case GRAB_CUP:
            // Xoay gripper và đóng ngón tay để nắm cốc - chuyển động đồng bộ hơn
            robot.wristY = s * -90.0f;  // Xoay để thẳng hàng với cốc
            
            // Đóng gripper từ từ để nắm chặt cốc - chuyển động mượt mà hơn
            if (s < 0.4f) {
                // 40% đầu: chỉ xoay gripper và chuẩn bị
                robot.fingers = 85.0f - s * 10.0f; // Giảm dần độ mở
            } else {
                // 60% sau: đóng gripper để nắm cốc với tốc độ thích hợp
                float gripProgress = (s - 0.4f) / 0.6f;
                robot.fingers = 81.0f - gripProgress * 55.0f;  // Đóng chặt hơn, mượt hơn
            }
            
            if (s > 0.6f) {
                robot.holdingCup = true;  // Bắt đầu cầm cốc
                // Nhấc cốc lên một chút để rời khỏi bàn - chuyển động mượt mà hơn
                float liftProgress = (s - 0.6f) / 0.4f;
                robot.shoulderZ = -25.0f + liftProgress * 10.0f; // Nhấc vai lên mượt mà
                robot.elbow = -45.0f + liftProgress * 5.0f;      // Điều chỉnh khuỷu tay nhẹ nhàng
            }
            
            if (robot.progress >= 1.0f) {
                advancePhase(TURN_TO_MACHINE, "🔄 Dang xoay ve phia may pha ca phe...");
            }
            break;
            
        case TURN_TO_MACHINE:
            // Xoay thân và định vị cánh tay về phía máy pha cà phê - chuyển động đồng bộ
            robot.body = s * -180.0f;                // Xoay thân 180 độ mượt mà
            robot.shoulderY = 12.0f + s * 18.0f;     // Điều chỉnh vai ngang với biên độ lớn hơn
            robot.shoulderZ = -15.0f + s * 10.0f;    // Nâng vai lên với quỹ đạo mượt mà
            robot.elbow = -40.0f + s * 25.0f;        // Duỗi khuỷu tay tự nhiên hơn
            robot.wristZ = 45.0f + s * 10.0f;        // Nâng cổ tay lên cao đồng bộ với vai
            robot.wristY = -90.0f + s * 5.0f;        // Điều chỉnh nhẹ gripper trong quá trình xoay
            
            if (robot.progress >= 1.0f) {
                advancePhase(POUR_COFFEE, "☕ Dang rot ca phe...");
            }
            break;
            
        case POUR_COFFEE:
            // Giữ cốc ổn định dưới vòi pha cà phê - thêm chuyển động nhỏ để tự nhiên
            robot.wristZ = 55.0f;   // Điều chỉnh cốc từ từ đến vị trí hoàn hảo
            robot.wristY = -85.0f - s * 5.0f;   // Tinh chỉnh gripper để đặt cốc chính xác
            
            if (robot.progress >= 1.5f) {  // Đợi lâu hơn để pha cà phê
                robot.cupHasCoffee = true;
                advancePhase(RETURN_CUP, "↩️ Dang quay ve ban...");
            }
            break;
            
        case RETURN_CUP: {
            // Đảo ngược chuyển động để quay về bàn - chuyển động mượt mà hơn
            float reverse = 1.0f - s;
            robot.body = reverse * -180.0f;              // Xoay thân về vị trí ban đầu
            robot.shoulderY = 12.0f + reverse * 18.0f;   // Điều chỉnh vai ngang mượt mà
            robot.shoulderZ = -15.0f + reverse * 10.0f;  // Hạ vai xuống tự nhiên
            robot.elbow = -40.0f + reverse * 25.0f;      // Gập khuỷu tay lại đồng bộ
            robot.wristZ = 45.0f + reverse * 10.0f;      // Hạ cổ tay xuống mượt mà
            robot.wristY = -90.0f + reverse * 5.0f;      // Điều chỉnh gripper nhẹ nhàng
            
            if (robot.progress >= 1.0f) {
                advancePhase(PLACE_CUP, "📍 Dang dat coc xuong...");
            }
            break;
        }
            
        case PLACE_CUP:
            // Hạ cốc xuống và thả ra - chuyển động đồng bộ và tự nhiên hơn
            robot.shoulderZ = -25.0f - s * 10.0f;    // Hạ vai xuống để đặt cốc mượt mà
            robot.elbow = -40.0f - s * 5.0f;         // Điều chỉnh khuỷu tay nhẹ nhàng
            robot.wristZ = 45.0f - s * 15.0f;        // Hạ cổ tay xuống đồng bộ
            robot.wristY = -90.0f + s * 90.0f;       // Xoay gripper về vị trí ban đầu
            
            if (s > 0.5f) {
                robot.holdingCup = false;  // Thả cốc ra
                // Mở gripper từ từ với chuyển động mượt mà hơn
                float releaseProgress = (s - 0.5f) / 0.5f;
                float easeRelease = smooth(releaseProgress); // Áp dụng hàm smooth cho chuyển động mở
                robot.fingers = 30.0f + easeRelease * 60.0f;  // Mở gripper mượt mà
            }
            
            if (robot.progress >= 1.0f) {
                advancePhase(GO_HOME, "🏠 Dang ve vi tri ban dau...");
            }
            break;
            
        case GO_HOME: {
            // Đưa tất cả khớp về vị trí trung tính - chuyển động mượt mà hơn
            float reverse2 = 1.0f - s;
            float easeReverse = smooth(reverse2); // Áp dụng hàm smooth cho chuyển động về
            
            robot.shoulderY = easeReverse * 12.0f;    // Đưa vai về giữa mượt mà
            robot.shoulderZ = easeReverse * -35.0f;   // Đưa vai về vị trí ban đầu tự nhiên
            robot.elbow = easeReverse * -45.0f;       // Duỗi khuỷu tay mượt mà
            robot.wristZ = easeReverse * 30.0f;       // Đưa cổ tay về vị trí ban đầu
            robot.wristY = easeReverse * -5.0f;       // Gripper thẳng
            robot.fingers = 90.0f - easeReverse * 5.0f; // Gripper mở hoàn toàn
            
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

// Hàm tiện ích để chuyển pha animation, giảm mã lặp
void advancePhase(RobotPhase newPhase, const char* message) {
    robot.phase = newPhase;
    robot.progress = 0.0f;
    printf("%s\n", message);
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
        // Tận dụng lại các phép biến đổi từ drawRobotArm để tối ưu hóa
        // Di chuyển đến vị trí gripper (dùng ít phép biến đổi hơn)
        glTranslatef(0, 1.1f, 0);                    // Nâng lên độ cao robot
        glRotatef(robot.body, 0, 1, 0);              // Xoay theo thân robot
        
        // Đến vị trí vai
        glTranslatef(0, 1, 0);                      
        glRotatef(robot.shoulderY, 0, 1, 0);        
        glRotatef(robot.shoulderZ, 0, 0, 1);         
        
        // Cánh tay trên đến khuỷu tay
        glTranslatef(ARM_UPPER, 0, 0);               
        glRotatef(robot.elbow, 0, 0, 1);             
        
        // Cánh tay dưới đến cổ tay
        glTranslatef(ARM_LOWER, 0, 0);               
        glRotatef(robot.wristZ, 0, 0, 1);            
        glRotatef(robot.wristY, 0, 1, 0);            
        glTranslatef(0.4f, 0, 0);                    // Vị trí trong gripper
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