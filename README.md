- 👋 Xin chào, tôi là @duccom0123
- 👀 Đây là võ lâm 1 tôi đã chuyển đổi lên vs2022
- ⚡ Sửa đổi:
    * Mở rộng màn hình lên 1600x900
    * Thêm KMp3LibClass khi lên vs2022
- ⚡ Sử dụng:
    * B1: git clone https://github.com/duccom0123/jx1-vs2022.git
    * B2: mở SwordOnline\Sources\JXAll.sln bằng vs2022
    * B3: sửa id và pass của bishop tại SwordOnline\Sources\MultiServer\Bishop\Application.cpp dòng 78 (hiện tại đang là "fsjx", "1234")
    * B4: build solution
- Build ở chế độ debug với các file liên quan đến server
- Build ở chế độ client debug với các file liên quan đến client
- Các file sau khi build sẽ ở thư mục bin
- Đây là phiên bản sơ khai chưa hoàn thiện mục đích để học hỏi là chính, nên ae lưu ý khi sử dụng.
- Client & Server: https://drive.google.com/file/d/1dr8izoH0Cm15w3TE7aP3NKPefMaqXHPS/view?usp=sharing

- ⚡ Cập nhật:
    * 0.0.1:
        - fix văng gs khi chuyển pk(file liên quan):
            * SwordOnline\Headers\KProtocol.h dòng 1358-1370 sửa thành 1372-1380 (tagSHOW_MSG_SYNC)
            * SwordOnline\Sources\Core\Src\KPlayerPK.cpp dòng 353
            * ... vài file liên quan nữa ae tìm theo tagSHOW_MSG_SYNC (https://github.com/duccom0123/jx1-vs2022/commit/a1f54536d17c66b02a2ad8a14e1a09a2dc07205d)
	* 0.0.2:
        - Sửa lỗi khiến relay bị văng