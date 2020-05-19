#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <thread>

//display size 73 * 30

const int MAXR = 20;
const int MAXC = 18;

using namespace std;

typedef char grid[MAXR+1][MAXC+1];

grid board = {
    '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
    '#',' ',' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',' ','#',
    '#',' ','#','#',' ','#','#','#',' ','#',' ','#','#','#',' ','#','#',' ','#',
    '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',
    '#',' ','#','#',' ','#',' ','#','#','#','#','#',' ','#',' ','#','#',' ','#',
    '#',' ',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ',' ','#',
    '#','#','#','#',' ','#','#','#',' ','#',' ','#','#','#',' ','#','#','#','#',
    ' ',' ',' ','#',' ','#',' ',' ',' ',' ',' ',' ',' ','#',' ','#',' ',' ',' ',
    '#','#','#','#',' ','#',' ','#',' ',' ',' ','#',' ','#',' ','#','#','#','#',
    ' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',
    '#','#','#','#',' ','#',' ','#','#','#','#','#',' ','#',' ','#','#','#','#',
    ' ',' ',' ','#',' ','#',' ',' ',' ',' ',' ',' ',' ','#',' ','#',' ',' ',' ',
    '#','#','#','#',' ','#',' ','#','#','#','#','#',' ','#',' ','#','#','#','#',
    '#',' ',' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',' ','#',
    '#',' ','#','#',' ','#','#','#',' ','#',' ','#','#','#',' ','#','#',' ','#',
    '#',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',' ',' ','#',
    '#','#',' ','#',' ','#',' ','#','#','#','#','#',' ','#',' ','#',' ','#','#',
    '#',' ',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ',' ','#',
    '#',' ','#','#','#','#','#','#',' ','#',' ','#','#','#','#','#','#',' ','#',
    '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',
    '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
};

enum direction {r,l,u,d};

struct mov_elem {
    //defining the current position
    int i;
    int j;
    //defining direction
    enum direction dir;
    char type;
};

struct static_elem {
    int i;
    int j;
    char type;
};

struct static_elem fruit = {0,0,'%'};
struct static_elem pellet[4];
struct mov_elem pacman = {15,9,r,'<'};
struct mov_elem ghost[4];

int life = 3;

void initialise() {
    ghost[1].i = ghost[0].i = ghost[2].i = 9;
    ghost[3].i = 8;
    ghost[1].j = ghost[3].j = 9;
    ghost[0].j = 8;
    ghost[2].j = 10;
    for (int i=0; i<4; i++) {
        // ghost[i].i = rand()%2+8;
        // ghost[i].j = rand()%2+9;
        ghost[i].type = 'H';
        ghost[i].dir = u;
    }
    pacman.i = 15;
    pacman.j = 9;
    pacman.dir = r;
    pacman.type = '<';
}

void set_pellets() {
    pellet[0].i = 2;
    pellet[0].j = 1;
    pellet[1].i = 2;
    pellet[1].j = 17;
    pellet[2].i = 15;
    pellet[2].j = 1;
    pellet[3].i = 15;
    pellet[3].j = 17;
    for (int k=0; k<4; k++) {
        pellet[k].type = 'o';
        board[pellet[k].i][pellet[k].j] = pellet[k].type;
    }
}

int flag              = 0;
int score             = 0;
int speed             = 0;
const int microsec    = 110000;
int power_up          = 0;
int pow_dur           = 0; // duration of the power up time in terms of pow_dur is DUR
const int DUR         = 50; // i.e 5 * 110000e-6 = 5.5 seconds

void board_print() {
    system("clear");
    while(flag == 0) {
        fruit.i = rand()%18 + 1;
        fruit.j = rand()%16 + 1;
        fruit.type = '%';
        if(board[fruit.i][fruit.j]==' ' && !(fruit.i == 7 && (fruit.j < 3 || fruit.j > 15)) && !(fruit.i == 11 && (fruit.j < 3 || fruit.j > 15))) {
            board[fruit.i][fruit.j] = fruit.type;
            flag = 1;
        }
    }
    for(int i1 = 0; i1<21; i1++) {
        cout<<"\t";
        for(int j1 = 0; j1<19; j1++) {
            if((i1==0||i1==MAXR||j1==0||j1==MAXC) && (i1!=9 && i1!=7 && i1!=11))
                cout<<"\e[34m\e[44m";
            else if(board[i1][j1]=='#')
                cout<<"\e[34m\e[44m";
            else if(i1==pacman.i && j1==pacman.j)
                cout<<"\e[33m";
            else if(i1==ghost[0].i && j1==ghost[0].j)
                if(power_up == 0)
                    cout<<"\e[32m";
                else
                    cout<<"\e[34m";
            else if(i1==ghost[1].i && j1==ghost[1].j)
                if(power_up == 0)
                    cout<<"\e[35m";
                else
                    cout<<"\e[34m";
            else if(i1==ghost[2].i && j1==ghost[2].j)
                if(power_up == 0)
                    cout<<"\e[36m";
                else
                    cout<<"\e[34m";
            else if(i1==ghost[3].i && j1==ghost[3].j)
                if(power_up == 0)
                    cout<<"\e[38;5;167m";
                else
                    cout<<"\e[34m";
            else if(i1==fruit.i && j1==fruit.j)
                cout<<"\e[31m";
            else if(i1 == pellet[0].i && j1 == pellet[0].j)
                cout<<"\e[33m";
            else if(i1 == pellet[1].i && j1 == pellet[1].j)
                cout<<"\e[33m";
            else if(i1 == pellet[2].i && j1 == pellet[2].j)
                cout<<"\e[33m";
            else if(i1 == pellet[3].i && j1 == pellet[3].j)
                cout<<"\e[33m";
            else
                cout<<"\e[0m";
            cout<<" "<<board[i1][j1]<<" ";
            cout<<"\e[0m";
        }
        cout<<endl;
    }
    cout<<endl<<"\t\e[0m\e[4m\e[41mSCORE:\e[0m "<<score<<"\t\t\t\t\t\e[4m\e[41mLIVES:\e[0m "<<life<<"\n\n";
}

void normal_movement_ghost() {
    for(int k=0; k<4; k++) {
        if(ghost[k].dir == r) {
            if ((ghost[k].j+1) < MAXC && (board[ghost[k].i][ghost[k].j+1]!='#')) {
                board[ghost[k].i][ghost[k].j] = ' ';
                ++ghost[k].j;
            }
            else if(board[ghost[k].i][ghost[k].j+1]!='#' && (ghost[k].j+1) >= MAXC) {
                board[ghost[k].i][ghost[k].j] = ' ';
                ghost[k].j = 0;
            }
        }
        else if(ghost[k].dir == l) {
            if ((ghost[k].j-1) > 0 && (board[ghost[k].i][ghost[k].j-1]!='#')) {
                board[ghost[k].i][ghost[k].j] = ' ';
                --ghost[k].j;
            }
            else if(board[ghost[k].i][ghost[k].j-1]!='#' && (ghost[k].j-1) <= 0) {
                board[ghost[k].i][ghost[k].j] = ' ';
                ghost[k].j = MAXC;
            }
        }
        else if(ghost[k].dir == u) {
            if ((ghost[k].i-1) > 0 && (board[ghost[k].i-1][ghost[k].j]!='#')) {
                board[ghost[k].i][ghost[k].j] = ' ';
                --ghost[k].i;
            }
            // else if(board[ghost[k].i-1][ghost[k].j]!='#' && (ghost[k].i-1) <= 0){
            //   board[ghost[k].i][ghost[k].j] = ' ';
            //   ghost[k].i = 18;
            // }
        }
        else if(ghost[k].dir == d) {
            if ((ghost[k].i+1) < MAXR && (board[ghost[k].i+1][ghost[k].j]!='#')) {
                board[ghost[k].i][ghost[k].j] = ' ';
                ++ghost[k].i;
            }
            // else if(board[ghost[k].i+1][ghost[k].j]!='#' && (ghost[k].i+1) >= 19){
            //   board[ghost[k].i][ghost[k].j] = ' ';
            //   ghost[k].i = 1;
            // }
        }
        board[ghost[k].i][ghost[k].j] = ghost[k].type;
    }
}

void dir_change_ghost() {
    int c=0;
    for(int k=0; k<4; k++) {
        switch(ghost[k].dir) {
        case u:
            if(board[ghost[k].i-1][ghost[k].j]=='#') {
                c=1;
            }
            break;
        case r:
            if(board[ghost[k].i][ghost[k].j+1]=='#') {
                c=1;
            }
            break;
        case l:
            if(board[ghost[k].i][ghost[k].j-1]=='#') {
                c=1;
            }
            break;
        case d:
            if(board[ghost[k].i+1][ghost[k].j]=='#') {
                c=1;
            }
            break;
        default:
            c=0;
        }
        if(c!=0) {
            switch(rand()%4) {
            case 0:
                ghost[k].dir = r;
                break;
            case 1:
                ghost[k].dir = l;
                break;
            case 2:
                ghost[k].dir = u;
                break;
            case 3:
                ghost[k].dir = d;
            }
        }
        c=0;
    }
}

void game_over() {
    for(int k=0; k<4; k++) {
        if(pacman.i==ghost[k].i && pacman.j==ghost[k].j) {
            life = life - 1;
            system("clear");
            usleep(microsec*3);
            board_print();
            usleep(microsec*3);
            system("clear");
            usleep(microsec*3);
            board_print();
            usleep(microsec*3);
            system("clear");
            if(life == 0) {
                cout<<"\e[31m";
                cout<<"  ▄████  ▄▄▄       ███▄ ▄███▓▓█████     ▒█████   ██▒   █▓▓█████  ██▀███  \n";
                cout<<" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀    ▒██▒  ██▒▓██░   █▒▓█   ▀ ▓██ ▒ ██▒\n";
                cout<<"▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███      ▒██░  ██▒ ▓██  █▒░▒███   ▓██ ░▄█ ▒"<<endl;
                cout<<"░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄    ▒██   ██░  ▒██ █░░▒▓█  ▄ ▒██▀▀█▄  "<<endl;
                cout<<"░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒   ░ ████▓▒░   ▒▀█░  ░▒████▒░██▓ ▒██▒"<<endl;
                cout<<" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░   ░ ▒░▒░▒░    ░ ▐░  ░░ ▒░ ░░ ▒▓ ░▒▓░"<<endl;
                cout<<"  ░   ░   ▒   ▒▒ ░░  ░      ░ ░ ░  ░     ░ ▒ ▒░    ░ ░░   ░ ░  ░  ░▒ ░ ▒░"<<endl;
                cout<<"░ ░   ░   ░   ▒   ░      ░      ░      ░ ░ ░ ▒       ░░     ░     ░░   ░ "<<endl;
                cout<<"      ░       ░  ░       ░      ░  ░       ░ ░        ░     ░  ░   ░     "<<endl;
                cout<<"                                                     ░                   \e[0m"<<endl<<endl;
                cout<<"\e[41m\e[1mYOUR SCORE:\e[0m "<<score<<endl;
                system("stty echo");
                exit(1);
            }
            else {
                for(int k=0; k<4; k++) {
                    if(board[ghost[k].i][ghost[k].j] == ghost[k].type)
                        board[ghost[k].i][ghost[k].j] = ' ';
                }
                if(board[pacman.i][pacman.j] == pacman.type)
                    board[pacman.i][pacman.j] = ' ';
                if(board[fruit.i][fruit.j] == fruit.type)
                    board[fruit.i][fruit.j] = ' ';
                initialise();
                set_pellets();
                flag = 0;
                board_print();
            }
        }
    }
}

void normal_movement() {
    for ( ; ; ) {//empty loop as movement in a single direction is indefinite
        // if(speed%2==0){
        board[fruit.i][fruit.j]=fruit.type;
        for(int k=0; k<4; k++) {
            board[pellet[k].i][pellet[k].j] = pellet[k].type;
        }
        if(pacman.dir == r) {
            pacman.type = '<';
            if ((pacman.j+1) < MAXC && (board[pacman.i][pacman.j+1]!='#')) {
                board[pacman.i][pacman.j] = ' ';
                ++pacman.j;
            }
            else if(board[pacman.i][pacman.j+1]!='#' && (pacman.j+1) >= MAXC) {
                board[pacman.i][pacman.j] = ' ';
                pacman.j = 0;
            }
        }
        else if(pacman.dir == l) {
            pacman.type = '>';
            if ((pacman.j-1) > 0 && (board[pacman.i][pacman.j-1]!='#')) {
                board[pacman.i][pacman.j] = ' ';
                --pacman.j;
            }
            else if(board[pacman.i][pacman.j-1]!='#' && (pacman.j-1) <= 0) {
                board[pacman.i][pacman.j] = ' ';
                pacman.j = MAXC;
            }
        }
        else if(pacman.dir == u) {
            pacman.type = 'v';
            if ((pacman.i-1) > 0 && (board[pacman.i-1][pacman.j]!='#')) {
                board[pacman.i][pacman.j] = ' ';
                --pacman.i;
            }
            // else if(board[pacman.i-1][pacman.j]!='#' && (pacman.i-1) <= 0){
            //   board[pacman.i][pacman.j] = ' ';
            //   pacman.i = 18;
            // }
        }
        else if(pacman.dir == d) {
            pacman.type = '^';
            if ((pacman.i+1) <= MAXR  && (board[pacman.i+1][pacman.j]!='#')) {
                board[pacman.i][pacman.j] = ' ';
                ++pacman.i;
            }
            // else if(board[pacman.i+1][pacman.j]!='#' && (pacman.i+1) >= 19){
            //   board[pacman.i][pacman.j] = ' ';
            //   pacman.i = 1;
            // }
        }
        if(board[pacman.i][pacman.j] == fruit.type) {
            flag = 0;
            ++score;
            cout<<"\a";
        }
        for(int k=0; k<4; k++) {
            if(pacman.i == pellet[k].i && pacman.j == pellet[k].j && pellet[k].type == 'o') {
                power_up = 1;
                pellet[k].type = ' ';
            }
        }
        board[pacman.i][pacman.j] = pacman.type;
        // }
        if(power_up == 1) { //to be activated in power pellet mode
            if(speed%2 == 0) {
                normal_movement_ghost();
                dir_change_ghost();
            }
        }
        else {
            normal_movement_ghost();
            dir_change_ghost();
        }
        board_print();
        if(speed==5) {
            speed=0;
        }
        else {
            speed++;
        }
        if(power_up == 0) {
            game_over();
        }
        else {

            for(int k=0; k<4; k++) {
                if(ghost[k].type == 'H')
                    ghost[k].type = 'h';
                if(pacman.i == ghost[k].i && pacman.j == ghost[k].j && ghost[k].type != 'x') {
                    ghost[k].type = 'x';
                    score += 2;
                }
                // if(ghost[k].type == 'x'){
                //     life[k]++;
                //     if(life[k] >= DUR/2)
                //         ghost[k].type = 'H';

                // }
            }
        }
        if(power_up == 1) {
            pow_dur++;
            if(pow_dur == DUR) {
                power_up = 0;
                pow_dur = 0;
                ghost[0].type = ghost[1].type = ghost[2].type = ghost[3].type = 'H';
            }
        }
        usleep(microsec);
    }
}

void direction_change() {
    char c;
    while((c=getchar())) {
        //if(kbhit()){
        if (c == 27) { //arrow key detection
            //cout<<"arrow";
            c = getchar();
            //cout<<c;
            if (c == 91) {
                c = getchar();
                //cout<<c;
                switch(c) {
                case 65:
                    if(board[pacman.i-1][pacman.j]!='#')
                        pacman.dir = u;
                    break;
                case 66:
                    if(board[pacman.i+1][pacman.j]!='#')
                        pacman.dir = d;
                    break;
                case 67:
                    if(board[pacman.i][pacman.j+1]!='#')
                        pacman.dir = r;
                    break;
                case 68:
                    if(board[pacman.i][pacman.j-1]!='#')
                        pacman.dir = l;
                    break;
                }
            }
        }
        else {
            switch(c) {
            case 'w':
                if(board[pacman.i-1][pacman.j]!='#')
                    pacman.dir = u;
                break;
            case 's':
                if(board[pacman.i+1][pacman.j]!='#')
                    pacman.dir = d;
                break;
            case 'd':
                if(board[pacman.i][pacman.j+1]!='#')
                    pacman.dir = r;
                break;
            case 'a':
                if(board[pacman.i][pacman.j-1]!='#')
                    pacman.dir = l;
                break;
            }
        }
        if( c == '\n') {
            system("stty echo");
            exit(0);
        }
    }
}

void about() {
    cout<<"\e[30m\e[103m\e[1m";
    cout<<" _______     _        ______       ____    ____       _       ____  _____ \n";
    cout<<"|_   __ \\   / \\     .' ___  |     |_   \\  /   _|     / \\     |_   \\|_   _|\n";
    cout<<"  | |__) | / _ \\   / .'   \\_|______ |   \\/   |      / _ \\      |   \\ | |  \n";
    cout<<"  |  ___/ / ___ \\  | |      |______|| |\\  /| |     / ___ \\     | |\\ \\| |  \n";
    cout<<" _| |_  _/ /   \\ \\_\\ `.___.'\\      _| |_\\/_| |_  _/ /   \\ \\_  _| |_\\   |_ \n";
    cout<<"|_____||____| |____|`.____ .'     |_____||_____||____| |____||_____|\\____|\n";

    cout<<"\n\e[0m\e[4m\e[1mWELCOME TO PAC-MAN !";
    cout<<"\e[0m\nThe rules are very simple:\n";
    cout<<"\e[44m\e[4mPACMAN\e[0m \e[33m< \e[0m:This is you\n";
    cout<<"\e[44m\e[4mGHOSTS\e[0m  \e[32mH \e[35mH \e[36mH \e[38;5;167mH \e[0m: Stay away from them\n";
    cout<<"\e[44m\e[4mCHERRY\e[0m  \e[31m %\e[0m: You have to eat this to increase score\n";
    cout<<"\e[44m\e[4mPOWER PELLET\e[0m  \e[33mo\e[0m: After eating this, you can eat ghosts\e[34m h\e[0m to score points,\nbut beware this only works for a small duration\n";
    cout<<"\nPRESS ENTER TO CONTINUE...";
    char g = getchar();
}

int main() {
    cout<<"\e[8;30;74t";
    system("clear");
    srand(time(NULL));
    about();
    initialise();
    set_pellets();
    system("stty cbreak");
    system("stty -echo");
    std::thread m(normal_movement);
    //m.detach();
    std::thread t(direction_change);
    t.join();
    m.join();
    system("stty echo");
    return 0;

}
