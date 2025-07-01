# 📊 FOLDER VISUALIZATION - HƯỚNG DẪN TRỰC QUAN

## 🎯 Mục đích
Folder này chứa các hướng dẫn trực quan để giúp bạn hiểu chương trình Robot Pha Cà Phê một cách nhanh chóng và dễ dàng hơn thông qua:
- Sơ đồ kiến trúc
- Biểu đồ luồng hoạt động  
- Hình ảnh minh họa các thành phần
- Hướng dẫn thay đổi tham số và hiệu ứng

## 📁 Cấu trúc folder

### 📄 Các file hướng dẫn chính:
- `robot_h_guide.md` - Hướng dẫn trực quan cho **robot.h**
- `robot_cpp_guide.md` - Hướng dẫn trực quan cho **robot.cpp** 
- `coffee_robot_cpp_guide.md` - Hướng dẫn trực quan cho **coffee_robot.cpp**
- `parameters_tuning_guide.md` - Hướng dẫn thay đổi tham số

## 🚀 Cách sử dụng

### Bước 1: Hiểu kiến trúc tổng thể
Đọc file `robot_h_guide.md` để nắm được:
- Các struct và enum được định nghĩa
- Các hằng số quan trọng
- Mối quan hệ giữa các thành phần

### Bước 2: Hiểu logic điều khiển robot
Đọc file `robot_cpp_guide.md` để hiểu:
- Cách robot di chuyển qua từng giai đoạn
- Logic animation và chuyển động
- Cách các hàm vẽ hoạt động

### Bước 3: Hiểu giao diện và tương tác
Đọc file `coffee_robot_cpp_guide.md` để nắm:
- Cách chương trình khởi tạo OpenGL
- Xử lý sự kiện bàn phím/chuột
- Vòng lặp chính của chương trình

### Bước 4: Tùy chỉnh và thử nghiệm
Đọc file `parameters_tuning_guide.md` để:
- Thay đổi tốc độ animation
- Điều chỉnh kích thước robot
- Tùy chỉnh màu sắc và ánh sáng

## 🎨 Ký hiệu trong hướng dẫn

### Mức độ quan trọng:
- 🔴 **Quan trọng nhất** - Phải hiểu
- 🟡 **Quan trọng** - Nên hiểu  
- 🟢 **Bổ sung** - Có thể bỏ qua

### Loại nội dung:
- 📊 Sơ đồ/Biểu đồ
- 💡 Giải thích khái niệm
- ⚙️ Hướng dẫn thay đổi tham số
- 🎮 Tương tác người dùng
- 🐛 Lỗi thường gặp

## 📖 Thứ tự đọc đề xuất

### Cho người mới bắt đầu:
1. `robot_h_guide.md` (Hiểu cấu trúc dữ liệu)
2. `coffee_robot_cpp_guide.md` (Hiểu luồng chương trình chính)
3. `robot_cpp_guide.md` (Hiểu logic robot)
4. `parameters_tuning_guide.md` (Thực hành)

### Cho người có kinh nghiệm:
1. Đọc nhanh tất cả các file
2. Tập trung vào `parameters_tuning_guide.md`
3. Thử nghiệm thay đổi các tham số

## 🎯 Mục tiêu học tập

Sau khi hoàn thành các hướng dẫn, bạn sẽ:
- ✅ Hiểu được kiến trúc chương trình robot
- ✅ Biết cách robot di chuyển qua từng giai đoạn
- ✅ Có thể tùy chỉnh chương trình theo ý muốn
- ✅ Hiểu được cách OpenGL hoạt động trong context này
- ✅ Có thể debug và sửa lỗi cơ bản

## 📞 Hỗ trợ

Nếu có thắc mắc về bất kỳ phần nào, hãy tham khảo:
- Comments trong code
- File `docs/` trong thư mục gốc
- Thử nghiệm thay đổi tham số nhỏ để quan sát hiệu ứng 