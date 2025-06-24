#ifndef ROBOT_H
#define ROBOT_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>

// Trạng thái animation
enum AnimationPhase {
    PHASE_IDLE = 0,        // Chờ
    PHASE_REACH_CUP,       // Tay di chuyển ra cốc
    PHASE_GRAB_CUP,        // Tay nhấc cốc
    PHASE_TURN_TO_MACHINE, // Xoay người qua máy pha
    PHASE_POUR_COFFEE,     // Rót cà phê (chờ tại chỗ)
    PHASE_RETURN_CUP,      // Quay lại trả cốc
    PHASE_PLACE_CUP,       // Đặt cốc xuống
    PHASE_RETRACT,         // Rút tay về vị trí nghỉ
    PHASE_COMPLETE         // Hoàn thành
};

// Cấu trúc robot
struct RobotState {
    // Góc xoay các khớp
    float shoulderRotateY;   // Xoay vai quanh trục Y
    float shoulderRotateZ;   // Xoay vai quanh trục Z
    float elbowAngle;        // Góc gập khuỷu tay
    float wristAngle;        // Góc cổ tay
    float fingerAngle;       // Góc mở/đóng ngón tay
    float bodyRotateY;       // Xoay thân robot
    
    // Trạng thái animation
    AnimationPhase currentPhase;
    float animationProgress; // 0.0 đến 1.0 cho mỗi phase
    bool isAnimating;
    
    // Vị trí cốc
    float cupX, cupY, cupZ;
    bool cupInHand;
};

// Các hàm chính
void initRobot();
void drawRobot();
void drawEnvironment();
void updateAnimation();
void startAnimation();
void resetRobot();

// Hàm vẽ các thành phần
void drawRobotBody();
void drawRobotArm();
void drawRobotHand();
void drawTable();
void drawCoffeeMachine();
void drawCup();

// Biến toàn cục
extern RobotState robot;

#endif
