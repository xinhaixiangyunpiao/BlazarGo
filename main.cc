#include <iostream>
#include <string.h>
#include "search.h"
#include "board/force.h"
using namespace std;

int board_temp[SIZE][SIZE];
int Step_temp;
int ChessNum_temp;
int OppoNum_temp;
int OppoNumKnow_temp;
int ChessColor_temp;
int OppoColor_temp;
int board_know_temp[SIZE][SIZE];                                //暂存
foolgo::FullBoard<foolgo::MAIN_BOARD_LEN> full_board_temp;      //暂存
double time_all;                                                //总时间
double time_temp;                                               //暂存
timeval start,finish;                                           //计时器
float k = 0.9;

void displayTime(){
        cout << "USE_TIME: " << time_all << " s" << endl;
        int times = (int)time_all;
        int minute = times/60;
        int second = times%60;
        char min[2];
        char sec[2];
        min[0] = (char)(minute%10 + '0');
        min[1] = (char)(minute/10 + '0');
        sec[0] = (char)(second%10 + '0');
        sec[1] = (char)(second/10 + '0');
        cout << "          " << min[1] << min[0] << ":" << sec[1] << sec[0] << endl;
}

void record(){
        time_temp = time_all;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        board_temp[i][j] = board[i][j];
                        board_know_temp[i][j] = board_know[i][j];
                }
        }
        Step_temp = Step;
        ChessNum_temp = ChessNum;
        OppoNum_temp = OppoNum;
        OppoNumKnow_temp = OppoNumKnow;
        ChessColor_temp = ChessColor;
        OppoColor_temp = OppoColor;
        full_board_temp.Copy(full_board);
}

void InitGame(){
        time_all = 0;
        time_temp = 0;
        terminal = 0;
        memset(board,0,sizeof(board));
        memset(tempBoard,0,sizeof(tempBoard));
        memset(board_know,0,sizeof(board_know));
        for (int i = 0; i < 11; i++)
        {
                board[i][0] = BORDER;
                board[0][i] = BORDER;
                board[i][10] = BORDER;
                board[10][i] = BORDER;
        }              //边界赋上BORDER
        Step = 0;
        ChessNum = 0;
        OppoNum = 0;
        OppoNumKnow = 0;
        ChessColor = BLACK;
        OppoColor = WHITE;
        Chess = foolgo::BLACK_FORCE;
        Oppo = foolgo::WHITE_FORCE;
        seed = foolgo::GetTimeSeed();
        cout << "seed:" << seed << std::endl; //get seed
        foolgo::ZobHasher<foolgo::MAIN_BOARD_LEN>::Init(seed);
        full_board.Init();
        full_board_temp.Init();
        record();
}

foolgo::PositionIndex convert(Move pMove){
        int x = pMove.x;
        int y = pMove.y;
        foolgo::PositionIndex result = (x - 1)*9 + y - 1;
        return result;
}

void Load(){
        int a[11][11] = {
                3,3,3,3,3,3,3,3,3,3,3,
                3,0,0,0,0,0,2,1,0,1,3,
                3,0,2,2,2,2,2,1,1,0,3,
                3,2,0,2,1,1,1,1,1,0,3,
                3,0,2,2,2,1,1,0,1,1,3,
                3,2,1,2,1,1,1,1,0,1,3,
                3,1,1,1,1,2,2,1,1,0,3,
                3,1,1,2,2,2,2,1,1,1,3,
                3,1,2,2,2,0,2,1,1,1,3,
                3,2,0,2,0,2,0,2,1,0,3,
                3,3,3,3,3,3,3,3,3,3,3
        };
        for(int i = 0; i <= 10; i++) {
                for(int j = 0; j <= 10; j++) {
                        board[i][j] = a[i][j];
                }
        }
        Step = 43;
        OppoNum = 36;
        Move temp;
        ChessNum = 0;
        OppoNumKnow = 0;
        time_all = 0;
        for(temp.x = 1; temp.x < 10; temp.x++) {
                for(temp.y = 1; temp.y < 10; temp.y++) {
                        if(board[temp.x][temp.y] == NOSTONE) {

                        }else if(board[temp.x][temp.y] == ChessColor) {
                                ChessNum++;
                                full_board.PlayMove(foolgo::Move(Chess,convert(temp)));
                        }else if(board[temp.x][temp.y] == OppoColor) {
                                OppoNumKnow++;
                                full_board.PlayMove(foolgo::Move(Oppo,convert(temp)));
                        }
                }
        }
}

void PrintMessage(){
        cout << "     Step: " << Step << endl;
        cout << "     ChessNum: " << ChessNum << endl;
        cout << "     OppoNum: " << OppoNum << endl;
        cout << "     OppoNumKnow: " << OppoNumKnow << endl;
}

void PrintBoard(){
        for(int i = 1; i <= 9; i++) {
                cout << (char)('9' - i + 1);
                for(int j = 1; j <= 9; j++) {
                        switch (board[i][j])
                        {
                        case 0:
                                cout << " -";
                                break;
                        case 1:
                                cout << " *";
                                break;
                        case 2:
                                cout << " O";
                                break;
                        }
                }
                cout << endl;
        }
        cout << " ";
        for(int i = 1; i <= 9; i++) {
                cout << " " << (char)(i + 'A' - 1);
        }
        cout << endl;
        PrintMessage();
}

void move(){
        cout << "jiex: " << jie.x << endl;
        cout << "jiey: " << jie.y << endl;
        if(SearchMove(&bestMove)) {    //搜索到最优步
                cout << "move " << (char)(bestMove.y + 'A' - 1) << (char)('A' + '9' - (char)(bestMove.x + 'A' - 1)) << endl;
                fflush(stdout);
        }else{                         //搜索失败
                printf("move pass\n");
        }
}

void undo(){
        time_all = time_temp;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        board[i][j] = board_temp[i][j];
                        board_know[i][j] = board_know_temp[i][j];
                }
        }
        Step = Step_temp;
        ChessNum = ChessNum_temp;
        OppoNum = OppoNum_temp;
        OppoNumKnow = OppoNumKnow_temp;
        ChessColor = ChessColor_temp;
        OppoColor = OppoColor_temp;
        full_board.Copy(full_board_temp);
}

void AddEnemy(int x,int y){
        board[x][y] = OppoColor;
        OppoNumKnow++;
}

int main(){
        fout.open("record.txt", std::ios::app);
        InitGame();
        char msg[256];  //控制台输入的命令
        char msgs[256];
        int num;        //记录数量
        while(1) {
                fflush(stdin);
                sleep(1);
                fflush(stdout);
                scanf("%s",msg);
                if(strcmp(msg,"name?") == 0) {
                        cout << "name";
                        cout << " blazar" << endl;
                        fflush(stdout);
                }
                else if(strcmp(msg,"new") == 0) {                              //开始游戏
                        scanf("%s",msg);
                        if(strcmp(msg,"black") == 0) {
                                ChessColor = BLACK;
                                OppoColor = WHITE;
                                Chess = foolgo::BLACK_FORCE;
                                Oppo = foolgo::WHITE_FORCE;
                                gettimeofday(&start, NULL);
                                move();
                        }else {
                                ChessColor = WHITE;
                                OppoColor = BLACK;
                                Chess = foolgo::WHITE_FORCE;
                                Oppo = foolgo::BLACK_FORCE;
                        }
                }else if(strcmp(msg,"move") == 0) {                          //开始搜索下一步
                        gettimeofday(&start, NULL);
                        record();
                        jie.x = 0;
                        jie.y = 0;          //清空劫的棋盘
                        OppoNum++;
                        move();
                }else if(strcmp(msg,"Y") == 0 || strcmp(msg,"y") == 0) {     //合法
                        if(board[bestMove.x][bestMove.y] == ChessColor) {

                        }else if(board[bestMove.x][bestMove.y] == NOSTONE) {   //如果是空
                                if(ChessColor == BLACK)
                                        fout << "B[";
                                else
                                        fout << "W[";
                                fout << (char)(bestMove.y + 'A' - 1) << (char)(1 + '9' - bestMove.x) << "];" << endl;
                                board[bestMove.x][bestMove.y] = ChessColor;
                                full_board.PlayMove(foolgo::Move(Chess,convert(bestMove)));
                                ChessNum++;
                                Step++;
                        }else if(board[bestMove.x][bestMove.y] == OppoColor) {  //如果是对手棋子
                                board[bestMove.x][bestMove.y] = ChessColor;
                                ChessNum++;
                                Step++;
                                OppoNumKnow--;
                                OppoNum--;
                        }
                        jie.x = 0;
                        jie.y = 0;
                        PrintBoard();
                        foolgo::ffflag = 0;
                        gettimeofday(&finish, NULL);
                        time_all += (double)((finish.tv_sec-start.tv_sec) * 1000000 + finish.tv_usec-start.tv_usec)*k/1000000;
                        start.tv_sec = finish.tv_sec;
                        start.tv_usec = finish.tv_usec;
                        displayTime();
                }else if(strcmp(msg,"N") == 0 || strcmp(msg,"n") == 0) {     //非法
                        gettimeofday(&finish, NULL);
                        time_all += (double)((finish.tv_sec-start.tv_sec) * 1000000 + finish.tv_usec-start.tv_usec)*k/1000000;
                        start.tv_sec = finish.tv_sec;
                        start.tv_usec = finish.tv_usec;
                        displayTime();
                        gettimeofday(&start, NULL);
                        if(board[bestMove.x][bestMove.y] != OppoColor) {
                                board[bestMove.x][bestMove.y] = OppoColor;
                                OppoNumKnow++;
                        }
                        board_know[bestMove.x][bestMove.y]++;      //试探次数加一
                        PrintBoard();
                        full_board.PlayMove4(foolgo::Move(Oppo,convert(bestMove)));
                        move();
                }else if(strcmp(msg,"take") == 0) {                          //提子
                        record();
                        fflush(stdin);
                        scanf("%d",&num);
                        scanf("%s",msgs);
                        int number = 0;
                        for(int i = 0; msgs[i] != '\0'; i++) {
                                number++;
                        }
                        if(number != num*2) {
                                cout << "     Error input!!!" << endl;
                        }else{
                                for(int i = 0; i < num; i++) {
                                        msg[2*i+1] = msgs[2*i];
                                        msg[2*i] = 'A' + '9' - msgs[2*i+1];
                                }
                                jie.x = 0;
                                jie.y = 0;
                                Move ssmove;
                                for(int i = 0; i < num; i++) {
                                        if(num == 1) {
                                                jie.x = msg[2*i] - 'A' + 1;
                                                jie.y = msg[2*i+1] - 'A' + 1;
                                        }
                                        if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == OppoColor) {
                                                OppoNum--;
                                                OppoNumKnow--;
                                                board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                                        }else if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == NOSTONE) {
                                                OppoNum--;
                                                board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                                        }
                                        ssmove.x = msg[2*i] - 'A' + 1;
                                        ssmove.y = msg[2*i+1] - 'A' + 1;
                                        foolgo::PositionIndex a = convert(ssmove);
                                        if(full_board.GetPointState(a) != Oppo)
                                                full_board.PlayMove2(foolgo::Move(Oppo,a));
                                }
                                PrintBoard();
                        }
                }else if(strcmp(msg,"taked") == 0) {                        //被提子
                        fflush(stdin);
                        gettimeofday(&start, NULL);
                        record();
                        scanf("%d",&num);
                        scanf("%s",msgs);
                        int number = 0;
                        for(int i = 0; msgs[i] != '\0'; i++) {
                                number++;
                        }
                        if(number != num*2) {
                                cout << "     Error input!!!" << endl;
                        }else{
                                for(int i = 0; i < num; i++) {
                                        msg[2*i+1] = msgs[2*i];
                                        msg[2*i] = 'A' + '9' - msgs[2*i+1];
                                }
                                jie.x = 0;
                                jie.y = 0;
                                Move ppmove;
                                for(int i = 0; i < num; i++) {
                                        if(num == 1) {
                                                jie.x = msg[2*i] - 'A' + 1;
                                                jie.y = msg[2*i+1] - 'A' + 1;
                                        }
                                        if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == ChessColor) {
                                                ChessNum--;
                                                int x = msg[2 * i] - 'A' + 1;
                                                int y = msg[2 * i + 1] - 'A' + 1;
                                                //棋盘四周补上敌方棋子
                                                if (board[x - 1][y] == NOSTONE)
                                                {
                                                        ppmove.x = x - 1;
                                                        ppmove.y = y;
                                                        foolgo::PositionIndex a = convert(ppmove);
                                                        full_board.PlayMove3(foolgo::Move(Oppo,a));
                                                        board[x - 1][y] = OppoColor;
                                                        OppoNumKnow++;
                                                }
                                                if (board[x + 1][y] == NOSTONE)
                                                {
                                                        ppmove.x = x + 1;
                                                        ppmove.y = y;
                                                        foolgo::PositionIndex a = convert(ppmove);
                                                        full_board.PlayMove3(foolgo::Move(Oppo,a));
                                                        board[x + 1][y] = OppoColor;
                                                        OppoNumKnow++;
                                                }
                                                if (board[x][y - 1] == NOSTONE)
                                                {
                                                        ppmove.x = x;
                                                        ppmove.y = y - 1;
                                                        foolgo::PositionIndex a = convert(ppmove);
                                                        full_board.PlayMove3(foolgo::Move(Oppo,a));
                                                        board[x][y - 1] = OppoColor;
                                                        OppoNumKnow++;
                                                }else if(strcmp(msg,"debug") == 0) {
                                                }
                                                if (board[x][y + 1] == NOSTONE)
                                                {
                                                        ppmove.x = x;
                                                        ppmove.y = y + 1;
                                                        foolgo::PositionIndex a = convert(ppmove);
                                                        full_board.PlayMove3(foolgo::Move(Oppo,a));
                                                        board[x][y + 1] = OppoColor;
                                                        OppoNumKnow++;
                                                }
                                        }
                                }
                                for(int i = 0; i < num; i++) {
                                        if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == ChessColor) {
                                                board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                                        }
                                }
                                OppoNum++;
                                move();
                                PrintBoard();
                        }
                }else if(strcmp(msg,"generate") == 0) {
                        fout.close();
                        string name;
                        cout << "Please input your txt name: " << endl;
                        cin >> name;
                        string firstMsg;
                        cout << "Please input the name of julio cruz team: " << endl;
                        cin >> firstMsg;
                        string laterMsg;
                        cout << "Please input the name of the back team: " << endl;
                        cin >> laterMsg;
                        bool winner;
                        cout << "Please input the winner team: (0:先手胜;1:后手胜)" << endl;
                        cin >> winner;
                        string Time;
                        cout << "Please input the name of the day: (example:2017.07.29) " << endl;
                        cin >> Time;
                        string And;
                        cout << "Please input the name of the time: (example:14:00) " << endl;
                        cin >> And;
                        string Place;
                        cout << "Please input the name of the place: (example:重庆) " << endl;
                        cin >> Place;
                        string temp;
                        if(winner == 0)
                                temp = "先手胜";
                        else
                                temp = "后手胜";
                        ofstream out;
                        out.open(name, std::ios::app);
                        out << "([PG][先手参赛队 " << firstMsg << "][后手参赛队 " << laterMsg << "][" << temp << "][" << Time << " " << And << " " << Place << "][2018 CCGC];";
                        FILE* fp;
                        fp = fopen("record.txt","r");
                        char b;
                        b = fgetc(fp);
                        while(b != EOF) {
                                if(b != ' ' && b != '\n') {
                                        out << b;
                                }
                                b = fgetc(fp);
                        }
                        fclose(fp);
                        out << ')';
                        out.close();
                        cout << "write finished." << endl;
                }else if(strcmp(msg,"print") == 0) {
                        PrintBoard();
                }else if(strcmp(msg,"init") == 0) {
                        InitGame();
                        cout << "     init succeed. " << endl;
                }else if(strcmp(msg,"debug") == 0) {
                        boardCopy1();
                        int x,y,color;
                        cin >> x >> y >> color;
                        Move n;
                        n.x = x; n.y = y;
                        cout << "result: " << Check(n,color,board,board_know);
                }else if(strcmp(msg,"undo") == 0) {
                        undo();
                        PrintBoard();
                }else if(strcmp(msg,"quit") == 0 || strcmp(msg,"end") == 0) {
                        break;
                }else if(strcmp(msg,"error") == 0) {
                        cout << "error" << endl;
                }else if(strcmp(msg,"load") == 0) {
                        Load();
                        PrintBoard();
                }else if(strcmp(msg,"add") == 0) {
                        scanf("%s",msg);
                        int x = msg[0] - 'A' + 1;
                        int y = msg[1] - 'A' + 1;
                        AddEnemy(x,y);
                        PrintBoard();
                }else if(strcmp(msg,"debug") == 0) {

                }else
                        cout << "     Error input!!! " << endl;
        }
        return 0;
}
