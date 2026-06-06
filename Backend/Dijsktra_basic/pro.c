#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INF 999999
#define MAX_NODES 30

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
char stationNames[MAX_NODES][50];
int numStations = 0;

int dist_time[MAX_NODES];  // Lưu thời gian thực tế
int dist_cost[MAX_NODES];  // Lưu chi phí thực tế
int prev_node[MAX_NODES];  // Lưu đỉnh trước để truy vết
int prev_line[MAX_NODES];  // Lưu tuyến tàu đã đi
bool visited[MAX_NODES];   // Đánh dấu ga đã duyệt

// =========================================================================
// 2. Các hàm cơ bản (Khởi tạo, Đọc file, Tìm ID)
// =========================================================================
void InitGraph() {
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == j) {
                map[i][j].time = 0;
                map[i][j].cost = 0;
            } else {
                map[i][j].time = INF;
                map[i][j].cost = INF;
            }
            map[i][j].line_ID = 0;
            map[i][j].is_active = true;
        }
    }
}

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
}

int GetStationID(char* name) {
    for (int i = 0; i < numStations; i++) {
        if (strcmp(stationNames[i], name) == 0) return i;
    }
    return -1;
}

// =========================================================================
// 1.4 Hàm Dijkstra Nâng Cấp - Tuỳ chọn Tiêu Chí
// criteria: 1 = Nhanh nhất (Time), 2 = Rẻ nhất (Cost), 3 = Cân bằng
// =========================================================================
void DijkstraAdvanced(int start, int end, int criteria) {
    int weight[MAX_NODES]; // Mảng lưu "điểm số" để xét Dijkstra

    for (int i = 0; i < numStations; i++) {
        weight[i] = INF;
        dist_time[i] = INF;
        dist_cost[i] = INF;
        prev_node[i] = -1;
        prev_line[i] = -1;
        visited[i] = false;
    }

    weight[start] = 0;
    dist_time[start] = 0;
    dist_cost[start] = 0;

    for (int count = 0; count < numStations - 1; count++) {
        // Tìm đỉnh u có trọng số nhỏ nhất
        int min_w = INF, u = -1;
        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && weight[v] <= min_w) { 
                min_w = weight[v]; 
                u = v; 
            }
        }

        if (u == -1 || u == end) break;
        visited[u] = true;

        // Cập nhật các đỉnh kề
        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && map[u][v].time != INF && map[u][v].time > 0) {
                
                int penalty = 0;
                // Phạt 5 phút nếu đổi tuyến tàu
                if (prev_node[u] != -1 && map[u][v].line_ID != prev_line[u]) {
                    penalty = 5; 
                }

                int new_time = dist_time[u] + map[u][v].time + penalty;
                int new_cost = dist_cost[u] + map[u][v].cost;
                int new_weight = 0;

                // Tính điểm số dựa trên sự lựa chọn của người dùng
                if (criteria == 1) {
                    new_weight = new_time; // Ưu tiên Thời gian
                } else if (criteria == 2) {
                    new_weight = new_cost; // Ưu tiên Giá vé
                } else {
                    new_weight = (new_time * 10) + new_cost; // Ưu tiên Cân bằng
                }

                // Cập nhật nếu tìm được đường tốt hơn
                if (new_weight < weight[v]) {
                    weight[v] = new_weight;
                    dist_time[v] = new_time;
                    dist_cost[v] = new_cost;
                    prev_node[v] = u;
                    prev_line[v] = map[u][v].line_ID;
                } 
                // Xử lý khi điểm số bằng nhau (Tie-breaker)
                else if (new_weight == weight[v]) {
                    if (criteria == 1 && new_cost < dist_cost[v]) { // Nhanh bằng nhau thì chọn cái rẻ hơn
                        dist_cost[v] = new_cost;
                        dist_time[v] = new_time;
                        prev_node[v] = u;
                        prev_line[v] = map[u][v].line_ID;
                    } else if (criteria == 2 && new_time < dist_time[v]) { // Rẻ bằng nhau thì chọn cái nhanh hơn
                        dist_cost[v] = new_cost;
                        dist_time[v] = new_time;
                        prev_node[v] = u;
                        prev_line[v] = map[u][v].line_ID;
                    }
                }
            }
        }
    }
}

// =========================================================================
// 1.5 Hàm truy vết đường đi
// =========================================================================
void PrintPath(int start, int end, int criteria) {
    if (dist_time[end] == INF) {
        printf("\n[!] Khong co duong di tu %s den %s!\n", stationNames[start], stationNames[end]);
        return;
    }

    int path[MAX_NODES], lines[MAX_NODES], count = 0, curr = end;
    while (curr != -1) {
        path[count] = curr;
        lines[count] = prev_line[curr];
        curr = prev_node[curr];
        count++;
    }

    printf("\n==== KET QUA TIM DUONG TOI UU ====\n");
    if (criteria == 1) printf("Tieu chi: NGAN NHAT (Uu tien thoi gian)\n");
    else if (criteria == 2) printf("Tieu chi: RE NHAT (Uu tien gia ve)\n");
    else printf("Tieu chi: CAN BANG (Toi uu thoi gian va gia ve)\n");
    
    printf("-> Tong thoi gian: %d phut (da tinh thoi gian doi tuyen)\n", dist_time[end]);
    printf("-> Tong chi phi:   %d Yen\n", dist_cost[end]);
    printf("----------------------------------\n");
    printf("Lo trinh chi tiet:\n");
    
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", stationNames[path[i]]);
        if (i > 0) printf(" --(Line %d)--> ", lines[i-1]);
    }
    printf("\n==================================\n");
}

// =========================================================================
// 1.7 Hệ thống Menu Console (Đã cập nhật UX)
// =========================================================================
void Menu() {
    int choice, criteria;
    char s_name[50], e_name[50];
    
    do {
        printf("\n========== HE THONG TOKYO METRO ==========\n");
        printf("1. Xem danh sach cac ga\n");
        printf("2. Tim duong di\n");
        printf("0. Thoat chuong trinh\n");
        printf("Nhap lua chon cua ban: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nDanh sach cac ga: \n");
                for (int i = 0; i < numStations; i++) {
                    printf("- %-15s", stationNames[i]);
                    if ((i + 1) % 3 == 0) printf("\n");
                }
                printf("\n");
                break;
                
            case 2:
                printf("\nNhap ten ga XUAT PHAT: "); scanf("%s", s_name);
                printf("Nhap ten ga DEN: "); scanf("%s", e_name);
                
                int s = GetStationID(s_name);
                int e = GetStationID(e_name);
                
                if (s == -1 || e == -1) {
                    printf("[!] Ten ga khong ton tai. Vui long viet hoa chu cai dau (VD: Shinjuku)!\n");
                } else {
                    // Vòng lặp con: Giữ nguyên Ga A và Ga B, chỉ chọn lại tiêu chí
                    do {
                        printf("\n--- So sanh lo trinh: [%s] -> [%s] ---\n", stationNames[s], stationNames[e]);
                        printf("  1. Ngan nhat (Uu tien thoi gian)\n");
                        printf("  2. Re nhat (Uu tien gia ve)\n");
                        printf("  3. Can bang (Tot nhat cho ca 2)\n");
                        printf("  0. Quay lai chon cap ga khac (Hoac ve Menu chinh)\n");
                        printf("Nhap tieu chi (0->3): ");
                        scanf("%d", &criteria);
                        
                        if (criteria >= 1 && criteria <= 3) {
                            // Chạy thuật toán và in kết quả dựa trên tiêu chí vừa chọn
                            DijkstraAdvanced(s, e, criteria);
                            PrintPath(s, e, criteria);
                        } else if (criteria != 0) {
                            printf("\n[!] Lua chon khong hop le, vui long chon lai!\n");
                        }
                    } while (criteria != 0); // Nhập 0 để thoát vòng lặp con
                }
                break;
        }
    } while (choice != 0); // Nhập 0 ở Menu chính để thoát chương trình
    
    printf("\nCam on ban da su dung he thong. Hen gap lai!\n");
}

int main() {
    InitGraph();
    LoadData("TokyoData.txt");
    Menu();
    return 0;
}