#include "robot.h"
#include <cstdio>

// Biến toàn cục
RobotState robot;

// Khởi tạo robot
void initRobot() {
    robot.shoulderRotateY = 0.0f;
    robot.shoulderRotateZ = 0.0f;
    robot.elbowAngle = 0.0f;
    robot.wristAngle = 0.0f;
    robot.fingerAngle = 0.0f;
    robot.bodyRotateY = 0.0f;
    
    robot.currentPhase = PHASE_IDLE;
    robot.animationProgress = 0.0f;
    robot.isAnimating = false;
    
    // Vị trí cốc - trên mặt bàn, gần robot hơn để có thể với tới
    robot.cupX = 2.5f;    // Giảm khoảng cách với robot
    robot.cupY = 0.1f;    // Đáy cốc ngay trên mặt bàn
    robot.cupZ = 0.0f;
    robot.cupInHand = false;
}

// Bắt đầu animation
void startAnimation() {
    if (robot.isAnimating) return; // Tránh bắt đầu lại khi đang chạy
    robot.isAnimating = true;
    robot.currentPhase = PHASE_REACH_CUP;
    robot.animationProgress = 0.0f;
    printf("\n=== BẮT ĐẦU QUY TRÌNH PHA CAFE ===\n");
    printf("Giai đoạn 1: Vươn tay tới lấy cốc.\n");
}

// Reset robot về trạng thái ban đầu
void resetRobot() {
    initRobot();
    printf("\nRobot đã được reset về vị trí ban đầu. Nhấn SPACE để bắt đầu lại.\n");
}

// Cập nhật animation
void updateAnimation() {
    if (!robot.isAnimating) return;
    
    const float ANIMATION_SPEED = 0.015f; // Chậm lại để chuyển động mượt hơn
    robot.animationProgress += ANIMATION_SPEED;
    
    float t = robot.animationProgress;
    if (t > 1.0f) t = 1.0f;
    // Sử dụng hàm smoothstep để chuyển động mượt hơn
    float smoothT = t * t * (3.0f - 2.0f * t);
    
    switch (robot.currentPhase) {
        case PHASE_REACH_CUP: {
            // Tay vươn ra lấy cốc, đảm bảo không đi xuyên bàn
            robot.shoulderRotateZ = smoothT * -30.0f;
            robot.elbowAngle = smoothT * -30.0f;
            robot.wristAngle = smoothT * 60.0f;
            robot.fingerAngle = 90.0f; // Mở tay
            
            if (robot.animationProgress >= 1.0f) {
                robot.animationProgress = 0.0f;
                robot.currentPhase = PHASE_GRAB_CUP;
                printf("Giai đoạn 2: Kẹp và nhấc cốc.\n");
            }
            break;
        }
        case PHASE_GRAB_CUP: {
            // Kẹp và nhấc cốc
            robot.fingerAngle = 90.0f - smoothT * 50.0f; // Kẹp lại
            
            // Nhấc lên một chút
            robot.shoulderRotateZ = -30.0f + smoothT * 5.0f; // (-30 -> -25)
            robot.elbowAngle = -30.0f + smoothT * 5.0f;      // (-30 -> -25)
            robot.wristAngle = 60.0f; // Giữ nguyên góc cổ tay

            if (smoothT > 0.5f && !robot.cupInHand) {
                robot.cupInHand = true;
            }
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_TURN_TO_MACHINE;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 3: Xoay người về phía máy pha cà phê.\n");
            }
            break;
        }
        case PHASE_TURN_TO_MACHINE: {
            // Xoay người và tay về phía máy pha cafe
            robot.bodyRotateY = smoothT * -90.0f;
            
            robot.shoulderRotateY = smoothT * -45.0f;
            robot.shoulderRotateZ = -25.0f + smoothT * 15.0f; // (-25 -> -10)
            robot.elbowAngle = -25.0f - smoothT * 75.0f;      // (-25 -> -100)
            robot.wristAngle = 60.0f - smoothT * 105.0f;      // (60 -> -45)

            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_POUR_COFFEE;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 4: Đặt cốc vào vòi và rót cà phê.\n");
            }
            break;
        }
        case PHASE_POUR_COFFEE: {
            // Giữ cốc dưới vòi, nghiêng để rót
            float pourT = sin(robot.animationProgress * 3.14159f); // 0 -> 1 -> 0
            robot.wristAngle = -45.0f + pourT * -30.0f; // Nghiêng cổ tay

            if (robot.animationProgress >= 1.5f) { // Giữ lâu hơn
                robot.animationProgress = 0.0f;
                robot.currentPhase = PHASE_RETURN_CUP;
                printf("Giai đoạn 5: Quay trở lại vị trí ban đầu.\n");
            }
            break;
        }
        case PHASE_RETURN_CUP: {
            // Đảo ngược chuyển động xoay người và tay
            float returnT = 1.0f - smoothT;
            robot.bodyRotateY = returnT * -90.0f;
            
            robot.shoulderRotateY = returnT * -45.0f;
            robot.shoulderRotateZ = -25.0f + returnT * 15.0f;
            robot.elbowAngle = -25.0f - returnT * 75.0f;
            robot.wristAngle = 60.0f - returnT * 105.0f;

            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_PLACE_CUP;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 6: Đặt cốc xuống bàn.\n");
            }
            break;
        }
        case PHASE_PLACE_CUP: {
            // Hạ cốc xuống bàn
            float returnT = 1.0f - smoothT;
            robot.shoulderRotateZ = -30.0f + returnT * 5.0f; // (-25 -> -30)
            robot.elbowAngle = -30.0f + returnT * 5.0f;      // (-25 -> -30)
            robot.wristAngle = 60.0f;
            
            if (smoothT > 0.5f) {
                if (robot.cupInHand) {
                    robot.cupInHand = false;
                    robot.cupX = 2.5f; // Cập nhật vị trí đặt cốc
                    robot.cupY = 0.1f;
                    robot.cupZ = 0.0f;
                }
                robot.fingerAngle = 40.0f + (smoothT - 0.5f) * 2.0f * 50.0f; // Mở tay
            }

            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_RETRACT;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 7: Rút tay về.\n");
            }
            break;
        }
        case PHASE_RETRACT: {
            // Rút tay về vị trí ban đầu
            float returnT = 1.0f - smoothT;
            robot.shoulderRotateZ = returnT * -30.0f;
            robot.elbowAngle = returnT * -30.0f;
            robot.wristAngle = returnT * 60.0f;
            robot.fingerAngle = 90.0f;

            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_COMPLETE;
                robot.isAnimating = false;
                printf("=== HOÀN THÀNH! Cà phê của bạn đã sẵn sàng. ===\n");
            }
            break;
        }
        default:
            break;
    }
}

// Vẽ thân robot chi tiết
void drawRobotBody() {
    // Đế robot (cố định)
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(1.8f, 0.2f, 1.8f);
    glColor3f(0.3f, 0.3f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Trụ xoay
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.6f);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, 0.5f, 0.5f, 1.0f, 20, 1);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Thân trên (sẽ được vẽ trong hàm drawRobot để xoay)
}

// Vẽ cánh tay robot chi tiết
void drawRobotArm() {
    glPushMatrix();
    // Di chuyển đến khớp vai, gắn trên thân trên
    glTranslatef(0.0f, 1.0f, 0.0f); 
    glRotatef(robot.shoulderRotateY, 0, 1, 0);
    glRotatef(robot.shoulderRotateZ, 0, 0, 1);
    
    // Khớp vai (hình cầu)
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.4f, 15, 15);
    
    // Cánh tay trên (upper arm)
    glTranslatef(0.8f, 0.0f, 0.0f); 
    glPushMatrix();
    glScalef(1.6f, 0.5f, 0.5f);
    glColor3f(0.6f, 0.6f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Khớp khuỷu tay
    glTranslatef(0.8f, 0.0f, 0.0f); 
    glRotatef(robot.elbowAngle, 0, 0, 1);
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.3f, 12, 12);
    
    // Cánh tay dưới (forearm)
    glTranslatef(0.7f, 0.0f, 0.0f); 
    glPushMatrix();
    glScalef(1.4f, 0.4f, 0.4f);
    glColor3f(0.6f, 0.6f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Cổ tay
    glTranslatef(0.7f, 0.0f, 0.0f); 
    glRotatef(robot.wristAngle, 0, 0, 1);
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.2f, 10, 10);

    // Bàn tay (gripper)
    glTranslatef(0.2f, 0.0f, 0.0f); 
    drawRobotHand();
    
    glPopMatrix();
}

// Vẽ bàn tay robot (gripper) chi tiết
void drawRobotHand() {
    // Lòng bàn tay
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.5f);
    glScalef(0.2f, 0.3f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Ngón tay 1 (trên)
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f); 
    glRotatef(-robot.fingerAngle, 0, 0, 1); 
    glTranslatef(0.2f, 0.0f, 0.0f); 
    
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.6f);
    glScalef(0.4f, 0.1f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();
    
    // Ngón tay 2 (dưới)
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);
    glRotatef(robot.fingerAngle, 0, 0, 1);
    glTranslatef(0.2f, 0.0f, 0.0f);

    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.6f);
    glScalef(0.4f, 0.1f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();
}

// Vẽ bàn
void drawTable() {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(0.8f, 0.6f, 0.4f);
    glScalef(10.0f, 0.2f, 8.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Vẽ máy pha cà phê chi tiết
void drawCoffeeMachine() {
    glPushMatrix();
    // Đặt máy trên mặt bàn (Y=0.1) và dịch ra xa
    // Thân máy cao 2.2, nên tâm Y của nó phải là 0.1 + 2.2/2 = 1.2
    glTranslatef(-4.0f, 1.2f, 0.0f);

    // Thân máy chính (vỏ kim loại)
    glColor3f(0.6f, 0.6f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f); // Dịch lên để đáy chạm mặt bàn (cao 2.2/2 = 1.1)
    glScalef(1.5f, 2.2f, 1.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Phần đầu máy (group head)
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.7f); 
    glScalef(0.7f, 0.5f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Vòi rót (từ group head)
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.8f); // Dịch vòi xuống thấp hơn
    GLUquadric* quad = gluNewQuadric();
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad, 0.1f, 0.05f, 0.5f, 10, 1);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Tay cầm pha chế (portafilter handle)
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 1.4f); // Hạ tay cầm xuống
    glScalef(0.15f, 0.15f, 1.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Các nút bấm
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(-0.4f, 1.8f, 0.75f);
    glutSolidSphere(0.1f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.4f, 1.8f, 0.75f);
    glutSolidSphere(0.1f, 8, 8);
    glPopMatrix();

    // Đồng hồ áp suất
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.75f);
    // Mặt đồng hồ
    glColor3f(1.0f, 1.0f, 1.0f);
    GLUquadric* diskQuad = gluNewQuadric();
    gluDisk(diskQuad, 0.0f, 0.25f, 20, 1);
    // Viền đồng hồ
    glColor3f(0.3f, 0.3f, 0.3f);
    gluCylinder(diskQuad, 0.25f, 0.25f, 0.05f, 20, 1);
    // Kim đồng hồ
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.06f);
    glVertex3f(0.15f, 0.1f, 0.06f);
    glEnd();
    gluDeleteQuadric(diskQuad);
    glPopMatrix();

    glPopMatrix(); 
}

// Vẽ cốc chi tiết
void drawCup() {
    glPushMatrix();
    
    if (robot.cupInHand) {
        // Tính toán vị trí cốc dựa trên chuỗi transform của cánh tay robot
        // để đảm bảo cốc luôn nằm chính xác trong tay gắp.
        glTranslatef(0.0f, 1.1f, 0.0f);
        glRotatef(robot.bodyRotateY, 0, 1, 0);
        
        glTranslatef(0.0f, 1.0f, 0.0f);
        glRotatef(robot.shoulderRotateY, 0, 1, 0);
        glRotatef(robot.shoulderRotateZ, 0, 0, 1);
        
        glTranslatef(1.6f, 0.0f, 0.0f); // Chiều dài cánh tay trên
        glRotatef(robot.elbowAngle, 0, 0, 1);
        
        glTranslatef(1.4f, 0.0f, 0.0f); // Chiều dài cánh tay dưới
        glRotatef(robot.wristAngle, 0, 0, 1);
        
        // Vị trí cốc trong kẹp gắp (so với cổ tay)
        glTranslatef(0.4f, 0.0f, 0.0f); // Dịch ra giữa các ngón tay

    } else {
        // Cốc trên bàn
        glTranslatef(robot.cupX, robot.cupY, robot.cupZ);
    }
    
    // Vẽ thân cốc (trụ)
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.25f, 0.25f, 0.5f, 20, 1); 
    
    // Vẽ đáy cốc
    gluDisk(quad, 0.0f, 0.25f, 20, 1); 

    // Vẽ tay cầm
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, 0.25f); 
    glutSolidTorus(0.04f, 0.15f, 8, 16);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
    
    glPopMatrix();
}

// Vẽ toàn bộ robot
void drawRobot() {
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);  // Đặt robot trên mặt đất (Y=0.1 là mặt bàn)
    
    // Phần đế và thân dưới (cố định)
    drawRobotBody();

    // Phần thân trên và cánh tay (xoay được)
    glPushMatrix();
    glRotatef(robot.bodyRotateY, 0, 1, 0);
    
    // Thân trên
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glScalef(1.2f, 1.4f, 1.2f);
    glColor3f(0.8f, 0.8f, 0.9f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Cánh tay robot
    drawRobotArm();

    glPopMatrix(); // Kết thúc phần xoay
    
    glPopMatrix(); // Kết thúc robot
}

// Vẽ môi trường
void drawEnvironment() {
    // Bàn
    drawTable();
    
    // Máy pha cà phê
    drawCoffeeMachine();
    
    // Hiệu ứng pha cà phê
    if (robot.currentPhase == PHASE_POUR_COFFEE) {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Giọt cà phê từ vòi của máy mới
        for (int i = 0; i < 8; i++) {
            float x_offset = sin(robot.animationProgress * 20 + i) * 0.03f;
            float z_offset = cos(robot.animationProgress * 15 + i) * 0.03f;
            float drop_progress = fmod(robot.animationProgress * 1.5f, 1.0f);
            
            // Vị trí bắt đầu từ vòi máy pha cafe
            float startY = 0.9f; // Vị trí Y của vòi
            float y = startY - (drop_progress * 0.5f + i * 0.05f);
            
            // Vị trí cốc khi đang pha
            float cup_y_target = 0.8f; 

            if (y > cup_y_target) { 
                glPushMatrix();
                // Tọa độ X, Z của máy pha cafe
                glTranslatef(-4.0f + x_offset, y, 0.8f + z_offset);
                glColor4f(0.4f, 0.2f, 0.1f, 0.8f - i * 0.1f);
                glutSolidSphere(0.015f, 4, 4);
                glPopMatrix();
            }
        }
        
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
    
    // Cốc
    drawCup();
}
