#include <iostream>
#include <QApplication>
#include "mainwindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

//    fout.open("record.txt", std::ios::app);
//            InitGame();
//            char msg[256];  //控制台输入的命令
//            char msgs[256];
//            int num;        //记录数量
//            while(1) {
//                    fflush(stdin);
//                    sleep(1);
//                    fflush(stdout);
//                    scanf("%s",msg);
//                    History_Count_per_Move = w.GetHistoryCount();
//                    Future_Count_Per_Move = w.GetFutureCount();
//                    Thread_Count = w.GetThreadCount();
//                    cout << "His: " << History_Count_per_Move << endl;
//                    cout << "Fru: " << Future_Count_Per_Move << endl;
//                    cout << "Thr: " << Thread_Count << endl;
//                    if(strcmp(msg,"name?") == 0) {
//                            cout << "name";
//                            cout << " blazar" << endl;
//                            fflush(stdout);
//                    }
//                    else if(strcmp(msg,"new") == 0) {                              //开始游戏
//                            scanf("%s",msg);
//                            if(strcmp(msg,"black") == 0) {
//                                    ChessColor = BLACK;
//                                    OppoColor = WHITE;
//                                    Chess = foolgo::BLACK_FORCE;
//                                    Oppo = foolgo::WHITE_FORCE;
//                                    gettimeofday(&start, nullptr);
//                                    move();
//                            }else {
//                                    ChessColor = WHITE;
//                                    OppoColor = BLACK;
//                                    Chess = foolgo::WHITE_FORCE;
//                                    Oppo = foolgo::BLACK_FORCE;
//                            }
//                    }else if(strcmp(msg,"move") == 0) {                          //开始搜索下一步
//                            gettimeofday(&start, nullptr);
//                            record();
//                            jie.x = 0;
//                            jie.y = 0;          //清空劫的棋盘
//                            OppoNum++;
//                            move();
//                    }else if(strcmp(msg,"Y") == 0 || strcmp(msg,"y") == 0) {     //合法
//                            if(board[bestMove.x][bestMove.y] == ChessColor) {

//                            }else if(board[bestMove.x][bestMove.y] == NOSTONE) {   //如果是空
//                                    if(ChessColor == BLACK)
//                                            fout << "B[";
//                                    else
//                                            fout << "W[";
//                                    fout << static_cast<char>(bestMove.y + 'A' - 1) << static_cast<char>(1 + '9' - bestMove.x) << "];" << endl;
//                                    board[bestMove.x][bestMove.y] = ChessColor;
//                                    full_board.PlayMove(foolgo::Move(Chess,convert(bestMove)));
//                                    ChessNum++;
//                                    Step++;
//                            }else if(board[bestMove.x][bestMove.y] == OppoColor) {  //如果是对手棋子
//                                    board[bestMove.x][bestMove.y] = ChessColor;
//                                    ChessNum++;
//                                    Step++;
//                                    OppoNumKnow--;
//                                    OppoNum--;
//                            }
//                            jie.x = 0;
//                            jie.y = 0;
//                            PrintBoard();
//                            foolgo::ffflag = 0;
//                            gettimeofday(&finish, nullptr);
//                            time_all += static_cast<double>((finish.tv_sec-start.tv_sec) * 1000000 + finish.tv_usec-start.tv_usec)*k/1000000;
//                            start.tv_sec = finish.tv_sec;
//                            start.tv_usec = finish.tv_usec;
//                            displayTime();
//                    }else if(strcmp(msg,"N") == 0 || strcmp(msg,"n") == 0) {     //非法
//                            gettimeofday(&finish, nullptr);
//                            time_all += static_cast<double>((finish.tv_sec-start.tv_sec) * 1000000 + finish.tv_usec-start.tv_usec)*k/1000000;
//                            start.tv_sec = finish.tv_sec;
//                            start.tv_usec = finish.tv_usec;
//                            displayTime();
//                            gettimeofday(&start, nullptr);
//                            if(board[bestMove.x][bestMove.y] != OppoColor) {
//                                    board[bestMove.x][bestMove.y] = OppoColor;
//                                    OppoNumKnow++;
//                            }
//                            board_know[bestMove.x][bestMove.y]++;      //试探次数加一
//                            PrintBoard();
//                            full_board.PlayMove4(foolgo::Move(Oppo,convert(bestMove)));
//                            move();
//                    }else if(strcmp(msg,"take") == 0) {                          //提子
//                            record();
//                            fflush(stdin);
//                            scanf("%d",&num);
//                            scanf("%s",msgs);
//                            int number = 0;
//                            for(int i = 0; msgs[i] != '\0'; i++) {
//                                    number++;
//                            }
//                            if(number != num*2) {
//                                    cout << "     Error input!!!" << endl;
//                            }else{
//                                    for(int i = 0; i < num; i++) {
//                                            msg[2*i+1] = msgs[2*i];
//                                            msg[2*i] = 'A' + '9' - msgs[2*i+1];
//                                    }
//                                    jie.x = 0;
//                                    jie.y = 0;
//                                    Move ssmove;
//                                    for(int i = 0; i < num; i++) {
//                                            if(num == 1) {
//                                                    jie.x = msg[2*i] - 'A' + 1;
//                                                    jie.y = msg[2*i+1] - 'A' + 1;
//                                            }
//                                            if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == OppoColor) {
//                                                    OppoNum--;
//                                                    OppoNumKnow--;
//                                                    board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
//                                            }else if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == NOSTONE) {
//                                                    OppoNum--;
//                                                    board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
//                                            }
//                                            ssmove.x = msg[2*i] - 'A' + 1;
//                                            ssmove.y = msg[2*i+1] - 'A' + 1;
//                                            foolgo::PositionIndex a = convert(ssmove);
//                                            if(full_board.GetPointState(a) != Oppo)
//                                                    full_board.PlayMove2(foolgo::Move(Oppo,a));
//                                    }
//                                    PrintBoard();
//                            }
//                    }else if(strcmp(msg,"taked") == 0) {                        //被提子
//                            fflush(stdin);
//                            gettimeofday(&start, nullptr);
//                            record();
//                            scanf("%d",&num);
//                            scanf("%s",msgs);
//                            int number = 0;
//                            for(int i = 0; msgs[i] != '\0'; i++) {
//                                    number++;
//                            }
//                            if(number != num*2) {
//                                    cout << "     Error input!!!" << endl;
//                            }else{
//                                    for(int i = 0; i < num; i++) {
//                                            msg[2*i+1] = msgs[2*i];
//                                            msg[2*i] = 'A' + '9' - msgs[2*i+1];
//                                    }
//                                    jie.x = 0;
//                                    jie.y = 0;
//                                    Move ppmove;
//                                    for(int i = 0; i < num; i++) {
//                                            if(num == 1) {
//                                                    jie.x = msg[2*i] - 'A' + 1;
//                                                    jie.y = msg[2*i+1] - 'A' + 1;
//                                            }
//                                            if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == ChessColor) {
//                                                    ChessNum--;
//                                                    int x = msg[2 * i] - 'A' + 1;
//                                                    int y = msg[2 * i + 1] - 'A' + 1;
//                                                    //棋盘四周补上敌方棋子
//                                                    if (board[x - 1][y] == NOSTONE)
//                                                    {
//                                                            ppmove.x = x - 1;
//                                                            ppmove.y = y;
//                                                            foolgo::PositionIndex a = convert(ppmove);
//                                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
//                                                            board[x - 1][y] = OppoColor;
//                                                            OppoNumKnow++;
//                                                    }
//                                                    if (board[x + 1][y] == NOSTONE)
//                                                    {
//                                                            ppmove.x = x + 1;
//                                                            ppmove.y = y;
//                                                            foolgo::PositionIndex a = convert(ppmove);
//                                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
//                                                            board[x + 1][y] = OppoColor;
//                                                            OppoNumKnow++;
//                                                    }
//                                                    if (board[x][y - 1] == NOSTONE)
//                                                    {
//                                                            ppmove.x = x;
//                                                            ppmove.y = y - 1;
//                                                            foolgo::PositionIndex a = convert(ppmove);
//                                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
//                                                            board[x][y - 1] = OppoColor;
//                                                            OppoNumKnow++;
//                                                    }else if(strcmp(msg,"debug") == 0) {
//                                                    }
//                                                    if (board[x][y + 1] == NOSTONE)
//                                                    {
//                                                            ppmove.x = x;
//                                                            ppmove.y = y + 1;
//                                                            foolgo::PositionIndex a = convert(ppmove);
//                                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
//                                                            board[x][y + 1] = OppoColor;
//                                                            OppoNumKnow++;
//                                                    }
//                                            }
//                                    }
//                                    for(int i = 0; i < num; i++) {
//                                            if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == ChessColor) {
//                                                    board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
//                                            }
//                                    }
//                                    OppoNum++;
//                                    move();
//                                    PrintBoard();
//                            }
//                    }else if(strcmp(msg,"generate") == 0) {
//                            fout.close();
//                            string name;
//                            cout << "Please input your txt name: " << endl;
//                            cin >> name;
//                            string firstMsg;
//                            cout << "Please input the name of julio cruz team: " << endl;
//                            cin >> firstMsg;
//                            string laterMsg;
//                            cout << "Please input the name of the back team: " << endl;
//                            cin >> laterMsg;
//                            bool winner;
//                            cout << "Please input the winner team: (0:先手胜;1:后手胜)" << endl;
//                            cin >> winner;
//                            string Time;
//                            cout << "Please input the name of the day: (example:2017.07.29) " << endl;
//                            cin >> Time;
//                            string And;
//                            cout << "Please input the name of the time: (example:14:00) " << endl;
//                            cin >> And;
//                            string Place;
//                            cout << "Please input the name of the place: (example:重庆) " << endl;
//                            cin >> Place;
//                            string temp;
//                            if(winner == 0)
//                                    temp = "先手胜";
//                            else
//                                    temp = "后手胜";
//                            ofstream out;
//                            out.open(name, std::ios::app);
//                            out << "([PG][先手参赛队 " << firstMsg << "][后手参赛队 " << laterMsg << "][" << temp << "][" << Time << " " << And << " " << Place << "][2018 CCGC];";
//                            FILE* fp;
//                            fp = fopen("record.txt","r");
//                            char b;
//                            b = static_cast<char>(fgetc(fp));
//                            while(b != EOF) {
//                                    if(b != ' ' && b != '\n') {
//                                            out << b;
//                                    }
//                                    b = static_cast<char>(fgetc(fp));
//                            }
//                            fclose(fp);
//                            out << ')';
//                            out.close();
//                            cout << "write finished." << endl;
//                    }else if(strcmp(msg,"print") == 0) {
//                            PrintBoard();
//                    }else if(strcmp(msg,"init") == 0) {
//                            InitGame();
//                            cout << "     init succeed. " << endl;
//                    }else if(strcmp(msg,"debug") == 0) {
//                            boardCopy1();
//                            int x,y,color;
//                            cin >> x >> y >> color;
//                            Move n;
//                            n.x = x; n.y = y;
//                            cout << "result: " << Check(n,color,board,board_know);
//                    }else if(strcmp(msg,"undo") == 0) {
//                            undo();
//                            PrintBoard();
//                    }else if(strcmp(msg,"quit") == 0 || strcmp(msg,"end") == 0) {
//                            break;
//                    }else if(strcmp(msg,"error") == 0) {
//                            cout << "error" << endl;
//                    }else if(strcmp(msg,"load") == 0) {
//                            Load();
//                            PrintBoard();
//                    }else if(strcmp(msg,"add") == 0) {
//                            scanf("%s",msg);
//                            int x = msg[0] - 'A' + 1;
//                            int y = msg[1] - 'A' + 1;
//                            AddEnemy(x,y);
//                            PrintBoard();
//                    }else if(strcmp(msg,"debug") == 0) {

//                    }else
//                            cout << "     Error input!!! " << endl;
//            }
}
