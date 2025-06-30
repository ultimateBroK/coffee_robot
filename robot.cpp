#include "robot.h"
#include <cstdio>

using namespace RobotConstants;

// Biến toàn cục
RobotState robot;

// Utility functions for smoother animation
float smoothStep(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

float easeInOut(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
}

// Resource Manager Implementation
GLResourceManager::GLResourceManager() 
    : cylinderQuad(gluNewQuadric(), gluDeleteQuadric)
    , diskQuad(gluNewQuadric(), gluDeleteQuadric) {
}

GLResourceManager::~GLResourceManager() {
    // Unique_ptr will automatically clean up
}

GLResourceManager& GLResourceManager::getInstance() {
    static GLResourceManager instance;
    return instance;
}

GLUquadric* GLResourceManager::getCylinderQuadric() {
    return cylinderQuad.get();
}

GLUquadric* GLResourceManager::getDiskQuadric() {
    return diskQuad.get();
}

void GLResourceManager::cleanup() {
    // Force cleanup if needed
    cylinderQuad.reset();
    diskQuad.reset();
}

// Initialize robot to rest position
void initRobot() {
    // Reset all joint angles to neutral position
    robot.shoulderRotateY = 0.0f;
    robot.shoulderRotateZ = 0.0f;
    robot.elbowAngle = 0.0f;
    robot.wristRotateZ = 0.0f;
    robot.wristRotateY = 0.0f;  // New wrist rotation for proper grip
    robot.fingerAngle = 90.0f;  // Start with gripper open
    robot.bodyRotateY = 0.0f;
    
    // Animation state
    robot.currentPhase = PHASE_IDLE;
    robot.animationProgress = 0.0f;
    robot.isAnimating = false;
    
    // Cup position on table
    robot.cupX = 2.8f;
    robot.cupY = TABLE_HEIGHT;
    robot.cupZ = 0.0f;
    robot.cupInHand = false;
    robot.isCupFull = false;
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

// SIMPLIFIED ANIMATION PHASES WITH PROPER GRIPPER ALIGNMENT

void updatePhaseReachCup(float t) {
    float smooth = easeInOut(t);
    
    // Reach toward cup with natural arm movement
    robot.shoulderRotateZ = smooth * -25.0f;  // Lower arm down
    robot.elbowAngle = smooth * -45.0f;       // Bend elbow to extend reach
    robot.wristRotateZ = smooth * 30.0f;      // Angle wrist down toward cup
    robot.wristRotateY = 0.0f;                // Keep wrist straight for now
    robot.fingerAngle = 90.0f;                // Keep gripper open
}

void updatePhaseGrabCup(float t) {
    float smooth = smoothStep(t);
    
    // Position gripper around cup and close
    robot.wristRotateY = smooth * -90.0f;     // Rotate wrist to align gripper with cup
    robot.fingerAngle = 90.0f - smooth * 50.0f; // Close gripper (90° -> 40°)
    
    // Slight lift after gripping
    if (smooth > 0.7f) {
        float liftT = (smooth - 0.7f) / 0.3f;
        robot.shoulderRotateZ = -25.0f + liftT * 5.0f;
        robot.cupInHand = true;
    }
}

void updatePhaseTurnToMachine(float t) {
    float smooth = easeInOut(t);
    
    // Smooth rotation to coffee machine
    robot.bodyRotateY = smooth * -180.0f;     // Turn body toward machine
    robot.shoulderRotateY = smooth * 20.0f;   // Adjust shoulder for reach
    robot.shoulderRotateZ = -20.0f + smooth * 15.0f; // Lift arm up
    robot.elbowAngle = -45.0f + smooth * 25.0f;      // Extend arm toward machine
    robot.wristRotateZ = 30.0f + smooth * 60.0f;     // Orient cup upright
    robot.wristRotateY = -90.0f;              // Keep gripper aligned
}

void updatePhasePourCoffee(float t) {
    // Hold steady position under coffee spout
    robot.wristRotateZ = 90.0f;   // Cup perfectly upright
    robot.wristRotateY = -90.0f;  // Gripper properly aligned
    
    if (t >= 1.5f) {
        robot.isCupFull = true;
    }
}

void updatePhaseReturnCup(float t) {
    float smooth = easeInOut(t);
    float reverse = 1.0f - smooth;
    
    // Reverse the turn to machine movement
    robot.bodyRotateY = reverse * -180.0f;
    robot.shoulderRotateY = reverse * 20.0f;
    robot.shoulderRotateZ = -20.0f + reverse * 15.0f;
    robot.elbowAngle = -45.0f + reverse * 25.0f;
    robot.wristRotateZ = 30.0f + reverse * 60.0f;
    robot.wristRotateY = -90.0f;
}

void updatePhasePlaceCup(float t) {
    float smooth = smoothStep(t);
    
    // Lower cup back to table
    robot.shoulderRotateZ = -25.0f - smooth * 5.0f;  // Lower arm
    robot.wristRotateZ = 30.0f;                      // Angle down to table
    robot.wristRotateY = -90.0f + smooth * 90.0f;    // Rotate gripper back to normal
    
    // Release cup and open gripper
    if (smooth > 0.6f) {
        robot.cupInHand = false;
        robot.cupX = 2.8f;
        robot.cupY = TABLE_HEIGHT;
        robot.cupZ = 0.0f;
        
        float openT = (smooth - 0.6f) / 0.4f;
        robot.fingerAngle = 40.0f + openT * 50.0f;  // Open gripper
    }
}

void updatePhaseRetract(float t) {
    float smooth = easeInOut(t);
    float reverse = 1.0f - smooth;
    
    // Return all joints to neutral position
    robot.shoulderRotateZ = reverse * -30.0f;
    robot.elbowAngle = reverse * -45.0f;
    robot.wristRotateZ = reverse * 30.0f;
    robot.wristRotateY = 0.0f;
    robot.fingerAngle = 90.0f;  // Fully open
}

// Improved animation update function
void updateAnimation() {
    if (!robot.isAnimating) return;
    
    robot.animationProgress += ANIMATION_SPEED;
    
    float t = robot.animationProgress;
    if (t > 1.0f) t = 1.0f;
    
    switch (robot.currentPhase) {
        case PHASE_REACH_CUP:
            updatePhaseReachCup(t);
            if (robot.animationProgress >= 1.0f) {
                robot.animationProgress = 0.0f;
                robot.currentPhase = PHASE_GRAB_CUP;
                printf("Giai đoạn 2: Kẹp và nhấc cốc.\n");
            }
            break;
            
        case PHASE_GRAB_CUP:
            updatePhaseGrabCup(t);
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_TURN_TO_MACHINE;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 3: Xoay người về phía máy pha cà phê.\n");
            }
            break;
            
        case PHASE_TURN_TO_MACHINE:
            updatePhaseTurnToMachine(t);
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_POUR_COFFEE;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 4: Đặt cốc vào vòi và rót cà phê.\n");
            }
            break;
            
        case PHASE_POUR_COFFEE:
            updatePhasePourCoffee(t);
            if (robot.animationProgress >= 1.5f) {
                robot.animationProgress = 0.0f;
                robot.currentPhase = PHASE_RETURN_CUP;
                printf("Giai đoạn 5: Quay trở lại vị trí ban đầu.\n");
            }
            break;
            
        case PHASE_RETURN_CUP:
            updatePhaseReturnCup(t);
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_PLACE_CUP;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 6: Đặt cốc xuống bàn.\n");
            }
            break;
            
        case PHASE_PLACE_CUP:
            updatePhasePlaceCup(t);
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_RETRACT;
                robot.animationProgress = 0.0f;
                printf("Giai đoạn 7: Rút tay về.\n");
            }
            break;
            
        case PHASE_RETRACT:
            updatePhaseRetract(t);
            if (robot.animationProgress >= 1.0f) {
                robot.currentPhase = PHASE_COMPLETE;
                robot.isAnimating = false;
                printf("=== HOÀN THÀNH! Cà phê của bạn đã sẵn sàng. ===\n");
            }
            break;
            
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

    // Trụ xoay - using resource manager
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.6f);
    GLUquadric* quad = GLResourceManager::getInstance().getCylinderQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, 0.5f, 0.5f, 1.0f, 20, 1);
    glPopMatrix();

    // Thân trên (sẽ được vẽ trong hàm drawRobot để xoay)
}

// SIMPLIFIED robot arm with proper wrist control
void drawRobotArm() {
    glPushMatrix();
    
    // Move to shoulder joint
    glTranslatef(0.0f, 1.0f, 0.0f); 
    glRotatef(robot.shoulderRotateY, 0, 1, 0);  // Horizontal swing
    glRotatef(robot.shoulderRotateZ, 0, 0, 1);  // Up/down movement
    
    // Shoulder joint
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.4f, 15, 15);
    
    // Upper arm
    glTranslatef(SHOULDER_LENGTH / 2.0f, 0.0f, 0.0f); 
    glPushMatrix();
    glScalef(SHOULDER_LENGTH, 0.5f, 0.5f);
    glColor3f(0.6f, 0.6f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Elbow joint
    glTranslatef(SHOULDER_LENGTH / 2.0f, 0.0f, 0.0f); 
    glRotatef(robot.elbowAngle, 0, 0, 1);
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.3f, 12, 12);
    
    // Forearm
    glTranslatef(FOREARM_LENGTH / 2.0f, 0.0f, 0.0f); 
    glPushMatrix();
    glScalef(FOREARM_LENGTH, 0.4f, 0.4f);
    glColor3f(0.6f, 0.6f, 0.7f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Wrist joint with TWO rotations for proper gripper alignment
    glTranslatef(FOREARM_LENGTH / 2.0f, 0.0f, 0.0f); 
    glRotatef(robot.wristRotateZ, 0, 0, 1);  // Bend up/down
    glRotatef(robot.wristRotateY, 0, 1, 0);  // Twist left/right - KEY for cup grip!
    
    glColor3f(0.7f, 0.7f, 0.8f);
    glutSolidSphere(0.2f, 10, 10);

    // Gripper hand
    glTranslatef(0.2f, 0.0f, 0.0f); 
    drawRobotHand();
    
    glPopMatrix();
}

// SIMPLIFIED realistic gripper design
void drawRobotHand() {
    // Gripper base/palm
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.5f);
    glScalef(0.3f, 0.4f, 0.3f);
    glutSolidCube(1.0);
    glPopMatrix();
    
    // Gripper fingers - simple but effective
    glColor3f(0.5f, 0.5f, 0.6f);
    
    // Upper finger
    glPushMatrix();
    glTranslatef(0.15f, 0.15f, 0.0f);
    glRotatef(-robot.fingerAngle, 0, 0, 1);
    
    // Finger segment
    glTranslatef(0.15f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.3f, 0.08f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    
    // Finger tip
    glTranslatef(0.15f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.2f, 0.06f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPopMatrix();
    
    // Lower finger
    glPushMatrix();
    glTranslatef(0.15f, -0.15f, 0.0f);
    glRotatef(robot.fingerAngle, 0, 0, 1);
    
    // Finger segment
    glTranslatef(0.15f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.3f, 0.08f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    
    // Finger tip
    glTranslatef(0.15f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.2f, 0.06f, 0.15f);
    glutSolidCube(1.0);
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

// Vẽ máy pha cà phê chi tiết - improved with resource manager
void drawCoffeeMachine() {
    glPushMatrix();
    // Đặt máy trên mặt bàn và dịch ra xa
    glTranslatef(-4.0f, TABLE_HEIGHT + 1.1f, 0.0f);

    // Thân máy chính (vỏ kim loại)
    glColor3f(0.6f, 0.6f, 0.65f);
    glPushMatrix();
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

    // Vòi rót (từ group head) - using resource manager
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.8f);
    GLUquadric* quad = GLResourceManager::getInstance().getCylinderQuadric();
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad, 0.1f, 0.05f, 0.5f, 10, 1);
    glPopMatrix();

    // Tay cầm pha chế (portafilter handle)
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 1.4f);
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

    // Đồng hồ áp suất - using resource manager
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.75f);
    GLUquadric* diskQuad = GLResourceManager::getInstance().getDiskQuadric();
    
    // Mặt đồng hồ
    glColor3f(1.0f, 1.0f, 1.0f);
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
    glPopMatrix();

    glPopMatrix(); 
}

// SIMPLIFIED cup drawing with proper gripper alignment
void drawCup() {
    glPushMatrix();
    
    if (robot.cupInHand) {
        // Follow the exact same transformation chain as the gripper
        glTranslatef(0.0f, ROBOT_BASE_HEIGHT, 0.0f);
        glRotatef(robot.bodyRotateY, 0, 1, 0);
        
        glTranslatef(0.0f, 1.0f, 0.0f);
        glRotatef(robot.shoulderRotateY, 0, 1, 0);
        glRotatef(robot.shoulderRotateZ, 0, 0, 1);
        
        glTranslatef(SHOULDER_LENGTH, 0.0f, 0.0f);
        glRotatef(robot.elbowAngle, 0, 0, 1);
        
        glTranslatef(FOREARM_LENGTH, 0.0f, 0.0f);
        glRotatef(robot.wristRotateZ, 0, 0, 1);  // Match wrist Z rotation
        glRotatef(robot.wristRotateY, 0, 1, 0);  // Match wrist Y rotation - CRITICAL!
        
        // Position cup in gripper - offset to center between fingers
        glTranslatef(HAND_OFFSET + 0.1f, 0.0f, 0.0f);

    } else {
        // Cup on table
        glTranslatef(robot.cupX, robot.cupY, robot.cupZ);
    }
    
    // Draw cup body (cylinder)
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Orient cup upright
    GLUquadric* quad = GLResourceManager::getInstance().getCylinderQuadric();
    gluCylinder(quad, CUP_RADIUS, CUP_RADIUS, CUP_HEIGHT, 20, 1); 
    
    // Draw cup bottom
    GLUquadric* diskQuad = GLResourceManager::getInstance().getDiskQuadric();
    gluDisk(diskQuad, 0.0f, CUP_RADIUS, 20, 1); 

    // Draw coffee if cup is full
    if (robot.isCupFull) {
        glColor3f(0.4f, 0.2f, 0.1f);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, CUP_HEIGHT * 0.8f);
        gluDisk(diskQuad, 0.0f, CUP_RADIUS * 0.92f, 20, 1);
        glPopMatrix();
    }

    // Draw cup handle
    glPushMatrix();
    glTranslatef(CUP_RADIUS, 0.0f, CUP_HEIGHT * 0.5f); 
    glutSolidTorus(0.04f, 0.15f, 8, 16);
    glPopMatrix();
    
    glPopMatrix();
}

// Vẽ toàn bộ robot - improved with constants
void drawRobot() {
    glPushMatrix();
    glTranslatef(0.0f, ROBOT_BASE_HEIGHT, 0.0f);
    
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
