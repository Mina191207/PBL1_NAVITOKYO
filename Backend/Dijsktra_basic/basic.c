#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Thư viện để dùng kiểu true/false trong C

#define INF 999999
#define MAX_NODES 30

// 1. Khai báo Struct bằng typedef trong C
typedef struct {
    int time;
    int cost;
    int line_ID;
    bool is_active;
} Edge;

// 2. Khai báo các biến Toàn cục
Edge map[MAX_NODES][MAX_NODES];
char stationNames[MAX_NODES][50]; // Trong C, mảng chuỗi là mảng 2 chiều ký tự
int numStations = 0;

// 3. Hàm khởi tạo ma trận
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

// 4. Hàm đọc file (Dùng fscanf của C)
void LoadData(const char* filename) {
    FILE *file = fopen("TokyoData_22.txt", "r"); // Mở file riêng biệt, không đụng tới stdin
    if (file == NULL) {
        printf("[LOI] Khong tim thay file %s!\n", filename);
        return;
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

        map[u][v].time = t;
        map[u][v].cost = c;
        map[u][v].line_ID = line;
        
        map[v][u].time = t;
        map[v][u].cost = c;
        map[v][u].line_ID = line;
    }

    fclose(file); // Đọc xong phải đóng file lại cho an toàn
    printf("[THONG BAO] Da tai thanh cong: %d ga va %d tuyen duong.\n\n", numStations, numEdges);
}

// 5. Hàm main để Test
int main() {
    InitGraph();               // Quét dọn ma trận
    LoadData("TokyoData.txt"); // Gọi hàm đọc file chuẩn

    printf("--- DANH SACH NHA GA ---\n");
    for(int i = 0; i < numStations; i++) {
        printf("ID [%d] : %s\n", i, stationNames[i]);
    }

    printf("\n--- MA TRAN THOI GIAN DI CHUYEN ---\n");
    for (int i = 0; i < numStations; i++) {
        for (int j = 0; j < numStations; j++) {
            if (map[i][j].time == INF) {
                printf("%5s", "INF"); // In chữ INF chiếm 5 ô cho thẳng hàng
            } else {
                printf("%5d", map[i][j].time); // In số chiếm 5 ô
            }
        }
        printf("\n");
    }

    return 0;
}