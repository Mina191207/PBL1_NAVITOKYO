#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thuattoan.h"

Edge map[MAX_NODES][MAX_NODES];
Edge backup_map[MAX_NODES][MAX_NODES];
char stationNames[MAX_NODES][50];
int numStations = 0;

int dist_time[MAX_NODES];
int dist_cost[MAX_NODES];
int prev_node[MAX_NODES];
int prev_line[MAX_NODES];
bool visited[MAX_NODES];

void InitGraph() {
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == j) { map[i][j].time = 0; map[i][j].cost = 0; } 
            else { map[i][j].time = INF; map[i][j].cost = INF; }
            map[i][j].line_ID = 0;
            map[i][j].is_active = true;
        }
    }
}

void BackupGraph() {
    for(int i=0; i<MAX_NODES; i++) for(int j=0; j<MAX_NODES; j++) backup_map[i][j] = map[i][j];
}

void RestoreGraph() {
    for(int i=0; i<MAX_NODES; i++) for(int j=0; j<MAX_NODES; j++) map[i][j] = backup_map[i][j];
}

void CloseStation(int station_id) {
    for (int i = 0; i < numStations; i++) {
        map[station_id][i].is_active = false;
        map[i][station_id].is_active = false;
    }
}

void CloseEdge(int u, int v) {
    map[u][v].is_active = false;
    map[v][u].is_active = false;
} 

void ApplyJRPass_OnlyJR() {
    for(int i=0; i<numStations; i++) {
        for(int j=0; j<numStations; j++) {
            if (map[i][j].time != INF && map[i][j].line_ID != 1) { 
                map[i][j].time = INF; map[i][j].cost = INF;
            } else if (map[i][j].line_ID == 1) map[i][j].cost = 0; 
        }
    }
}

void ApplyJRPass_Mix() {
    for(int i=0; i<numStations; i++) {
        for(int j=0; j<numStations; j++) {
            if (map[i][j].line_ID == 1) map[i][j].cost = 0; 
        }
    }
}

void LoadData(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) exit(1);
    fscanf(file, "%d", &numStations);
    for (int i = 0; i < numStations; i++) fscanf(file, "%s", stationNames[i]);
    int numEdges; fscanf(file, "%d", &numEdges);
    for (int i = 0; i < numEdges; i++) {
        int u, v, t, c, line;
        fscanf(file, "%d %d %d %d %d", &u, &v, &t, &c, &line);
        map[u][v].time = t; map[u][v].cost = c; map[u][v].line_ID = line;
        map[v][u].time = t; map[v][u].cost = c; map[v][u].line_ID = line;
    }
    fclose(file);
    BackupGraph(); 
}

int GetLineFrequency(int line_id) {
    if (line_id == 0) return 0;
    if (line_id == 1) {
        return 5;
    }
    if (line_id >= 2 && line_id <= 5) {
        return 8; 
    }
    return 15; 
}

void DijkstraAdvanced(int start, int end, int criteria, int start_time, int start_line) {
    int weight[MAX_NODES]; 
    for (int i = 0; i < numStations; i++) {
        weight[i] = INF; dist_time[i] = INF; dist_cost[i] = INF;
        prev_node[i] = -1; prev_line[i] = -1; visited[i] = false;
    }
    
    // --- ĐỒNG BỘ THỜI GIAN Ở ĐÂY ---
    weight[start] = 0;              // Trọng số gốc vẫn là 0 để Dijkstra duyệt chuẩn
    dist_time[start] = start_time;  // Đồng hồ bắt đầu từ thời gian đã đi (ví dụ: 10 phút)
    dist_cost[start] = 0;           // Tiền vé thì chặng mới tính riêng, cộng dồn ở file main.c sau
    prev_line[start] = start_line;  // Ghi nhớ Line đang đứng (để xét xem có phải đi bộ 3 phút không)

    for (int count = 0; count < numStations - 1; count++) {
        int min_w = INF, u = -1;
        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && weight[v] <= min_w) { min_w = weight[v]; u = v; }
        }
        if (u == -1 || u == end) break;
        visited[u] = true;

        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && map[u][v].time != INF && map[u][v].time > 0 && map[u][v].is_active) {
                int current_arrival_time = dist_time[u];
                int walk_time = 0;
                int wait_time = 0;
                if (prev_line[u] != -1 && map[u][v].line_ID != prev_line[u]) {
                    if (map[u][v].line_ID == 0) {
                        walk_time = 0; 
                        wait_time = 0; 
                    } else {
                        walk_time = 3; 
                        int train_frequency = GetLineFrequency(map[u][v].line_ID); 
                        int time_after_walk = current_arrival_time + walk_time; 
                        if (train_frequency > 0) {
                            wait_time = train_frequency - (time_after_walk % train_frequency);
                            if (wait_time == train_frequency) wait_time = 0; 
                        }
                    }
                }
                int total_penalty_time = walk_time + wait_time;
                int new_time = dist_time[u] + map[u][v].time + total_penalty_time;
                int new_cost = dist_cost[u] + map[u][v].cost;
                int new_weight = (criteria == 1) ? new_time : (criteria == 2) ? new_cost : (new_time * 10) + new_cost; 
                if (new_weight < weight[v]) {
                    weight[v] = new_weight; dist_time[v] = new_time; dist_cost[v] = new_cost;
                    prev_node[v] = u; prev_line[v] = map[u][v].line_ID;
                } 
            }
        }
    }
}