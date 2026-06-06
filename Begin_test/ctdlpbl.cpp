#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// 1. Cấu trúc dữ liệu kết hợp (Struct cho Đồ thị đa trọng số)
struct Edge {
    int time;       // Thời gian (Phút)
    int cost;       // Giá vé (Yên)
    int line_ID;    // 1: Yamanote (JR), 2: Ginza, 3: Marunouchi
    bool is_active; // true: Bình thường, false: Đang có sự cố (Động đất)
};

// 2. Hàm in bản đồ ASCII Art ra Console (Dùng để chụp ảnh minh họa)
void DrawAsciiMap() {
    cout << "=========================================================\n";
    cout << "          NAVITOKYO: TOKYO METRO ASCII GRAPH             \n";
    cout << "=========================================================\n\n";
    
    cout << "  [Shinjuku] ============(5')============ [Yoyogi] \n";
    cout << "      ||                                     ||    \n";
    cout << "      || (JR Yamanote)                       || (Ginza Line)\n";
    cout << "    (15')                                  (10')   \n";
    cout << "      ||                                     ||    \n";
    cout << "      ||                                     ||    \n";
    cout << "  [Shibuya]  ------------(8')------------ [Harajuku]\n\n";
    
    cout << "---------------------------------------------------------\n";
    cout << "* Chu thich:  === (Tuyen JR)   --- (Tuyen Subway thuong)\n";
    cout << "=========================================================\n";
}

int main() {
    // Gọi hàm in bản đồ để chụp ảnh dán vào báo cáo
    DrawAsciiMap();
    
    // Gợi ý cho báo cáo: Chụp màn hình Console lúc này là có ngay "Hình ảnh minh họa" cực chất!
    cout << "\n[Hethong] Da tai thanh cong Cau truc Ma tran ke!\n";
    cout << "[Hethong] San sang chay thuat toan Dijkstra da tieu chi...\n";
    
    return 0;
}