#ifndef ROBOT_H
#define ROBOT_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>

//=============================================================================
// CÁC HẰNG SỐ ĐƠN GIẢN - Dễ hiểu và dễ chỉnh sửa
//=============================================================================
const float ANIMATION_SPEED = 0.015f;
const float ARM_UPPER = 1.6f;      // Chiều dài cánh tay trên
const float ARM_LOWER = 1.4f;      // Chiều dài cánh tay dưới  
const float CUP_SIZE = 0.25f;      // Bán kính cốc
const float TABLE_Y = 0.1f;        // Độ cao bàn

//=============================================================================
// CÁC PHA HOẠT ĐỘNG - Những gì robot đang làm
//=============================================================================
enum RobotPhase {
    IDLE = 0,           // Chờ đợi
    REACH_CUP,          // Di chuyển tới cốc
    GRAB_CUP,           // Cầm lấy cốc
    TURN_TO_MACHINE,    // Quay về phía máy pha cà phê
    POUR_COFFEE,        // Pha cà phê
    RETURN_CUP,         // Mang cốc trở về bàn
    PLACE_CUP,          // Đặt cốc xuống
    GO_HOME,            // Quay về vị trí ban đầu
    FINISHED            // Hoàn thành
};

//=============================================================================
// TRẠNG THÁI ROBOT - Tất cả góc khớp và trạng thái của robot
//=============================================================================
struct Robot {
    // Góc các khớp (tính bằng độ)
    float shoulderY, shoulderZ;     // Xoay vai
    float elbow;                    // Gập khuỷu tay
    float wristZ, wristY;          // Xoay cổ tay (Z=lên/xuống, Y=xoắn)
    float fingers;                  // Mở/đóng gripper
    float body;                     // Xoay thân
    
    // Animation
    RobotPhase phase;
    float progress;                 // 0.0 đến 1.0 cho pha hiện tại
    bool moving;
    
    // Cốc
    float cupX, cupY, cupZ;
    bool holdingCup;
    bool cupHasCoffee;
};

//=============================================================================
// CÁC HÀM CHÍNH - Những hàm bạn gọi để sử dụng robot
//=============================================================================
void initRobot();               // Đưa robot về vị trí khởi tạo
void updateRobot();             // Cập nhật animation (gọi mỗi frame)
void startCoffeeSequence();     // Bắt đầu quy trình pha cà phê
void resetRobot();              // Reset về trạng thái ban đầu

void drawRobot();               // Vẽ robot
void drawScene();               // Vẽ bàn, máy pha cà phê, v.v.

//=============================================================================
// ROBOT TOÀN CỤC - Robot duy nhất trong scene của chúng ta
//=============================================================================
extern Robot robot;

#endif
