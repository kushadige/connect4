#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <Windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define GAME_BOARD "tahta.txt"
#define GAME_RECORDS "hamle.txt"

// ################ CURSOR CONTROLS ################
#define cursorup(x) printf("\033[%dA", (x))
#define cursordown(x) printf("\033[%dB", (x))
#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))
#define savecursor() printf("\033[s")
#define restorecursor() printf("\033[u")
#define board_left() printf("\033[4D")
#define board_right() printf("\033[4C")
#define board_up() printf("\033[2A")
#define board_down() printf("\033[2B")

#define KEY_ESCAPE  27
#define KEY_ENTER   13
#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77

int wherex();
int wherey();

// ################ TYPE DEFINITIONS ################
typedef enum {
    PLAYER1,
    PLAYER2
} player;
typedef struct {
    int game_id;
    char record_time[30];
    char active_board[21][40];
} Game;

// ################ UTIL FUNCTIONS ################
int compare_string(char *first, char *second);
void clear_screen();
void ansi_enable();
char* get_current_time();
Game* read_data(char* filename, int* total);
bool write_data(char* filename, Game* data, int total);
bool update_data(char* filename, Game* data, int pos);
bool append_data(char* filename, Game* data, int total);
bool delete_data(char* filename, Game* data, int total); // -------------------------<<<<<<<< not defined

// ################ GAME FUNCTIONS ################
void main_menu();
void pause_menu();
void load_game(int* control);
void print_game_board(int continue_game);
int slide_the_ball(player x, int col, char active_board[21][40]);
void save_game();

Game active_game;

// ################ MAIN ################
int main() {
    ansi_enable();
    clear_screen();
    main_menu();
}

// ################ GAME FUNCTION DECLERATIONS ################
void main_menu() {
    printf(
        "+------------------+\n"
        "|  +  CONNECT4  +  |\n"
        "+------------------+\n"
    );
    printf(
        "\n"
        "+ Start New Game\n"
        "- Load Game\n"
        "- Exit\n"
    );
    cursorup(3);

    int cursor_state = 0;
    int key_input = 0;
    int selected_option;

    while(1 && key_input != -1) {
        switch((key_input = getch())) {
            case KEY_UP:
                if(cursor_state > 0) {
                    printf("-");
                    cursorup(1);
                    cursorbackward(1);
                    printf("+");
                    cursorbackward(1);
                    cursor_state -= 1;
                }
                break;
            case KEY_DOWN:
                if(cursor_state < 2) {
                    printf("-");
                    cursordown(1);
                    cursorbackward(1);
                    printf("+");
                    cursorbackward(1);
                    cursor_state += 1;
                }
                break;
            case KEY_ENTER:
                key_input = -1;
                selected_option = wherey();
                break;
            default:
                break;
        }
    }

    clear_screen();
    switch(selected_option) {
        case 5:         // start_new_game
            print_game_board(0);
            break;
        case 6:         // load_game
            int control = 1;
            load_game(&control);

            clear_screen();
            if(control) {
                print_game_board(1);
            } else {
                main_menu();
            }
            break;
        case 7:         // exit
            exit(1);
            break;
        default:
            break;
    }
}
void pause_menu() {
    printf(
        "+------------------+\n"
        "|  +  CONNECT4  +  |\n"
        "+------------------+\n"
    );
    printf(
        "\n"
        "+ Continue\n"
        "- Save Game\n"
        "- Main Menu\n"
        "- Exit\n"
    );
    cursorup(4);

    int cursor_state = 0;
    int key_input = 0;
    int selected_option;

    while(1 && key_input != -1) {
        switch((key_input = getch())) {
            case KEY_UP:
                if(cursor_state > 0) {
                    printf("-");
                    cursorup(1);
                    cursorbackward(1);
                    printf("+");
                    cursorbackward(1);
                    cursor_state -= 1;
                }
                break;
            case KEY_DOWN:
                if(cursor_state < 3) {
                    printf("-");
                    cursordown(1);
                    cursorbackward(1);
                    printf("+");
                    cursorbackward(1);
                    cursor_state += 1;
                }
                break;
            case KEY_ENTER:
                key_input = -1;
                selected_option = wherey();
                break;
            default:
                break;
        }
    }

    clear_screen();
    switch(selected_option) {
        case 5:         // continue_game
            print_game_board(1);
            break;
        case 6:         // save_game
            save_game();
            clear_screen();
            pause_menu();
            break;
        case 7:         // main_menu
            main_menu();
            break;
        case 8:         // exit
            exit(1);
            break;
        default:
            exit(1);
            break;
    }
}
void print_game_board(int continue_game) {
    if(!continue_game) {

        FILE *fp = fopen(GAME_BOARD, "r");

        if(!fp) {
            printf(GAME_BOARD, " bulunamadi.\n");
            return;
        }

        rewind(fp);

        // init board to array
        char buffer;
        int row_iterator = 0, col_iterator = 0;
        while((buffer = fgetc(fp)) != EOF) {
            printf("%c", buffer);
            active_game.active_board[row_iterator][col_iterator] = buffer;
            col_iterator++;

            if(buffer == '\n') {
                row_iterator++;
                col_iterator = 0;
            }
        }

        fclose(fp);
    } else {
        for(int i = 0; i < 21; i++) {
            for(int j = 0; j < 40; j++) {
                printf("%c", active_game.active_board[i][j]);
            }
        }
    }

    printf("\n\n...press esc to open menu");

    int cursor_state_x = 5;
    int key_input = 0;
    int selected_option;

    gotoxy(5, 0);
    printf("X");
    cursorbackward(1);

    int current_player = 0;
    int is_the_move_valid = 0;

    while(1 && key_input != -1) {

        switch((key_input = getch())) {
            case KEY_LEFT:
                if(cursor_state_x > 5) {
                    printf(" ");
                    cursorbackward(1);
                    board_left();
                    if(current_player == 0) {
                        printf("X");
                    } else {
                        printf("Y");
                    }
                    cursorbackward(1);
                    cursor_state_x -= 4;
                }
                break;
            case KEY_RIGHT:
                if(cursor_state_x < 34) {
                    printf(" ");
                    cursorbackward(1);
                    board_right();
                    if(current_player == 0) {
                        printf("X");
                    } else {
                        printf("Y");
                    }
                    cursorbackward(1);
                    cursor_state_x += 4;
                }
                break;
            case KEY_ESCAPE:
                key_input = -1;
                clear_screen();
                pause_menu();
                break;
            case KEY_ENTER:
                if(current_player == 0) {
                    is_the_move_valid = slide_the_ball(PLAYER1, cursor_state_x - 1, active_game.active_board);        // move down player1 red ball
                } else {
                    is_the_move_valid = slide_the_ball(PLAYER2, cursor_state_x - 1, active_game.active_board);        // move down player2 yellow ball 
                }

                if(is_the_move_valid) {

                    current_player = current_player == 0 ? 1 : 0;

                    if(current_player == 0) {
                        gotoxy(5, 0);
                        cursor_state_x = 5; 
                        printf("X");
                    } else {
                        gotoxy(37, 0);
                        cursor_state_x = 37; 
                        printf("Y");
                    }

                    cursorbackward(1);
                }

                break;
            default:
                break;
        }
    }

}
int slide_the_ball(player x, int col, char active_board[21][40]) {

    // <- active_board ball coords (row, col) -> 
    //  1.col    (0, 4) - (2, 4) - (4, 4) - ....
    //  2.col    (0, 8) - (2, 8) - (4, 8) - ....
    //  3.col    (0, 12) - (2, 12) - (4, 12) - ....             

    char active_ball = 'X'; 
    if(x == PLAYER2) {
        active_ball = 'Y';
    }

    int i = 0;
    while(active_board[2 + i*2][col] == ' ') {
        Sleep(25);

        printf(" ");
        cursorbackward(1);
        board_down();
        printf("%c", active_ball);
        cursorbackward(1);
        i++;
    }

    if(i != 0) {
        active_board[2 + (i-1)*2][col] = active_ball;
        return 1;
    } else {
        return 0;
    }
}
void save_game() {
    int total = 0;
    Game* file_data;
    
    file_data = read_data("hamle.bin", &total);

    if(file_data == NULL) {
        for(int i = 0; i < 4; i++) {
            printf(
                "    +------------------------------+\n"
                "    |                              |\n"
                "    |          empty slot          |\n"
                "    |                              |\n"
                "    +------------------------------+\n"
            );
        }
    } else {
        for(int i = 0; i < 4; i++) {
            bool intersect_control = false;
            int j;
            for(j = 0; j < total; j++) {
                if(file_data[j].game_id == i) {
                    intersect_control = true;
                    break;
                }
            }

            if(intersect_control) {
                printf(
                    "    +------------------------------+\n"
                    "    |game id: %d|\n"
                    "    |%s|\n"
                    "    |                              |\n"
                    "    +------------------------------+\n", file_data[j].game_id, file_data[j].record_time
                );
            } else {
                printf(
                    "    +------------------------------+\n"
                    "    |                              |\n"
                    "    |          empty slot          |\n"
                    "    |                              |\n"
                    "    +------------------------------+\n"
                );
            }
        }

        free(file_data);
    }

    printf("\n\n...press esc to cancel");

    gotoxy(0, 3);
    printf("->");
    cursorbackward(2);

    int cursor_state = 0;
    int key_input = 0;

    while(1 && key_input != -1) {
        switch((key_input = getch())) {
            case KEY_UP:
                if(cursor_state > 0) {
                    printf("  ");
                    cursorup(5);
                    cursorbackward(2);
                    printf("->");
                    cursorbackward(2);
                    cursor_state -= 1;
                }
                break;
            case KEY_DOWN:
                if(cursor_state < 3) {
                    printf("  ");
                    cursordown(5);
                    cursorbackward(2);
                    printf("->");
                    cursorbackward(2);
                    cursor_state += 1;
                }
                break;
            case KEY_ESCAPE:
                key_input = -1;
                break;
            case KEY_ENTER:
                key_input = -1;

                clear_screen();
                printf("\nsaving...");
                Sleep(150);

                file_data = read_data("hamle.bin", &total);

                Game rec_game;
                rec_game.game_id = cursor_state;
                
                char* foo = get_current_time();;
                foo[strlen(foo) - 1] = 0;

                strcpy(rec_game.record_time, foo);

                for(int i = 0; i < 21; i++) {
                    strcpy(rec_game.active_board[i], active_game.active_board[i]);
                }

                if(file_data == NULL) {
                    if(write_data("hamle.bin", &rec_game, 1)) {
                        printf("Write data OK.\n");
                        Sleep(1000);
                    } else {
                        printf("Error writing to file.\n");
                        Sleep(1000);
                    }
                } else {
                    // check overwrite
                    int cursor_state_position_in_file;
                    int is_overwrite = 0;
                    for(int i = 0; i < total; i++) {
                        if(cursor_state == file_data[i].game_id) {
                            is_overwrite = 1;
                            cursor_state_position_in_file = i;
                            break;
                        }
                    }

                    if(is_overwrite) {
                        // update record
                        if(update_data("hamle.bin", &rec_game, cursor_state_position_in_file)) {
                            printf("Update data OK.\n");
                            Sleep(1000);
                        } else {
                            printf("Error updating to file.\n");
                            Sleep(1000);
                        }
                    } else {
                        // append record
                        if(append_data("hamle.bin", &rec_game, total + 1)) {
                            printf("Append data OK.\n");
                            Sleep(1000);
                        } else {
                            printf("Error appending to file.\n");
                            Sleep(1000);
                        }
                    }
                }

                free(file_data);
                break;
            default:
                break;
        }
    }
}
void load_game(int* control) {
    int total = 0;
    Game* file_data;
    
    file_data = read_data("hamle.bin", &total);

    if(file_data == NULL) {
        printf(
            "    +--------------------------------------------+\n"
            "    |                                            |\n"
            "    |          there is no saved game..          |\n"
            "    |                                            |\n"
            "    +--------------------------------------------+\n"
        );
        *control = 0;
    } else {
        for(int i = 0; i < total; i++) {
            printf(
                "    +------------------------------+\n"
                "    |game id: %d|\n"
                "    |%s|\n"
                "    |                              |\n"
                "    +------------------------------+\n", file_data[i].game_id, file_data[i].record_time
            );
        }

        free(file_data);
    }

    printf("\n\n...press esc to cancel");

    gotoxy(0, 3);
    printf("->");
    cursorbackward(2);

    int cursor_state = 0;
    int key_input = 0;

    while(1 && key_input != -1) {
        switch((key_input = getch())) {
            case KEY_UP:
                if(cursor_state > 0) {
                    printf("  ");
                    cursorup(5);
                    cursorbackward(2);
                    printf("->");
                    cursorbackward(2);
                    cursor_state -= 1;
                }
                break;
            case KEY_DOWN:
                if(cursor_state < total - 1) {
                    printf("  ");
                    cursordown(5);
                    cursorbackward(2);
                    printf("->");
                    cursorbackward(2);
                    cursor_state += 1;
                }
                break;
            case KEY_ESCAPE:
                *control = 0;
                key_input = -1;
                break;
            case KEY_ENTER:
                if(*control == 0) {
                    break;
                }

                key_input = -1;

                clear_screen();
                printf("\nloading...");
                Sleep(250);

                file_data = read_data("hamle.bin", &total);

                // copy game board from file to active_game board
                for(int i = 0; i < 21; i++) {
                    strcpy(active_game.active_board[i], file_data[cursor_state].active_board[i]);
                }

                *control = 1;
                free(file_data);
                break;
            default:
                break;
        }
    }
}

// ################ UTIL FUNCTION DECLERATIONS ################
int compare_string(char *first, char *second) {
    // string.h kütüphanesinden strcmp() fonksiyonu da kullanılabilir.

    while (*first == *second) {
        if (*first == '\0' || *second == '\0') {
            break;
        }
        first++;
        second++;
    }

    if (*first == '\0' && *second == '\0') return 0;
    else return -1;
}
void clear_screen() {
    system("cls");
    printf("\033[2J\033[H"); // console clear ve cursor'u home adresine alma işlemi
}
void ansi_enable() {
    // Komut isteminde ANSI Kodlarını çalıştırmak için SetConsoleMode() fonksiyonu ile TERMINAL_PROCESSING'i açıyoruz.
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
char* get_current_time() {
    time_t rawtime = time(NULL);
    struct tm* timeinfo = localtime(&rawtime);

    char date[24];
    strftime(date, sizeof(date), "%c", timeinfo);

    return asctime(timeinfo);
}
Game* read_data(char* filename, int* total) {
    FILE *fp = fopen(filename, "rb");

    if(fp == NULL) return false;

    // Read total record number from file
    if(fread(total, sizeof(int), 1, fp) != 1) {
        return NULL;
    }

    Game* data = malloc(sizeof(Game) * *total);

    if(fread(data, sizeof(Game), *total, fp) != *total) {
        // Prevent Memory Leak with free before returning NULL.
        free(data);
        return NULL;
    }

    if(fclose(fp) == EOF) {
        free(data);
        return NULL;
    };

    return data;
}
bool write_data(char* filename, Game* data, int total) {
    FILE *fp;

    fp = fopen(filename, "wb");

    if(fp == NULL) return false;

    if(fwrite(&total, sizeof(int), 1, fp) != 1) {
        return false;
    }

    if(fwrite(data, sizeof(Game), total, fp) != total) {
        return false;
    }

    if(fclose(fp) == EOF) return false;

    return true;
}
bool update_data(char* filename, Game* data, int pos) {
    FILE *fp;

    fp = fopen(filename,"rb+");

    if(fp == NULL) return false;

    fseek(fp, sizeof(int) + (sizeof(Game) * pos), SEEK_SET);

    if(fwrite(data, sizeof(Game), 1, fp) != 1) {
        return false;
    }

    if(fclose(fp) == EOF) return false;

    return true;
}
bool append_data(char* filename, Game* data, int total) {
    FILE *fp;

    fp = fopen(filename,"rb+");
    fseek(fp, 0, SEEK_SET);

    if(fwrite(&total, sizeof(int), 1, fp) != 1) {
        return false;
    }

    if(fclose(fp) == EOF) return false;

    fp = fopen(filename, "ab");

    if(fp == NULL) return false;

    if(fwrite(data, sizeof(Game), 1, fp) != 1) {
        return false;
    }

    if(fclose(fp) == EOF) return false;

    return true;
}

// ################ CURSOR STATE FUNCTIONS ################
int wherex() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;    
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);    
    return csbi.dwCursorPosition.X;
}
int wherey() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;    
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);    
    return csbi.dwCursorPosition.Y;
}

// X' ve Y'lere yanıp sönme efekti

// Load game'de aktif oyuncu bilgisinin doğru getirilmesi