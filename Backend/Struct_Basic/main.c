#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INF 999999
#define MAX_NODES 20

typedef struct {
    int time;
    int cost;
    int line_ID;
    bool is_active;
} Edge;

Edge map[MAX_NODES][MAX_NODES];
char stationNames[MAX_NODES][50];
int numStations = 0;

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

// Hàm đọc file bây giờ dùng thẳng scanf, cực kỳ ngắn gọn!
void LoadData() {
    scanf("%d", &numStations);

    for (int i = 0; i < numStations; i++) {
        scanf("%s", stationNames[i]);
    }

    int numEdges;
    scanf("%d", &numEdges);

    for (int i = 0; i < numEdges; i++) {
        int u, v, t, c, line;
        scanf("%d %d %d %d %d", &u, &v, &t, &c, &line);

        map[u][v].time = t;
        map[u][v].cost = c;
        map[u][v].line_ID = line;
        
        map[v][u].time = t;
        map[v][u].cost = c;
        map[v][u].line_ID = line;
    }
    
    printf("[THONG BAO] Da tai du lieu: %d ga va %d tuyen duong.\n\n", numStations, numEdges);
}

int main() {
    // CHỈ CẦN 1 DÒNG NÀY ĐỂ BẺ LÁI DỮ LIỆU
    if (freopen("TokyoData.txt", "r", stdin) == NULL) {
        printf("[LOI] Khong tim thay file TokyoData.txt!\n");
        return 1; // Thoát chương trình nếu lỗi
    }

    InitGraph();
    LoadData(); // Gọi hàm bình thường

    // Test in danh sách
    printf("--- DANH SACH NHA GA ---\n");
    for(int i = 0; i < numStations; i++) {
        printf("ID [%d] : %s\n", i, stationNames[i]);
    }

    // Test in ma trận
    printf("\n--- MA TRAN THOI GIAN DI CHUYEN ---\n");
    for (int i = 0; i < numStations; i++) {
        for (int j = 0; j < numStations; j++) {
            if (map[i][j].time == INF) {
                printf("%5s", "INF");
            } else {
                printf("%5d", map[i][j].time);
            }
        }
        printf("\n");
    }

    return 0;
}