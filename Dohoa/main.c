#include "include/raylib.h"
#include "thuattoan.h" 
#include <stdio.h>
#include <math.h>
#include <string.h>


typedef struct {
    const char* name; int originalX; int originalY;
    int drawX; int drawY; int textOffsetX; int textOffsetY;
} StationUI;

typedef struct {
    int gaA; int gaB; Color color;
    int offsetX; int offsetY; int curveOffsetX; int curveOffsetY; 
} EdgeUI;

int main(void) {
    InitGraph();
    LoadData("TokyoData.txt");

    InitWindow(1500, 800, "NaviTokyo - He Thong Tuong Tac UI");
    SetTargetFPS(60); 

    StationUI stations[22] = {
        {"Shinjuku", 279, 407, 0, 0, -90, -10}, {"Shibuya", 278, 537, 0, 0, -85, 10},
        {"Tokyo", 758, 461, 0, 0, 15, 15}, {"Ikebukuro", 323, 184, 0, 0, -20, -25},
        {"Ueno", 757, 168, 0, 0, 15, -20}, {"Shimbashi", 758, 597, 0, 0, 15, 10},
        {"Ebisu", 278, 587, 0, 0, -25, 15}, {"Akihabara", 764, 271, 0, 0, 15, -10},
        {"Ginza", 807, 530, 0, 0, 15, 15}, {"Akasaka-mitsuke", 529, 476, 0, 0, 15, -25},
        {"Kasumigaseki", 636, 555, 0, 0, 0, -40}, {"Shinjuku-sanchome", 349, 394, 0, 0, -80, -25},
        {"Omote-sando", 371, 544, 0, 0, -30, 15}, {"Nihombashi", 823, 418, 0, 0, 15, -10},
        {"Kanda", 745, 341, 0, 0, -70, -10}, {"Asakusa", 897, 165, 0, 0, 15, -10},
        {"Yotsuya", 449, 397, 0, 0, 15, -25}, {"Otemachi", 688, 416, 0, 0, -100, 0},
        {"Roppongi", 445, 590, 0, 0, -45, 15}, {"Naka-meguro", 219, 589, 0, 0, -135, -20},
        {"Higashi-ginza", 855, 530, 0, 0, 15, 15}, {"Meiji-jingumae", 306, 491, 0, 0, -150, -10}
    };

    float scale = 1.35f; int offsetX = -150; int offsetY = -80;      
    for (int i = 0; i < 22; i++) {
        stations[i].drawX = stations[i].originalX * scale + offsetX;
        stations[i].drawY = stations[i].originalY * scale + offsetY;
    }

    Color C_JR = LIME; Color C_GINZA = ORANGE; Color C_MARU = RED; Color C_HIBIYA = GRAY; Color C_FUKU = BROWN;

    EdgeUI map_edges[] = {
        {3,0, C_JR, 0, 0, 0, 0}, {0,1, C_JR, 0, 0, 0, 0}, {1,6, C_JR, 0, 0, 0, 0}, 
        {6,5, C_JR, 0, 0, 0, 60}, {5,2, C_JR, 0, 0, 0, 0}, {2,7, C_JR, 0, 0, 0, 0}, {7,4, C_JR, 0, 0, 0, 0}, {4,3, C_JR, 0, 0, 0, 0},
        {1,12, C_GINZA, 0, 0, 0, 0}, {12,9, C_GINZA, 0, 0, 0, 0}, {9,5, C_GINZA, 0, 0, -50, 40}, 
        {5,8, C_GINZA, 0, 0, 0, 0}, {8,13, C_GINZA, 0, 0, 0, 0}, {13,14, C_GINZA, 0, 0, 0, 0}, {14,4, C_GINZA, 0, 0, 0, 0}, {4,15, C_GINZA, 0, 0, 0, 0},
        {0,11, C_MARU, 0, 0, 0, 0}, {11,16, C_MARU, 0, 0, 0, 0}, {16,9, C_MARU, 0, 0, 0, 0}, {9,10, C_MARU, 0, 0, 0, 0}, {10,8, C_MARU, 0, 0, 0, 0}, {8,2, C_MARU, 0, 0, 0, 0}, {2,17, C_MARU, 0, 0, 0, 0}, {17,3, C_MARU, 0, 0, 0, 0},
        {19,6, C_HIBIYA, 0, 0, 0, 0}, {6,18, C_HIBIYA, 0, 0, 0, 0}, {18,10, C_HIBIYA, 0, 0, 0, 0}, 
        {10,8, C_HIBIYA, 0, 6, 0, 0}, {8,20, C_HIBIYA, 0, 0, 0, 0}, {20,7, C_HIBIYA, 0, 0, 60, 0}, {7,4, C_HIBIYA, 6, 0, 0, 0},    
        {3,11, C_FUKU, 0, 0, 0, 0}, {11,21, C_FUKU, 0, 0, 0, 0}, {21,1, C_FUKU, 0, 0, 0, 0}
    };
    int total_edges = 33;

    int gaDi = -1;
    int gaDen = -1;
    bool is_JR_ON = false;
    int criteria = 3; 
    
    bool has_result = false;
    int trace_path[MAX_NODES]; 
    int path_wait_times[MAX_NODES] = {0}; 
    int path_walk_times[MAX_NODES] = {0}; 
    int path_length = 0;

    // --- BIẾN CHO TÍNH NĂNG CÁ NHÂN HÓA & SỰ CỐ ---
    bool has_disruption = false; // Đã xảy ra sự cố chưa
    int current_station = -1;    // Ga khách đang đứng khi xảy ra sự cố
    int broken_station = -1;     // Ga phía trước bị sập
    int history_time = 0;        // Tổng thời gian ĐÃ ĐI trước sự cố
    int history_cost = 0;        // Tổng tiền vé ĐÃ ĐI trước sự cố

    int normal_time = 0;
    int normal_cost = 0;
    int normal_penalty = 0;
    int history_pure_time = 0;

    int last_gaDi = -1;
    int last_gaDen = -1;
    int last_time = 0;
    int last_cost = 0;
    int last_penalty = 0;
    bool is_comparing_criteria = false; // Cờ hiệu bật tính năng so sánh tiêu chí

    // --- BIẾN HIỂN THỊ CHỮ PHẠT TRÊN MAP ---
    char penalty_text[50] = "";
    int penalty_x = 0;
    int penalty_y = 0;

    Rectangle btn_TimDuong = {1170, 200, 120, 40};
    Rectangle btn_Reset = {1310, 200, 120, 40};
    Rectangle btn_JR = {1170, 125, 260, 30};
    Rectangle btn_TieuChi = {1170, 165, 260, 30};

    while (!WindowShouldClose()) { 
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (int i = 0; i < 22; i++) {
                if (CheckCollisionPointCircle(mousePos, (Vector2){stations[i].drawX, stations[i].drawY}, 15.0f)) {
                    if (gaDi == -1) gaDi = i;
                    else if (gaDen == -1 && i != gaDi) gaDen = i;
                }
            }

            if (CheckCollisionPointRec(mousePos, btn_JR)) is_JR_ON = !is_JR_ON;

            if (CheckCollisionPointRec(mousePos, btn_TieuChi)) {
                criteria++;
                if (criteria > 3) criteria = 1;
            }

            if (CheckCollisionPointRec(mousePos, btn_Reset)) {
                gaDi = -1; gaDen = -1; has_result = false; path_length = 0;
                has_disruption = false; current_station = -1; broken_station = -1; 
                history_time = 0; history_cost = 0; strcpy(penalty_text, "");
            }

            if (CheckCollisionPointRec(mousePos, btn_TimDuong) && gaDi != -1 && gaDen != -1) {
                // So sánh: Nếu không đổi ga, mà chỉ bấm lại nút -> Đang đổi tiêu chí
                if (gaDi == last_gaDi && gaDen == last_gaDen && has_result && !has_disruption) {
                    is_comparing_criteria = true;
                    last_time = normal_time;       // Lưu lại thông số của tiêu chí cũ
                    last_cost = normal_cost;
                    last_penalty = normal_penalty;
                } else {
                    is_comparing_criteria = false; // Đi tuyến mới hoàn toàn
                    last_gaDi = gaDi;
                    last_gaDen = gaDen;
                }

                RestoreGraph(); 
                if (is_JR_ON) ApplyJRPass_OnlyJR(); 
                
                DijkstraAdvanced(gaDi, gaDen, criteria, 0, -1); 

                if (dist_time[gaDen] == INF && is_JR_ON) {
                    RestoreGraph();
                    ApplyJRPass_Mix();
                    DijkstraAdvanced(gaDi, gaDen, criteria, 0, -1);
                }

                if (dist_time[gaDen] != INF) {
                    int count = 0, curr = gaDen;
                    int temp[MAX_NODES];
                    while (curr != -1) {
                        temp[count++] = curr;
                        curr = prev_node[curr];
                    }
                    path_length = count;
                    for (int i = 0; i < count; i++) {
                        trace_path[i] = temp[count - 1 - i];
                    }
                    // Đảo ngược mảng xong thì chèn đoạn này vào:
                    // LƯU KẾT QUẢ CỦA LẦN TÌM ĐƯỜNG 1 (BÌNH THƯỜNG)
                    normal_time = dist_time[gaDen];
                    normal_cost = dist_cost[gaDen];
                    
                    int pure_time = 0;
                    for (int i = 0; i < path_length - 1; i++) {
                        pure_time += map[trace_path[i]][trace_path[i+1]].time;
                    }
                    normal_penalty = normal_time - pure_time;
                    history_pure_time = 0;

                    has_result = true; 

                    int current_time_accumulator = 0; 
                    for (int i = 0; i < path_length - 1; i++) {
                        int u = trace_path[i];
                        int v = trace_path[i+1];
                        int line_truoc = (i == 0) ? -1 : map[trace_path[i-1]][u].line_ID;
                        int line_hien_tai = map[u][v].line_ID;

                        path_wait_times[i] = 0;
                        path_walk_times[i] = 0;

                        if (line_hien_tai == 0) {
                            path_walk_times[i] = map[u][v].time; 
                            current_time_accumulator += map[u][v].time;
                        } else {
                            int walk_trong_ga = 0;
                            if (line_truoc != -1 && line_hien_tai != line_truoc) {
                                walk_trong_ga = 3; 
                            }
                            
                            int thoi_diem_san_sang = current_time_accumulator + walk_trong_ga;
                            int tan_suat = GetLineFrequency(line_hien_tai); 
                            int cho_tau = 0;
                            if (tan_suat > 0) {
                                cho_tau = (tan_suat - (thoi_diem_san_sang % tan_suat)) % tan_suat;
                            }

                            path_walk_times[i] = walk_trong_ga;
                            path_wait_times[i] = cho_tau;
                            
                            current_time_accumulator += (walk_trong_ga + cho_tau + map[u][v].time);
                        }
                    }
                }
            }
        }

        // =========================================================================
        // --- XỬ LÝ SỰ CỐ GIỮA ĐƯỜNG TH1 & TH2 (CLICK CHUỘT PHẢI VÀO GA) ---
        // =========================================================================
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && has_result && !has_disruption) {
            // Quét từ ga số 2 (index 1) đến ga sát cuối
            for (int i = 1; i < path_length - 1; i++) {
                int clicked_ga = trace_path[i];
                Vector2 ga_pos = {stations[clicked_ga].drawX, stations[clicked_ga].drawY};
                
                if (CheckCollisionPointCircle(mousePos, ga_pos, 15.0f)) {
                    int ga_hien_tai = trace_path[i];     // Ga khách đang đứng (Ginza hoặc Nihombashi)
                    int ga_truoc = trace_path[i - 1];    // Ga vừa đi qua
                    int ga_tiep = trace_path[i + 1];     // Ga chuẩn bị đi tới nhưng đường bị hỏng
                    
                    int line_in = map[ga_truoc][ga_hien_tai].line_ID;
                    int line_out = map[ga_hien_tai][ga_tiep].line_ID;
                    
                    // 1. TÍNH LẠI THỜI GIAN/TIỀN VÉ LỊCH SỬ ĐÃ ĐI
                    history_time = 0; history_cost = 0; history_pure_time = 0;
                    int temp_time_acc = 0;
                    for (int k = 0; k < i; k++) {
                        int u = trace_path[k]; int v = trace_path[k+1];
                        int l_truoc = (k == 0) ? -1 : map[trace_path[k-1]][u].line_ID;
                        int l_hien = map[u][v].line_ID;
                        
                        history_cost += map[u][v].cost; // Cộng tiền vé
                        history_pure_time += map[u][v].time;

                        int walk_tg = 0, wait_tg = 0;
                        if (l_hien == 0) { walk_tg = map[u][v].time; } 
                        else {
                            if (l_truoc != -1 && l_hien != l_truoc) walk_tg = 3;
                            int thoi_diem = temp_time_acc + walk_tg;
                            int freq = GetLineFrequency(l_hien);
                            if (freq > 0) wait_tg = (freq - (thoi_diem % freq)) % freq;
                        }
                        temp_time_acc += walk_tg + wait_tg + map[u][v].time;
                    }
                    history_time = temp_time_acc; // Đồng hồ tại thời điểm xảy ra sự cố
                    
                    // 2. PHÂN LOẠI SỰ CỐ VÀ THIẾT LẬP LẠI ĐỒNG HỒ
                    int start_cuu_ho = -1;
                    int start_time_cuu_ho = history_time;
                    int start_line_cuu_ho = -1;
                    int cut_index = -1; // Vị trí cắt mảng để nối

                    RestoreGraph(); // Mở khóa mọi thứ để chuẩn bị setup sự cố mới

                    if (line_in == line_out && line_in != 0) {
                        // --- TH1: CÙNG TUYẾN (Sự cố kẹt trên tàu) ---
                        start_cuu_ho = ga_truoc; 
                        
                        // Lịch sử đã tính thời gian TỚI ga kẹt. 
                        // Giờ cộng thêm hình phạt: Thời gian NGỒI TÀU LÙI VỀ ga trước.
                        start_time_cuu_ho += map[ga_hien_tai][ga_truoc].time; 
                        start_line_cuu_ho = line_in;
                        
                        CloseEdge(ga_hien_tai, ga_tiep); 
                        CloseEdge(ga_truoc, ga_hien_tai); 
                        
                        broken_station = ga_hien_tai; 
                        cut_index = i - 1;            
                        
                        // LƯU CHỮ PHẠT (TH1: Rõ ràng việc kẹt và phải lùi xe)
                        sprintf(penalty_text, "Ket tau. Lui xe: %dp", map[ga_hien_tai][ga_truoc].time);
                        penalty_x = (stations[ga_hien_tai].drawX + stations[ga_truoc].drawX) / 2 - 40;
                        penalty_y = (stations[ga_hien_tai].drawY + stations[ga_truoc].drawY) / 2 - 20;

                    } else {
                        // --- TH2: KHÁC TUYẾN (Sự cố trung chuyển) ---
                        start_cuu_ho = ga_hien_tai;
                        
                        // Khách đi bộ sang line mới mất 3p. Thấy biển báo sập. 
                        // Đi bộ ngược về lại sảnh cũ mất 3p. -> TỔNG THIỆT HẠI: 6 phút!
                        start_time_cuu_ho += 6; 
                        start_line_cuu_ho = line_in;
                        
                        CloseEdge(ga_hien_tai, ga_tiep); 
                        
                        broken_station = ga_tiep; 
                        cut_index = i;            

                        // LƯU CHỮ PHẠT (TH2: Hiển thị rõ tổng thời gian đi lóc cóc)
                        sprintf(penalty_text, "Huy chuyen! Di bo ra-vao: 6p");
                        penalty_x = stations[ga_hien_tai].drawX - 70;
                        penalty_y = stations[ga_hien_tai].drawY - 30;
                    }
                    
                    // 3. CHẠY THUẬT TOÁN TÌM ĐƯỜNG MỚI
                    if (is_JR_ON) ApplyJRPass_OnlyJR(); 
                    DijkstraAdvanced(start_cuu_ho, gaDen, criteria, start_time_cuu_ho, start_line_cuu_ho); 
                    
                    if (dist_time[gaDen] == INF && is_JR_ON) {
                        RestoreGraph(); 
                        CloseEdge(ga_hien_tai, ga_tiep);
                        if (line_in == line_out) CloseEdge(ga_truoc, ga_hien_tai);
                        ApplyJRPass_Mix();
                        DijkstraAdvanced(start_cuu_ho, gaDen, criteria, start_time_cuu_ho, start_line_cuu_ho);
                    }
                    
                    // 4. NỐI MẢNG NẾU TÌM ĐƯỢC ĐƯỜNG
                    if (dist_time[gaDen] != INF) {
                        int new_path[MAX_NODES]; int new_count = 0; int curr = gaDen;
                        while (curr != -1) { new_path[new_count++] = curr; curr = prev_node[curr]; }
                        
                        for (int k = 0; k < new_count; k++) { 
                            trace_path[cut_index + k] = new_path[new_count - 1 - k]; 
                        }
                        path_length = cut_index + new_count;
                        has_disruption = true; 
                        
                        // Cập nhật lại mảng walk/wait times cho UI vẽ chữ (Tái tạo y hệt lúc Tìm Đường)
                        int current_time_accumulator = 0; 
                        for (int h = 0; h < path_length - 1; h++) {
                            int u = trace_path[h]; int v = trace_path[h+1];
                            int line_truoc = (h == 0) ? -1 : map[trace_path[h-1]][u].line_ID;
                            int line_hien_tai = map[u][v].line_ID;

                            path_wait_times[h] = 0; path_walk_times[h] = 0;

                            if (line_hien_tai == 0) {
                                path_walk_times[h] = map[u][v].time; 
                                current_time_accumulator += map[u][v].time;
                            } else {
                                int walk_trong_ga = 0;
                                if (line_truoc != -1 && line_hien_tai != line_truoc) walk_trong_ga = 3; 
                                
                                int thoi_diem_san_sang = current_time_accumulator + walk_trong_ga;
                                int tan_suat = GetLineFrequency(line_hien_tai); 
                                int cho_tau = 0;
                                if (tan_suat > 0) cho_tau = (tan_suat - (thoi_diem_san_sang % tan_suat)) % tan_suat;

                                path_walk_times[h] = walk_trong_ga;
                                path_wait_times[h] = cho_tau;
                                current_time_accumulator += (walk_trong_ga + cho_tau + map[u][v].time);
                            }
                        }
                    }
                    break; // Xử lý xong 1 click thì thoát vòng lặp
                }
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE); 
            
            DrawRectangle(20, 20, 250, 170, Fade(LIGHTGRAY, 0.3f));
            DrawText("CHU GIAI TUYEN:", 30, 30, 20, BLACK);
            DrawLineEx((Vector2){30, 60}, (Vector2){70, 60}, 5.0f, C_JR);     DrawText("JR Yamanote", 80, 50, 20, DARKGRAY);
            DrawLineEx((Vector2){30, 85}, (Vector2){70, 85}, 5.0f, C_GINZA);  DrawText("Ginza Line", 80, 75, 20, DARKGRAY);
            DrawLineEx((Vector2){30, 110}, (Vector2){70, 110}, 5.0f, C_MARU); DrawText("Marunouchi Line", 80, 100, 20, DARKGRAY);
            DrawLineEx((Vector2){30, 135}, (Vector2){70, 135}, 5.0f, C_HIBIYA);DrawText("Hibiya Line", 80, 125, 20, DARKGRAY);
            DrawLineEx((Vector2){30, 160}, (Vector2){70, 160}, 5.0f, C_FUKU); DrawText("Fukutoshin Line", 80, 150, 20, DARKGRAY);

            for (int i = 0; i < total_edges; i++) {
                int gA = map_edges[i].gaA; int gB = map_edges[i].gaB;
                Vector2 start = {stations[gA].drawX + map_edges[i].offsetX, stations[gA].drawY + map_edges[i].offsetY};
                Vector2 end = {stations[gB].drawX + map_edges[i].offsetX, stations[gB].drawY + map_edges[i].offsetY};
                if (map_edges[i].curveOffsetX == 0 && map_edges[i].curveOffsetY == 0) {
                    DrawLineEx(start, end, 4.0f, map_edges[i].color); 
                } else {
                    Vector2 control = { (start.x + end.x)/2.0f + map_edges[i].curveOffsetX, (start.y + end.y)/2.0f + map_edges[i].curveOffsetY };
                    DrawLineEx(start, control, 4.0f, map_edges[i].color);
                    DrawLineEx(control, end, 4.0f, map_edges[i].color);
                    DrawCircle(control.x, control.y, 2.0f, map_edges[i].color);
                }
            }

            if (has_result) {
                for (int i = 0; i < path_length - 1; i++) {
                    int gaU = trace_path[i]; int gaV = trace_path[i+1];
                    int c_offsetX = 0; int c_offsetY = 0; Color routeColor = BLACK; 
                    for (int j = 0; j < total_edges; j++) {
                        if ((map_edges[j].gaA == gaU && map_edges[j].gaB == gaV) || (map_edges[j].gaA == gaV && map_edges[j].gaB == gaU)) {
                            c_offsetX = map_edges[j].curveOffsetX; c_offsetY = map_edges[j].curveOffsetY; routeColor = map_edges[j].color; break;
                        }
                    }
                    Vector2 start = {stations[gaU].drawX, stations[gaU].drawY}; Vector2 end = {stations[gaV].drawX, stations[gaV].drawY};
                    
                    if (c_offsetX == 0 && c_offsetY == 0) { 
                        DrawLineEx(start, end, 14.0f, BLACK); DrawLineEx(start, end, 6.0f, routeColor);   
                    } else {             
                        Vector2 control = { (start.x + end.x)/2.0f + c_offsetX, (start.y + end.y)/2.0f + c_offsetY };
                        DrawLineEx(start, control, 14.0f, BLACK); DrawLineEx(control, end, 14.0f, BLACK); DrawCircle(control.x, control.y, 7.0f, BLACK);
                        DrawLineEx(start, control, 6.0f, routeColor); DrawLineEx(control, end, 6.0f, routeColor); DrawCircle(control.x, control.y, 3.0f, routeColor);
                    }

                    // ========================================================
                    // VỊ TRÍ 5: VẼ CHỮ BÁM DỌC TUYẾN (Khối 2 dòng - Chống đè tuyệt đối)
                    // ========================================================
                    
                    float dx = stations[gaV].drawX - stations[gaU].drawX;
                    float dy = stations[gaV].drawY - stations[gaU].drawY;
                    float length = sqrtf(dx*dx + dy*dy);
                    if (length == 0) length = 1;
                    
                    float dirX = dx / length; // Hướng dọc theo tuyến
                    float dirY = dy / length; 
                    float normX = -dirY;      // Hướng dạt ra lề
                    float normY = dirX;       

                    // 1. Hiển thị đi bộ trung chuyển NGOÀI TRỜI (Màu PURPLE)
                    if (map[gaU][gaV].line_ID == 0) {
                        int midX = (stations[gaU].drawX + stations[gaV].drawX) / 2;
                        int midY = (stations[gaU].drawY + stations[gaV].drawY) / 2;
                        
                        const char* walk_text = TextFormat("Di bo: %dp", map[gaU][gaV].time);
                        int wWidth = MeasureText(walk_text, 14);
                        
                        int textX = midX + normX * 14 - wWidth / 2;
                        int textY = midY + normY * 14 - 7;
                        
                        DrawText(walk_text, textX, textY, 14, PURPLE);
                    } 
                    
                    // 2 & 3. Hiển thị Đổi line và Chờ tàu (Xếp thành khối 2 dòng gọn gàng)
                    if ((path_walk_times[i] > 0 && map[gaU][gaV].line_ID != 0) || path_wait_times[i] > 0) {
                        
                        // Tìm một điểm "neo" cách ga 45px và dạt ra lề đường ray 20px
                        int anchorX = stations[gaU].drawX + dirX * 45 + normX * 20;
                        int anchorY = stations[gaU].drawY + dirY * 45 + normY * 20;
                        
                        int currentY = anchorY - 7; // Tọa độ Y bắt đầu vẽ chữ
                        
                        // Vẽ dòng 1: Đổi line (nếu có)
                        if (path_walk_times[i] > 0 && map[gaU][gaV].line_ID != 0) {
                            const char* doi_text = TextFormat("Doi line: %dp", path_walk_times[i]);
                            int tWidth = MeasureText(doi_text, 14);
                            DrawText(doi_text, anchorX - tWidth / 2, currentY, 14, MAGENTA);
                            
                            // Tự động dời tọa độ Y xuống 16 pixel (tạo hiệu ứng Enter xuống dòng)
                            currentY += 16; 
                        }

                        // Vẽ dòng 2: Chờ tàu (nếu có)
                        if (path_wait_times[i] > 0) {
                            const char* cho_text = TextFormat("Cho tau: %dp", path_wait_times[i]);
                            int tWidth = MeasureText(cho_text, 14);
                            DrawText(cho_text, anchorX - tWidth / 2, currentY, 14, ORANGE);
                        }
                    }
                    // ========================================================
                }
            }
            
            for (int i = 0; i < 22; i++) {
                DrawCircle(stations[i].drawX, stations[i].drawY, 8, BLACK);
                DrawCircle(stations[i].drawX, stations[i].drawY, 5, WHITE); 
                
                if (i == gaDi) DrawCircle(stations[i].drawX, stations[i].drawY, 12, GREEN);
                if (i == gaDen) DrawCircle(stations[i].drawX, stations[i].drawY, 12, RED);

                DrawText(stations[i].name, stations[i].drawX + stations[i].textOffsetX, stations[i].drawY + stations[i].textOffsetY, 20, DARKGRAY);
            }

            // ==========================================
            // 1. VẼ MENU CHỌN THÔNG SỐ (Nền Xanh nhạt)
            // ==========================================
            int menuX = 1150;
            int menuWidth = 330;
            
            DrawRectangle(menuX, 20, menuWidth, 245, Fade(SKYBLUE, 0.25f)); // Đẩy chiều cao nền xanh lên 245
            DrawText("BANG DIEU KHIEN (UI)", menuX + 55, 30, 20, DARKBLUE);

            DrawText(TextFormat("Ga Di  : %s", gaDi != -1 ? stations[gaDi].name : "[Click tren map]"), menuX + 15, 65, 20, DARKGRAY);
            DrawText(TextFormat("Ga Den : %s", gaDen != -1 ? stations[gaDen].name : "[Click tren map]"), menuX + 15, 90, 20, DARKGRAY);

            DrawRectangleRec(btn_JR, is_JR_ON ? GREEN : LIGHTGRAY);
            DrawText(TextFormat("JR PASS: %s", is_JR_ON ? "ON" : "OFF"), btn_JR.x + 8, btn_JR.y + 6, 18, BLACK);

            DrawRectangleRec(btn_TieuChi, LIGHTGRAY);
            const char* str_tc = (criteria == 1) ? "Nhanh Nhat" : (criteria == 2) ? "Re Nhat" : "Can Bang";
            DrawText(TextFormat("Tieu Chi: %s", str_tc), btn_TieuChi.x + 8, btn_TieuChi.y + 6, 18, BLACK);

            DrawRectangleRec(btn_TimDuong, BLUE); DrawText("TIM DUONG", btn_TimDuong.x + 15, btn_TimDuong.y + 10, 18, WHITE);
            DrawRectangleRec(btn_Reset, RED);     DrawText("RESET", btn_Reset.x + 15, btn_Reset.y + 10, 18, WHITE);

            // ==========================================
            // 2. VẼ BẢNG KẾT QUẢ TÁCH BIỆT (Nền Xám)
            // ==========================================
            if (has_result) {
                DrawRectangle(menuX, 280, menuWidth, 255, Fade(LIGHTGRAY, 0.5f)); 
                DrawText("KET QUA TIM DUONG", menuX + 65, 295, 20, DARKBLUE);

                if (dist_time[gaDen] >= 999999) {
                    // --- CASE 1: BỊ CÔ LẬP ---
                    DrawText("--- KHONG THE DEN DICH ---", menuX + 25, 350, 20, RED);
                    DrawText("Vui long xem bang loi ben duoi!", menuX + 15, 380, 18, DARKGRAY);
                } 
                else if (has_disruption) {
                    // --- CASE 2: CÓ SỰ CỐ -> SO SÁNH SỰ CỐ ---
                    DrawText("SO SANH: GOC vs SAU SU CO", menuX + 35, 330, 18, MAROON);
                    
                    DrawText("Lan 1", menuX + 160, 360, 18, DARKGREEN);
                    DrawText("Lan 2", menuX + 250, 360, 18, RED);
                    DrawLine(menuX + 10, 385, menuX + menuWidth - 10, 385, GRAY);

                    int new_time = dist_time[gaDen];
                    int new_cost = history_cost + dist_cost[gaDen];
                    int new_pure_time = history_pure_time;
                    for (int i = 0; i < path_length - 1; i++) new_pure_time += map[trace_path[i]][trace_path[i+1]].time;
                    int new_penalty = new_time - new_pure_time;

                    DrawText("Thoi gian:", menuX + 10, 400, 18, BLACK);
                    DrawText(TextFormat("%d p", normal_time), menuX + 160, 400, 18, DARKGREEN);
                    DrawText(TextFormat("%d p", new_time), menuX + 250, 400, 18, RED);

                    DrawText("Chi phi:", menuX + 10, 435, 18, BLACK);
                    DrawText(TextFormat("%d Y", normal_cost), menuX + 160, 435, 18, DARKGREEN);
                    DrawText(TextFormat("%d Y", new_cost), menuX + 250, 435, 18, RED);

                    DrawText("Trung chuyen:", menuX + 10, 470, 18, BLACK);
                    DrawText(TextFormat("%d p", normal_penalty), menuX + 160, 470, 18, DARKGREEN);
                    DrawText(TextFormat("%d p", new_penalty), menuX + 250, 470, 18, RED);
                    
                    // LÝ DO PHẠT TRỰC TIẾP (Thay thế cho dòng Phát sinh)
                    DrawText("Ly do:", menuX + 10, 505, 18, MAROON);
                    DrawText(penalty_text, menuX + 70, 505, 16, RED); 
                }
                else if (is_comparing_criteria) {
                    // --- CASE 3: KHÔNG SỰ CỐ, NHƯNG ĐỔI TIÊU CHÍ -> SO SÁNH TIÊU CHÍ ---
                    DrawText("SO SANH: THAY DOI TIEU CHI", menuX + 35, 330, 18, DARKBLUE);
                    
                    DrawText("T.Chi Cu", menuX + 140, 360, 18, GRAY);
                    DrawText("T.Chi Moi", menuX + 240, 360, 18, DARKGREEN);
                    DrawLine(menuX + 10, 385, menuX + menuWidth - 10, 385, GRAY);

                    DrawText("Thoi gian:", menuX + 10, 400, 18, BLACK);
                    DrawText(TextFormat("%d p", last_time), menuX + 155, 400, 18, GRAY);
                    DrawText(TextFormat("%d p", normal_time), menuX + 255, 400, 18, (normal_time < last_time) ? DARKGREEN : ((normal_time > last_time) ? RED : BLACK));

                    DrawText("Chi phi:", menuX + 10, 435, 18, BLACK);
                    DrawText(TextFormat("%d Y", last_cost), menuX + 155, 435, 18, GRAY);
                    DrawText(TextFormat("%d Y", normal_cost), menuX + 255, 435, 18, (normal_cost < last_cost) ? DARKGREEN : ((normal_cost > last_cost) ? RED : BLACK));

                    DrawText("Trung chuyen:", menuX + 10, 470, 18, BLACK);
                    DrawText(TextFormat("%d p", last_penalty), menuX + 155, 470, 18, GRAY);
                    DrawText(TextFormat("%d p", normal_penalty), menuX + 255, 470, 18, BLACK);
                    
                    DrawText("(* Chuot phai de gia lap su co)", menuX + 15, 510, 15, GRAY);
                }
                else {
                    // --- CASE 4: LẦN 1 BÌNH THƯỜNG ---
                    DrawText("LO TRINH GOC (LAN 1)", menuX + 65, 335, 18, DARKGREEN);
                    DrawText(TextFormat("Thoi gian tong : %d phut", normal_time), menuX + 15, 380, 20, BLACK);
                    DrawText(TextFormat(" - T.Gian di tren tau : %d phut", normal_time - normal_penalty), menuX + 15, 410, 18, DARKGRAY);
                    DrawText(TextFormat(" - T.Gian trung chuyen: %d phut", normal_penalty), menuX + 15, 440, 18, DARKGRAY);
                    DrawText(TextFormat("Tong chi phi   : %d Yen", normal_cost), menuX + 15, 485, 20, RED);
                    
                    DrawText("(* Chuot phai vao ga tren duong", menuX + 15, 515, 15, GRAY);
                    DrawText("   de gia lap su co tuyen truoc mat)", menuX + 15, 530, 15, GRAY);
                }
            }

            // ==========================================
            // 3. VẼ BOX BÁO LỖI BIỆT LẬP (Nền Đỏ)
            // ==========================================
            if (has_result && dist_time[gaDen] >= 999999) {
                DrawRectangle(menuX, 550, menuWidth, 90, Fade(RED, 0.85f));
                DrawRectangleLines(menuX, 550, menuWidth, 90, MAROON); 
                
                DrawText("LOI: MANG LUOI BI CO LAP!", menuX + 20, 565, 18, WHITE);
                DrawText("Khong the tim thay lo trinh den dich", menuX + 20, 592, 16, RAYWHITE);
                DrawText(TextFormat("do ga %s gap su co.", stations[broken_station].name), menuX + 20, 612, 16, RAYWHITE);
            }

            // ==========================================
            // 4. VẼ CHỮ X ĐỎ VÀ CHỮ PHẠT TRỰC TIẾP LÊN MAP
            // ==========================================
            if (has_disruption && broken_station != -1) {
                DrawCircle(stations[broken_station].drawX, stations[broken_station].drawY, 15, Fade(RED, 0.7f));
                DrawText("X", stations[broken_station].drawX - 6, stations[broken_station].drawY - 10, 24, WHITE);
                
                int text_width = MeasureText(penalty_text, 16);
                DrawRectangle(penalty_x - 5, penalty_y - 2, text_width + 10, 20, Fade(RAYWHITE, 0.8f));
                DrawText(penalty_text, penalty_x, penalty_y, 16, RED);
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}