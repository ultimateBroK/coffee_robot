# 📊 SƠ ĐỒ TỔNG QUAN HỆ THỐNG

## 🎯 Mục đích
File này chứa các sơ đồ trực quan giúp hiểu cách hoạt động của hệ thống robot pha cà phê.

---

## 🏗️ Kiến trúc hệ thống

```mermaid
graph TB
    subgraph "File Structure"
        H[robot.h<br/>📋 Definitions]
        CPP[robot.cpp<br/>🤖 Robot Logic]
        MAIN[coffee_robot.cpp<br/>🎮 Main Program]
    end
    
    subgraph "Runtime Components"
        GL[OpenGL<br/>🎨 Graphics]
        GLUT[GLUT<br/>🎮 Events]
        ROBOT[Robot Instance<br/>🤖 State]
    end
    
    H --> CPP
    H --> MAIN
    CPP --> ROBOT
    MAIN --> GL
    MAIN --> GLUT
    MAIN --> ROBOT
    
    style H fill:#81d4fa,color:#000000
    style CPP fill:#ce93d8,color:#000000
    style MAIN fill:#a5d6a7,color:#000000
```

---

## 🔄 Luồng hoạt động chương trình

```mermaid
sequenceDiagram
    participant U as 👤 User
    participant M as 📱 Main
    participant GL as 🎨 OpenGL
    participant R as 🤖 Robot
    
    U->>M: Chạy chương trình
    M->>GL: initGL()
    M->>R: initRobot()
    M->>GL: glutMainLoop()
    
    loop Mỗi 16ms (60 FPS)
        M->>R: updateRobot()
        R->>R: Cập nhật animation
        M->>GL: display()
        GL->>GL: Vẽ robot + scene
    end
    
    U->>M: Nhấn SPACE
    M->>R: startCoffeeSequence()
    R->>R: phase = REACH_CUP
    
    U->>M: Nhấn R
    M->>R: resetRobot()
    R->>R: phase = IDLE
```

---

## 🎭 Các giai đoạn Robot (State Machine)

```mermaid
stateDiagram-v2
    [*] --> IDLE
    
    IDLE --> REACH_CUP : Nhấn SPACE
    REACH_CUP --> GRAB_CUP : progress >= 1.0
    GRAB_CUP --> TURN_TO_MACHINE : progress >= 1.0
    TURN_TO_MACHINE --> POUR_COFFEE : progress >= 1.0
    POUR_COFFEE --> RETURN_CUP : progress >= 1.5
    RETURN_CUP --> PLACE_CUP : progress >= 1.0
    PLACE_CUP --> GO_HOME : progress >= 1.0
    GO_HOME --> FINISHED : progress >= 1.0
    
    FINISHED --> IDLE : Nhấn R
    IDLE --> IDLE : Nhấn R (reset)
    
    note right of REACH_CUP
        🤲 Di chuyển tay tới cốc
        shoulderZ: 0° → -30°
        elbow: 0° → -50°
    end note
    
    note right of GRAB_CUP
        ✊ Nắm lấy cốc
        wristY: 0° → -90°
        fingers: 90° → 30°
    end note
    
    note right of POUR_COFFEE
        ☕ Pha cà phê (lâu nhất)
        Thời gian: 1.5x bình thường
        cupHasCoffee = true
    end note
```

---

## 🤖 Cấu trúc Robot

```mermaid
graph TD
    subgraph "Robot Structure"
        BASE[🏗️ Base<br/>Đế robot]
        BODY[📦 Body<br/>Thân robot]
        
        subgraph "Arm Hierarchy"
            SHOULDER[🔵 Shoulder<br/>shoulderY, shoulderZ]
            UPPER[📏 Upper Arm<br/>ARM_UPPER = 1.6f]
            ELBOW[🔵 Elbow<br/>elbow angle]
            LOWER[📏 Lower Arm<br/>ARM_LOWER = 1.4f]
            WRIST[🔵 Wrist<br/>wristZ, wristY]
            GRIPPER[🤏 Gripper<br/>fingers angle]
        end
    end
    
    BASE --> BODY
    BODY --> SHOULDER
    SHOULDER --> UPPER
    UPPER --> ELBOW
    ELBOW --> LOWER
    LOWER --> WRIST
    WRIST --> GRIPPER
    
    style BASE fill:#ef9a9a,color:#000000
    style SHOULDER fill:#ce93d8,color:#000000
    style ELBOW fill:#ce93d8,color:#000000
    style WRIST fill:#ce93d8,color:#000000
    style GRIPPER fill:#81c784,color:#000000
```

---

## 🌍 Scene Layout

```mermaid
graph LR
    subgraph "3D Scene (Top View)"
        subgraph "Table Area"
            TABLE[📋 Table<br/>10×8×0.2]
            ROBOT[🤖 Robot<br/>(0, 1.1, 0)]
            CUP[☕ Cup<br/>(2.2, TABLE_Y+0.25, 0)]
            MACHINE[⚙️ Coffee Machine<br/>(-4, TABLE_Y+1.1, 0)]
        end
        
        subgraph "Camera System"
            CAM[📷 Camera<br/>Distance: 15.0f]
            ANGLE_X[🔄 AngleX<br/>Up/Down]
            ANGLE_Y[🔄 AngleY<br/>Left/Right]
        end
    end
    
    ROBOT -.-> CUP
    ROBOT -.-> MACHINE
    CAM --> ROBOT
    
    style ROBOT fill:#90caf9,color:#000000
    style CUP fill:#ffcc80,color:#000000
    style MACHINE fill:#ce93d8,color:#000000
    style CAM fill:#a5d6a7,color:#000000
```

---

## ⚡ Animation System

```mermaid
graph TB
    subgraph "Animation Pipeline"
        TIMER[⏰ Timer<br/>16ms = 60 FPS]
        UPDATE[🔄 updateRobot()]
        PROGRESS[📊 robot.progress += ANIMATION_SPEED]
        SMOOTH[✨ smooth(progress)]
        JOINTS[🦾 Update Joint Angles]
        DISPLAY[🎨 display()]
    end
    
    TIMER --> UPDATE
    UPDATE --> PROGRESS
    PROGRESS --> SMOOTH
    SMOOTH --> JOINTS
    JOINTS --> DISPLAY
    DISPLAY --> TIMER
    
    subgraph "Smooth Function"
        INPUT[Input: t (0.0 → 1.0)]
        FORMULA[Formula: t² × (3 - 2t)]
        OUTPUT[Output: Smooth curve]
    end
    
    SMOOTH --> INPUT
    INPUT --> FORMULA
    FORMULA --> OUTPUT
    
    style TIMER fill:#fff176,color:#000000
    style SMOOTH fill:#a5d6a7,color:#000000
    style JOINTS fill:#90caf9,color:#000000
```

---

## 🎮 User Interaction

```mermaid
graph LR
    subgraph "Input Events"
        SPACE[SPACE<br/>🚀 Start Coffee]
        R_KEY[R Key<br/>🔄 Reset Robot]
        ESC[ESC<br/>🚪 Exit Program]
        MOUSE[🖱️ Mouse Drag<br/>📷 Rotate Camera]
        ARROWS[⬆️⬇️ Arrow Keys<br/>🔍 Zoom In/Out]
    end
    
    subgraph "Robot Actions"
        START[startCoffeeSequence()]
        RESET[resetRobot()]
        EXIT[exit(0)]
        ROTATE[Update cameraAngleX/Y]
        ZOOM[Update cameraDistance]
    end
    
    SPACE --> START
    R_KEY --> RESET
    ESC --> EXIT
    MOUSE --> ROTATE
    ARROWS --> ZOOM
    
    style SPACE fill:#81c784,color:#000000
    style R_KEY fill:#ef9a9a,color:#000000
    style ESC fill:#ffcc02,color:#000000
    style MOUSE fill:#ce93d8,color:#000000
    style ARROWS fill:#4fc3f7,color:#000000
```

---

## 📊 Data Flow

```mermaid
graph TD
    subgraph "Constants (robot.h)"
        SPEED[ANIMATION_SPEED<br/>0.015f]
        ARM_U[ARM_UPPER<br/>1.6f]
        ARM_L[ARM_LOWER<br/>1.4f]
    end
    
    subgraph "Robot State"
        PHASE[phase<br/>Current stage]
        PROG[progress<br/>0.0 → 1.0]
        ANGLES[Joint Angles<br/>shoulderY, shoulderZ, etc.]
        CUP_STATE[Cup State<br/>position, holding, hasCoffee]
    end
    
    subgraph "Rendering"
        TRANSFORMS[OpenGL<br/>Transformations]
        GEOMETRY[Draw Geometry<br/>Boxes, Cylinders, Spheres]
        COLORS[Apply Colors<br/>RGB values]
    end
    
    SPEED --> PROG
    ARM_U --> ANGLES
    ARM_L --> ANGLES
    PHASE --> ANGLES
    PROG --> ANGLES
    ANGLES --> TRANSFORMS
    TRANSFORMS --> GEOMETRY
    GEOMETRY --> COLORS
    CUP_STATE --> TRANSFORMS
    
    style SPEED fill:#ef9a9a,color:#000000
    style PHASE fill:#ce93d8,color:#000000
    style TRANSFORMS fill:#90caf9,color:#000000
```

---

## 🎯 **Cách sử dụng sơ đồ:**

1. **Bắt đầu với "Kiến trúc hệ thống"** - Hiểu cấu trúc file
2. **Xem "Luồng hoạt động"** - Hiểu cách chương trình chạy
3. **Nghiên cứu "State Machine"** - Hiểu các giai đoạn robot
4. **Tham khảo "Animation System"** - Hiểu cách animation hoạt động
5. **Sử dụng "Data Flow"** - Hiểu cách dữ liệu di chuyển trong hệ thống 