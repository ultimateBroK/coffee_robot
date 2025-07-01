# Makefile cho Robot Pha Cà Phê
# Tác giả: Coffee Robot Project
# Ngày: 25/06/2025

CC = g++
CFLAGS = -std=c++11 -Wall
LIBS = -lGL -lGLU -lglut -lm

# Tên file thực thi
TARGET = coffee_robot

# Source files - using simplified version
SOURCES = coffee_robot.cpp robot.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Thêm cờ debug nếu biến DEBUG=1 được truyền vào
ifeq ($(DEBUG),1)
    CFLAGS += -g -DDEBUG
endif

# Quy tắc mặc định: build tất cả
all: $(TARGET)

# Quy tắc để tạo file thực thi từ các file object
$(TARGET): $(OBJECTS)
	@echo "🔗 Đang liên kết để tạo file thực thi..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo "✅ Biên dịch thành công! File thực thi là: ./$(TARGET)"
	@echo "🚀 Chạy với lệnh: 'make run' hoặc './setup.sh'"

# Rule chung để biên dịch file .cpp thành .o
%.o: %.cpp robot.h
	@echo "🔨 Đang biên dịch $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Chạy chương trình
run: all
	@echo "🤖 Khởi động Robot Pha Cà Phê..."
	./$(TARGET)

# Dọn dẹp
clean:
	@echo "🧹 Dọn dẹp các file đã biên dịch..."
	rm -f $(TARGET) $(OBJECTS)
	@echo "✅ Đã dọn dẹp xong!"

# Cài đặt dependencies cho các hệ điều hành khác nhau
install-deps:
	@echo "📦 Kiểm tra hệ điều hành và cài đặt dependencies..."
	@if [ -f /etc/arch-release ]; then \
		echo "-> Phát hiện Arch Linux. Dùng paru..."; \
		paru -Syu --noconfirm --needed base-devel mesa glu freeglut; \
	elif [ -f /etc/debian_version ]; then \
		echo "-> Phát hiện Debian/Ubuntu. Dùng apt-get..."; \
		sudo apt-get update && sudo apt-get install -y build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev; \
	else \
		echo "⚠️ Không hỗ trợ tự động cài đặt cho HĐH này."; \
		echo "Vui lòng cài đặt 'base-devel' (hoặc tương đương), 'mesa', 'glu', và 'freeglut' thủ công."; \
		exit 1; \
	fi
	@echo "✅ Cài đặt dependencies thành công!"

# Kiểm tra hệ thống
check-system:
	@echo "🔍 Kiểm tra hệ thống..."
	@echo "Compiler: $(shell $(CC) --version | head -1)"
	@pkg-config --exists gl && echo "✅ OpenGL (gl): OK" || echo "❌ OpenGL (gl): KHÔNG TÌM THẤY"
	@pkg-config --exists glu && echo "✅ GLU: OK" || echo "❌ GLU: KHÔNG TÌM THẤY" 
	@pkg-config --exists glut && echo "✅ GLUT: OK" || echo "❌ GLUT: KHÔNG TÌM THẤY"

# Trợ giúp
help:
	@echo "📋 Các lệnh có sẵn:"
	@echo "  make           - Biên dịch chương trình"
	@echo "  make run       - Biên dịch và chạy"
	@echo "  make clean     - Dọn dẹp files"
	@echo "  make install-deps - Cài đặt thư viện cần thiết"
	@echo "  make check-system - Kiểm tra hệ thống"
	@echo "  make help      - Hiển thị trợ giúp này"

.PHONY: all run clean install-deps check-system help
