#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INF 999999
#define MAX_NODES 30

// === BẢNG MÀU ANSI CHO CONSOLE ===
#define C_RESET   "\x1b[0m"
#define C_CYAN    "\x1b[36m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_RED     "\x1b[31m"
#define C_BOLD    "\x1b[1m"

// =========================================================================
// 1. Khai báo Struct & Biến toàn cục
// =========================================================================
typedef struct {
    int time;
    int cost;
    int line_ID;
    bool is_active;
} Edge;

Edge map[MAX_NODES][MAX_NODES];
Edge backup_map[MAX_NODES][MAX_NODES]; // Mảng sao lưu đồ thị gốc
char stationNames[MAX_NODES][50];
int numStations = 0;

int dist_time[MAX_NODES];
int dist_cost[MAX_NODES];
int prev_node[MAX_NODES];
int prev_line[MAX_NODES];
bool visited[MAX_NODES];

// =========================================================================
// 2. Các hàm Quản lý Đồ thị (Backup, Restore, Đóng ga, JR Pass)
// =========================================================================
void InitGraph() {
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == j) {
                map[i][j].time = 0; map[i][j].cost = 0;
            } else {
                map[i][j].time = INF; map[i][j].cost = INF;
            }
            map[i][j].line_ID = 0;
            map[i][j].is_active = true;
        }
    }
}

// Sao lưu đồ thị gốc sau khi đọc file
void BackupGraph() {
    for(int i = 0; i < MAX_NODES; i++) {
        for(int j = 0; j < MAX_NODES; j++) {
            backup_map[i][j] = map[i][j];
        }
    }
}

// Khôi phục đồ thị về trạng thái chưa bị chỉnh sửa
void RestoreGraph() {
    for(int i = 0; i < MAX_NODES; i++) {
        for(int j = 0; j < MAX_NODES; j++) {
            map[i][j] = backup_map[i][j];
        }
    }
}

// TASK 2.2: Đóng ga
void CloseStation(int station_id) {
    for (int i = 0; i < numStations; i++) {
        map[station_id][i].is_active = false;
        map[i][station_id].is_active = false;
    }
}

// TASK 2.1: Chế độ 1 - Chỉ đi JR (Ép các tuyến khác thành INF, JR giá = 0)
void ApplyJRPass_OnlyJR() {
    for(int i = 0; i < numStations; i++) {
        for(int j = 0; j < numStations; j++) {
            if (map[i][j].time != INF && map[i][j].line_ID != 1) { // 1 là mã JR Yamanote
                map[i][j].time = INF;
                map[i][j].cost = INF;
            } else if (map[i][j].line_ID == 1) {
                map[i][j].cost = 0; 
            }
        }
    }
}

// TASK 2.1: Chế độ 2 - Đi Mix nhưng JR miễn phí
void ApplyJRPass_Mix() {
    for(int i = 0; i < numStations; i++) {
        for(int j = 0; j < numStations; j++) {
            if (map[i][j].line_ID == 1) {
                map[i][j].cost = 0; 
            }
        }
    }
}

// =========================================================================
// 3. Các hàm Đọc file & Truy xuất
// =========================================================================
void LoadData(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("[LOI] Khong tim thay file %s!\n", filename);
        exit(1);
    }
    fscanf(file, "%d", &numStations);
    for (int i = 0; i < numStations; i++) {
        fscanf(file, "%s", stationNames[i]);
    }
    int numEdges;
    fscanf(file, "%d", &numEdges);
    for (int i = 0; i < numEdges; i++) {
        int u, v, t, c, line;
        fscanf(file, "%d %d %d %d %d", &u, &v, &t, &c, &line);
        map[u][v].time = t; map[u][v].cost = c; map[u][v].line_ID = line;
        map[v][u].time = t; map[v][u].cost = c; map[v][u].line_ID = line;
    }
    fclose(file);
    BackupGraph(); // <--- Lưu lại bản gốc ngay sau khi đọc file
}

int GetStationID(char* name) {
    for (int i = 0; i < numStations; i++) {
        if (strcmp(stationNames[i], name) == 0) return i;
    }
    return -1;
}

// =========================================================================
// 4. Thuật toán Dijkstra (Đã thêm check is_active)
// =========================================================================
void DijkstraAdvanced(int start, int end, int criteria) {
    int weight[MAX_NODES]; 

    for (int i = 0; i < numStations; i++) {
        weight[i] = INF; dist_time[i] = INF; dist_cost[i] = INF;
        prev_node[i] = -1; prev_line[i] = -1; visited[i] = false;
    }

    weight[start] = 0; dist_time[start] = 0; dist_cost[start] = 0;

    for (int count = 0; count < numStations - 1; count++) {
        int min_w = INF, u = -1;
        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && weight[v] <= min_w) { 
                min_w = weight[v]; u = v; 
            }
        }

        if (u == -1 || u == end) break;
        visited[u] = true;

        for (int v = 0; v < numStations; v++) {
            // Đã thêm map[u][v].is_active để thuật toán tự né các ga bị đóng
            if (!visited[v] && map[u][v].time != INF && map[u][v].time > 0 && map[u][v].is_active) {
                
                int penalty = 0;
                if (prev_node[u] != -1 && map[u][v].line_ID != prev_line[u]) {
                    penalty = 5; 
                }

                int new_time = dist_time[u] + map[u][v].time + penalty;
                int new_cost = dist_cost[u] + map[u][v].cost;
                int new_weight = 0;

                if (criteria == 1) new_weight = new_time; 
                else if (criteria == 2) new_weight = new_cost; 
                else new_weight = (new_time * 10) + new_cost; 

                if (new_weight < weight[v]) {
                    weight[v] = new_weight; dist_time[v] = new_time; dist_cost[v] = new_cost;
                    prev_node[v] = u; prev_line[v] = map[u][v].line_ID;
                } 
                else if (new_weight == weight[v]) {
                    if (criteria == 1 && new_cost < dist_cost[v]) {
                        dist_cost[v] = new_cost; dist_time[v] = new_time;
                        prev_node[v] = u; prev_line[v] = map[u][v].line_ID;
                    } else if (criteria == 2 && new_time < dist_time[v]) {
                        dist_cost[v] = new_cost; dist_time[v] = new_time;
                        prev_node[v] = u; prev_line[v] = map[u][v].line_ID;
                    }
                }
            }
        }
    }
}

// =========================================================================
// 5. In kết quả (Cập nhật: Báo cáo chuyển tuyến)
// =========================================================================
void PrintPath(int start, int end, int criteria) {
    if (dist_time[end] == INF) {
        printf(C_RED "\n[!] Khong the den %s theo tieu chi nay!\n" C_RESET, stationNames[end]);
        return;
    }

    int path[MAX_NODES], lines[MAX_NODES], count = 0, curr = end;
    while (curr != -1) {
        path[count] = curr;
        lines[count] = prev_line[curr];
        curr = prev_node[curr];
        count++;
    }

    // --- LOGIC ĐẾM SỐ LẦN CHUYỂN TUYẾN ---
    int transfers = 0;
    // Đi ngược từ ga xuất phát đến ga kề cuối
    for (int i = count - 2; i > 0; i--) {
        // Nếu mã line đến ga hiện tại khác với mã line rời đi -> Đổi tuyến!
        if (lines[i] != lines[i-1]) {
            transfers++;
        }
    }

    printf("\n" C_CYAN "==== KET QUA TIM DUONG TOI UU ====\n" C_RESET);
    if (criteria == 1) printf("  Tieu chi: NGAN NHAT (Uu tien thoi gian)\n");
    else if (criteria == 2) printf("  Tieu chi: RE NHAT (Uu tien gia ve)\n");
    else printf("  Tieu chi: CAN BANG (Toi uu thoi gian va gia ve)\n");
    
    // In kết quả thời gian có kèm giải thích
    printf("  -> Tong thoi gian: " C_BOLD "%d phut " C_RESET, dist_time[end]);
    if (transfers > 0) {
        printf(C_YELLOW "(Da tinh %d phut cho %d lan chuyen tuyen)\n" C_RESET, transfers * 5, transfers);
    } else {
        printf(C_GREEN "(Di thang, khong can chuyen tuyen)\n" C_RESET);
    }
    
    printf("  -> Tong chi phi  : " C_BOLD "%d Yen\n" C_RESET, dist_cost[end]);
    printf("--------------------------------------------------\n");
    printf("  Lo trinh chi tiet:\n  ");
    
    // In lộ trình dọc
    for (int i = count - 1; i >= 0; i--) {
        if (i == count - 1) { // Ga xuất phát
            printf(C_GREEN "[%s]" C_RESET, stationNames[path[i]]);
        } else if (i == 0) {  // Ga đích
            printf(C_RED "[%s]\n" C_RESET, stationNames[path[i]]);
        } else {              // Ga trung gian
            printf("[%s]", stationNames[path[i]]);
            // Nếu ga này là điểm trung chuyển, in cảnh báo
            if (lines[i] != lines[i-1]) {
                printf(C_YELLOW " (Doi tuyen)" C_RESET);
            }
        }
        
        // In mũi tên và tên Line
        if (i > 0) {
            printf(" --(Line %d)--> ", lines[i-1]);
        }
    }
    printf(C_CYAN "==================================================\n" C_RESET);
}

// =========================================================================
// 6. Menu Logic (Giao diện màu sắc, chống mỏi mắt)
// =========================================================================
void Menu() {
    int choice;
    
    do {
        printf("\n");
        printf(C_CYAN C_BOLD "==================================================\n" C_RESET);
        printf(C_CYAN C_BOLD "              HE THONG TOKYO METRO                \n" C_RESET);
        printf(C_CYAN C_BOLD "==================================================\n" C_RESET);
        printf("  [1] Xem danh sach cac ga\n");
        printf("  [2] Chon ga de kiem thu lo trinh\n");
        printf("  [0] Thoat chuong trinh\n");
        printf("--------------------------------------------------\n");
        printf(C_YELLOW "=> Nhap lua chon cua ban: " C_RESET);
        scanf("%d", &choice);

        if (choice == 1) {
            printf("\n" C_CYAN "--- DANH SACH CAC GA ---" C_RESET "\n");
            for (int i = 0; i < numStations; i++) {
                printf("- %-15s", stationNames[i]);
                if ((i + 1) % 3 == 0) printf("\n");
            }
            printf("\n");
        } 
        else if (choice == 2) {
            char s_name[50], e_name[50];
            printf("\n" C_YELLOW ">> Nhap ten ga XUAT PHAT: " C_RESET); scanf("%s", s_name);
            printf(C_YELLOW ">> Nhap ten ga DEN: " C_RESET); scanf("%s", e_name);
            
            int s = GetStationID(s_name);
            int e = GetStationID(e_name);
            
            if (s == -1 || e == -1) {
                printf(C_RED "\n[!] Ten ga khong ton tai. Vui long kiem tra lai!\n" C_RESET);
                continue;
            }

            bool has_jr = false;
            int closed_id = -1;
            int sub_choice;

            do {
                printf("\n");
                printf(C_CYAN "==================================================\n" C_RESET);
                printf(C_CYAN "             BANG DIEU KHIEN KIEM THU             \n" C_RESET);
                printf(C_CYAN "==================================================\n" C_RESET);
                
                // Hiển thị trạng thái cực kỳ trực quan với màu sắc
                printf("  Hanh trinh : " C_BOLD "[%s] -> [%s]\n" C_RESET, stationNames[s], stationNames[e]);
                
                printf("  The JR Pass: ");
                if (has_jr) printf(C_GREEN C_BOLD "[ DANG BAT ]\n" C_RESET);
                else printf(C_YELLOW "[ DANG TAT ]\n" C_RESET);
                
                printf("  Ga su co   : ");
                if (closed_id == -1) printf(C_GREEN "Khong co\n" C_RESET);
                else printf(C_RED C_BOLD "[DONG CUA] %s\n" C_RESET, stationNames[closed_id]);
                
                printf("--------------------------------------------------\n");
                printf("  [1] Chon Tieu chi & CHAY THUAT TOAN\n");
                printf("  [2] Thay doi the JR Pass (Bat/Tat)\n");
                printf("  [3] Thay doi Ga su co (Dong/Mo lai)\n");
                printf("  [4] Xem lai danh sach cac ga\n");
                printf("  [0] Quay lai Menu chinh\n");
                printf("--------------------------------------------------\n");
                printf(C_YELLOW "=> Lua chon cua ban: " C_RESET);
                scanf("%d", &sub_choice);

                switch (sub_choice) {
                    case 1:
                        RestoreGraph();
                        if (closed_id != -1) CloseStation(closed_id);

                        if (has_jr) {
                            printf(C_CYAN "\n--- Dang thu lo trinh ONLY JR (Nhanh Nhat) ---\n" C_RESET);
                            ApplyJRPass_OnlyJR();
                            DijkstraAdvanced(s, e, 1); 

                            if (dist_time[e] == INF) {
                                printf(C_RED "[!] Chua toi dich bang JR duoc -> Chuyen sang MIX.\n" C_RESET);
                                RestoreGraph();
                                if (closed_id != -1) CloseStation(closed_id);
                                ApplyJRPass_Mix();

                                int criteria;
                                printf(C_YELLOW ">> Chon tieu chi Metro (1=Nhanh, 2=Re, 3=Can bang): " C_RESET);
                                scanf("%d", &criteria);
                                
                                DijkstraAdvanced(s, e, criteria);
                                PrintPath(s, e, criteria);
                            } else {
                                PrintPath(s, e, 1); 
                            }
                        } else {
                            int criteria;
                            printf(C_CYAN "\n--- Tim duong KET HOP (Mix) ---\n" C_RESET);
                            printf(C_YELLOW ">> Chon tieu chi (1=Nhanh, 2=Re, 3=Can bang): " C_RESET);
                            scanf("%d", &criteria);
                            
                            DijkstraAdvanced(s, e, criteria);
                            PrintPath(s, e, criteria);
                        }
                        
                        // --- LOGIC HỎI NHANH: ĐÓNG CODE HAY TIẾP TỤC ---
                        int post_action;
                        printf("\n" C_YELLOW ">> Ban co muon tiep tuc kiem thu hay dong code? (1 = Tiep tuc, 0 = Dong ngay): " C_RESET);
                        scanf("%d", &post_action);
                        
                        if (post_action == 0) {
                            printf(C_CYAN "\nCam on ban da su dung he thong. Hen gap lai!\n" C_RESET);
                            exit(0); // Lệnh exit(0) sẽ ép tắt toàn bộ chương trình ngay lập tức
                        }
                        // Nếu post_action == 1, vòng lặp do-while tự động quay lại Bảng điều khiển
                        break;
                        
                    case 2:
                        has_jr = !has_jr; 
                        break; // Đổi trạng thái âm thầm rồi load lại bảng điều khiển

                    case 3:
                        if (closed_id == -1) {
                            char closed_name[50];
                            printf("\n" C_YELLOW ">> Nhap ten ga muon DONG CUA (Su co): " C_RESET);
                            scanf("%s", closed_name);
                            int temp_id = GetStationID(closed_name);
                            if (temp_id != -1) closed_id = temp_id;
                            else printf(C_RED "[!] Khong tim thay ga %s!\n" C_RESET, closed_name);
                        } else {
                            closed_id = -1; 
                        }
                        break;

                    case 4:
                        printf("\n" C_CYAN "--- DANH SACH CAC GA ---" C_RESET "\n");
                        for (int i = 0; i < numStations; i++) {
                            printf("- %-15s", stationNames[i]);
                            if ((i + 1) % 3 == 0) printf("\n");
                        }
                        printf("\n");
                        break;
                }
            } while (sub_choice != 0); 
        }
    } while (choice != 0); 
}

int main() {
    InitGraph();
    LoadData("TokyoData.txt");
    Menu();
    return 0;
}