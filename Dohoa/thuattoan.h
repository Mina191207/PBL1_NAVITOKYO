#ifndef THUATTOAN_H
#define THUATTOAN_H

#include <stdbool.h>

#define INF 999999
#define MAX_NODES 30

// Cấu trúc dữ liệu cạnh của em
typedef struct {
    int time;
    int cost;
    int line_ID;
    bool is_active;
} Edge;

// Khai báo extern để file main.c (Raylib) có thể đọc được dữ liệu này
extern Edge map[MAX_NODES][MAX_NODES];
extern Edge backup_map[MAX_NODES][MAX_NODES];
extern char stationNames[MAX_NODES][50];
extern int numStations;

extern int dist_time[MAX_NODES];
extern int dist_cost[MAX_NODES];
extern int prev_node[MAX_NODES];
extern int prev_line[MAX_NODES];
extern bool visited[MAX_NODES];

// Khai báo các hàm cốt lõi
void InitGraph();
void BackupGraph();
void RestoreGraph();
void CloseStation(int station_id);
void CloseEdge(int u, int v);
void ApplyJRPass_OnlyJR();
void ApplyJRPass_Mix();
void LoadData(char *filename);
void DijkstraAdvanced(int start, int end, int criteria, int start_time, int start_line);
int GetLineFrequency(int line_id);
#endif