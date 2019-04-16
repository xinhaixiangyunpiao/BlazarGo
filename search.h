#ifndef SEARCH_H
#define SEARCH_H
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <thread>
#include "player/uct_player.h"
#include "board/zob_hasher.h"
#include "def.h"
#include "util/rand.h"
#include "board/full_board.h"
#include "board/position.h"
#include "player/player.h"

using namespace std;

static ofstream fout;    //文件输出

static uint32_t seed;
static foolgo::FullBoard<foolgo::MAIN_BOARD_LEN> full_board;

#define SIZE 11                          //棋盘行数，其中1..9为棋的坐标，其他为边界
#define COUNT 81                         //可放棋子总数
#define BLACK  1                         //黑棋
#define WHITE  2                         //白棋
#define BORDER 3                         //边界
#define NOSTONE  0                       //空子无棋
#define MCMAX1 200                       //蒙特卡洛次数(模拟历史次数)
#define MCMAX2 50                        //蒙特卡洛次数(探索未来次数)
#define believePoint 0.0                 //信任值
#define LIMIT  7                         //搜索时间
static int History_Count_per_Move;       //模拟历史次数(UCT)
static int Future_Count_Per_Move;        //探索未来次数(UCT)
static int Thread_Count;                 //线程数

typedef struct node {
        int total;
        int win;
} node;

typedef struct Move {
        int x;
        int y;
} Move;

typedef struct Data {
        int totalBoard[SIZE][SIZE];
        int winBoard[SIZE][SIZE];
        int tempBoard[SIZE][SIZE];
        int board_know[SIZE][SIZE];
        int ChessColor;
        int OppoColor;
        int OppoNum;
        int OppoNumKnow;
        Move jie;
} Data;

static Move bestMove;                 //当前最佳着法
static Move jie;                      //劫

static int ChessNum;                  //己方棋子总数
static int OppoNum;                   //敌方棋子总数
static int OppoNumKnow;               //已知敌方棋子总数
static int ChessColor;                //我方棋子颜色
static int OppoColor;                 //敌方棋子颜色
static int ChessEye;                  //我方眼数
static int OppoEye;                   //敌方眼数
static int terminal;

static foolgo::Force Chess;
static foolgo::Force Oppo;

static int board[SIZE][SIZE];         //当前棋盘
static int tempBoard[SIZE][SIZE];     //临时棋盘
static int board_know[SIZE][SIZE];    //试探棋盘
static node markBoard[SIZE][SIZE];    //分数棋盘

static void SaveBoard(int board1[][SIZE],int board2[][SIZE]){
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        board1[i][j] = board2[i][j];
                }
        }
}

static void LoadBoard(int board1[][SIZE],int board2[][SIZE]){
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        board1[i][j] = board2[i][j];
                }
        }
}

static void boardCopy1(){              //复制棋盘
        for(int i = 0; i <= 10; i++) {
                for(int j = 0; j <= 10; j++) {
                        tempBoard[i][j] = board[i][j];
                }
        }
}

static bool isEye(Move pMove, int color, int board[][SIZE]){        //判断某点是否为某种颜色的眼
        Move temp = pMove;
        if(board[temp.x][temp.y] != NOSTONE)
                return false;
        for (int i = -1; i <= 1; i+=2) {
                temp.x = pMove.x + i;
                if (board[temp.x][temp.y] != color && board[temp.x][temp.y] != BORDER) {
                        return false;
                }
                temp.x = pMove.x;
                temp.y = pMove.y + i;
                if (board[temp.x][temp.y] != color && board[temp.x][temp.y] != BORDER) {
                        return false;
                }
                temp.y = pMove.y;
        }
        return true;
}

static int Remove(Move pMove,int color,int board[][SIZE],Move* jie){                 //提子
        int num = 1;
        Move temp = pMove;
        //cout << "x: " << temp.x << " y: " << temp.y << " " << tempBoard[temp.x][temp.y] << endl;
        board[temp.x][temp.y] = 0;
        jie->x = temp.x;
        jie->y = temp.y;
        for (int i = -1; i <= 1; i+=2) {
                temp.x = pMove.x + i;
                if (board[temp.x][temp.y] == color) {
                        num += Remove(temp,color,board,jie);
                }
                temp.x = pMove.x;
                temp.y = pMove.y + i;
                if (board[temp.x][temp.y] == color) {
                        num += Remove(temp,color,board,jie);
                }
                temp.y = pMove.y;
        }
        return num;
}

//位置，颜色，棋盘，visit标志棋盘
static bool Live(Move pMove,int color,int board[][SIZE],int board2[][SIZE]){         //判断棋子死活
        int x = pMove.x;
        int y = pMove.y;
        //cout << "x: " << x << " y: " << y << " " << tempBoard[x][y] << endl;
        Move temp = pMove;
        if(board[x][y] == NOSTONE) return true;
        if (board[x][y] != color) return false;
        board2[x][y] = 1;
        for(int i = -1; i <= 1; i+=2) {
                temp.x += i;
                if(!board2[temp.x][temp.y])
                        if(Live(temp,color,board,board2)) {
                                return true;
                        }
                temp.x = pMove.x;
                temp.y += i;
                if(!board2[temp.x][temp.y])
                        if(Live(temp,color,board,board2)) {
                                return true;
                        }
                temp.y = pMove.y;
        }
        return false;
}

//判断某点是否可下某颜色棋子   返回值含义： 0：不能下
// 1：可以下（普通） 2：可以空子打劫  3：可以敌子打劫        //位置，颜色，棋盘，board_know棋盘
static int Check(Move pMove, int color,int board[][SIZE],int board3[][SIZE]){    //board3：board_know
        int visited[SIZE][SIZE];
        int temp_color;                            //board_know作用：避免重复判断
        Move temp = pMove;
        memset(visited, 0, sizeof(visited));
        if (board[pMove.x][pMove.y] == color)   //这个点有己方棋子
                return 0;
        if (board[pMove.x][pMove.y] == 0) {     //这个点为空
                board[pMove.x][pMove.y] = color;
                if (Live(pMove, color,board,visited)) {
                        board[pMove.x][pMove.y] = 0;
                        return 1;
                }                     //可以打劫的情况也行
                board[pMove.x][pMove.y] = 0;    //如果放入此子后这个子还活着，就可以下这个子，否则重置回来
        }
        if (board3[pMove.x][pMove.y] < 2) {          //这个点为敌方且试探次数小于二      判断此子是否可打劫
                temp_color = board[pMove.x][pMove.y];     //暂存敌方颜色
                board[pMove.x][pMove.y] = color;          //把敌方先变成己方试一试
                for (int i =-1; i <= 1; i+=2) {
                        temp.x = pMove.x + i;                 //上下左右不是边界的其中之一  是敌方且那个子已经死了         （打劫成功）
                        memset(visited, 0, sizeof(visited));
                        if (board[temp.x][temp.y] != color && !Live(temp,color%2+1,board,visited) && board[temp.x][temp.y] != BORDER) {
                                if(temp_color == 0) {
                                        board[pMove.x][pMove.y] = 0;
                                        return 2;
                                }
                                board[pMove.x][pMove.y] = color%2+1;
                                return 3;
                        }
                        temp.x = pMove.x;
                        temp.y = pMove.y + i;
                        memset(visited, 0, sizeof(visited));
                        if (board[temp.x][temp.y] != color && !Live(temp,color%2+1,board,visited) && board[temp.x][temp.y] != BORDER) {
                                if(temp_color == 0) {
                                        board[pMove.x][pMove.y] = 0;
                                        return 2;
                                }
                                board[pMove.x][pMove.y] = color%2+1;
                                return 3;
                        }
                        temp.y = pMove.y;
                }
                board[pMove.x][pMove.y] = temp_color;       //再把这个子变回来
        }
        return 0;
}

//位置，颜色，棋盘，劫board
static int Clean(Move pMove,int color,int board[][SIZE],Move* jie){                  //清除棋子
        int visited[SIZE][SIZE];
        int num = 0;
        Move temp = pMove;
        for (int i = -1; i <= 1; i+=2) {
                temp.x = pMove.x + i;
                memset(visited, 0, sizeof(visited));
                if (board[temp.x][temp.y] == color && !Live(temp,color,board,visited)) {
                        num += Remove(temp,color,board,jie);
                }
                temp.x = pMove.x;
                temp.y = pMove.y + i;
                memset(visited, 0, sizeof(visited));
                if (board[temp.x][temp.y] == color && !Live(temp,color,board,visited)) {
                        num += Remove(temp,color,board,jie);
                }
                temp.y = pMove.y;
        }
        return num;
}

static bool calEnd(int board[][SIZE],int color1,int color2,int *a,int *b){   //判断对局是否结束
        int ChessZone = 0;
        int OppoZone = 0; //初始化
        Move temp;
        for (temp.x = 1; temp.x <= 9; temp.x++) {      //遍历整个棋盘
                for (temp.y = 1; temp.y <= 9; temp.y++) {
                        if(board[temp.x][temp.y] == color1) {
                                ChessZone++;
                        }else if(board[temp.x][temp.y] == color2) {
                                OppoZone++;
                        }else if(board[temp.x][temp.y] == NOSTONE) {
                                Move nTemp = temp;
                                for (int i = -1; i <= 1; i+=2) {
                                        nTemp.x = temp.x + i;
                                        if (board[nTemp.x][nTemp.y] == NOSTONE) {
                                                return false;   //棋局未结束
                                        }else if(board[nTemp.x][nTemp.y] == color1) {
                                                ChessZone++;
                                                break;
                                        }else if(board[nTemp.x][nTemp.y] == color2) {
                                                OppoZone++;
                                                break;
                                        }
                                        nTemp.x = temp.x;
                                        nTemp.y = temp.y + i;
                                        if (board[nTemp.x][nTemp.y] == NOSTONE) {
                                                return false;   //棋局未结束
                                        }else if(board[nTemp.x][nTemp.y] == color1) {
                                                ChessZone++;
                                                break;
                                        }else if(board[nTemp.x][nTemp.y] == color2) {
                                                OppoZone++;
                                                break;
                                        }
                                        nTemp.y = temp.y;
                                }
                        }
                }
        }
        *a = ChessZone;
        *b = OppoZone;
        return true;
}

static bool isSureFalseEye(Move pMove,int color,int board[][SIZE]){    //判断某点是否为真的假眼
        if(!isEye(pMove,color,board))
                return false;
        Move temp = pMove;
        int boardNum = 0;
        int enemyNum = 0;
        for (int i = -1; i <= 1; i+=2) {
                temp.x = pMove.x + i;
                temp.y = pMove.y + i;
                if (board[temp.x][temp.y] == color%2+1) {
                        enemyNum++;
                }else if(board[temp.x][temp.y] == BORDER) {
                        boardNum++;
                }
                temp.x = pMove.x + i;
                temp.y = pMove.y - i;
                if (board[temp.x][temp.y] == color%2+1) {
                        enemyNum++;
                }else if(board[temp.x][temp.y] == BORDER) {
                        boardNum++;
                }
        }
        switch(boardNum) {
        case 0: {
                if(enemyNum >= 2)
                        return true;
                else
                        return false;
        }
        case 2: {
                if(enemyNum >= 1)
                        return true;
                else
                        return false;
        }
        case 3: {
                if(enemyNum >= 1)
                        return true;
                else
                        return false;
        }
        default: return false;
        }
}

//颜色，棋盘，劫棋盘，试探已知棋盘
static int isEnd(int color,int board[][SIZE],Move jie,int board_know[][SIZE]){
        Move temp;
        int a;
        int availableNum = 0;
        for(temp.x = 1; temp.x <= 9; temp.x++) {
                for(temp.y = 1; temp.y <= 9; temp.y++) {
                        a = Check(temp,color,board,board_know);
                        if((a == 1 || a == 2) && (jie.x != temp.x || jie.y != temp.y) && (!isEye(temp,color,board) || isSureFalseEye(temp,color,board))) {
                                availableNum++;
                        }
                }
        }
        return availableNum;
}

static int SearchVoidPoint(int board[][SIZE]){    //计算棋盘上的空点数
        Move temp;
        int res = 0;
        for (temp.x = 1; temp.x <= 9; temp.x++) {
                for (temp.y = 1; temp.y <= 9; temp.y++) {
                        if(board[temp.x][temp.y] == NOSTONE)
                                res++;
                }
        }
        return res;
}

static void clearEye(){
        ChessEye = 0;
        OppoEye = 0;
        Move temp;
        for (temp.x = 1; temp.x <= 9; temp.x++) {
                for (temp.y = 1; temp.y <= 9; temp.y++) {
                        if(board[temp.x][temp.y] == NOSTONE) {
                                if((board[temp.x - 1][temp.y] == ChessColor || board[temp.x - 1][temp.y] == BORDER) &&
                                   (board[temp.x + 1][temp.y] == ChessColor || board[temp.x + 1][temp.y] == BORDER) &&
                                   (board[temp.x][temp.y - 1]  == ChessColor || board[temp.x][temp.y - 1]  == BORDER) &&
                                   (board[temp.x][temp.y + 1] == ChessColor || board[temp.x][temp.y + 1] == BORDER)) {
                                        ChessEye++;
                                }else if((board[temp.x - 1][temp.y] == OppoColor || board[temp.x - 1][temp.y] == BORDER) &&
                                         (board[temp.x + 1][temp.y] == OppoColor || board[temp.x + 1][temp.y] == BORDER) &&
                                         (board[temp.x][temp.y - 1]  == OppoColor || board[temp.x][temp.y - 1]  == BORDER) &&
                                         (board[temp.x][temp.y + 1] == OppoColor || board[temp.x][temp.y + 1] == BORDER)) {
                                        OppoEye++;
                                }
                        }
                }
        }
}

static int MonteCarloSearch(Data *data){         //随机走子，直到终局
        //参数传递与存储
        int tempBoard[SIZE][SIZE];               //临时棋盘
        int totalBoard[SIZE][SIZE];              //总次数棋盘
        int winBoard[SIZE][SIZE];                //胜利棋盘
        int board_know[SIZE][SIZE];              //探知有敌方子的棋盘
        int ChessColor = data->ChessColor;       //己方颜色
        int OppoColor = data->OppoColor;         //敌方颜色
        int OppoNum = data->OppoNum;             //敌方数量
        int OppoNumKnow = data->OppoNum;     //敌方已知棋子数
        Move jie;
        jie.x = data->jie.x;
        jie.y = data->jie.y;
        for(int i = 0; i <= 10; i++) {
                for(int j = 0; j <= 10; j++) {
                        tempBoard[i][j] = data->tempBoard[i][j];
                        totalBoard[i][j] = data->totalBoard[i][j];
                        winBoard[i][j] = data->winBoard[i][j];
                        board_know[i][j] = data->board_know[i][j];
                }
        }
        int initBoard[SIZE][SIZE];       //初始棋盘赋值     临时存储
        for(int i = 0; i <= 10; i++) {
                for(int j = 0; j <= 10; j++) {
                        initBoard[i][j] = tempBoard[i][j];
                }
        }
        int ChessZone;      //己方区域
        int OppoZone;       //敌方区域
        int num;            //随机数
        int first;          //第一步标志
        int color;          //每局每下一步的颜色存储
        int OppoStone;      //敌方棋子数
        int entireBoard_temp[SIZE][SIZE];           //敌我双方棋子补全棋盘暂存
        Move jie_temp;         //劫的暂存
        Move temp;             //某一步
        Move beginner;         //第一步
        //开始mcmax2次从当前局面开始的模拟
        clock_t startTime,endTime;
        startTime = clock();
        jie_temp.x = jie.x;
        jie_temp.y = jie.y;
        while(1) {
                endTime = clock();
                if(static_cast<double>(endTime - startTime)/CLOCKS_PER_SEC >= LIMIT) break;
                //对局初始化
                for(int i = 0; i <= 10; i++) {
                        for(int j = 0; j <= 10; j++) {
                                tempBoard[i][j] = initBoard[i][j];
                        }
                }
                srand(rand()*(int)time(0)*17+rand());                        //产生随机数种子
                OppoStone = OppoNumKnow;
                int N = SearchVoidPoint(tempBoard);
                int Avail = 0;
                for (temp.x = 1; temp.x <= 9; temp.x++) {
                        for (temp.y = 1; temp.y <= 9; temp.y++) {
                                if(tempBoard[temp.x][temp.y] == 0 && (Check(temp, OppoColor,tempBoard,board_know) == 1))
                                        Avail++;
                        }
                }
                if(OppoNum - OppoStone <= Avail) {

                }else{
                        while (OppoNum > OppoStone) {                     //随机补全敌方棋子直至与己方棋子数目相同
                                num = 1+(int)(N)*double(rand())/(RAND_MAX+1.0);
                                for (temp.x = 1; temp.x <= 9; temp.x++) {
                                        for (temp.y = 1; temp.y <= 9; temp.y++) {
                                                if (tempBoard[temp.x][temp.y] == 0) {
                                                        num--;
                                                        if (num == 1 && (Check(temp, OppoColor,tempBoard,board_know) == 1)) {
                                                                tempBoard[temp.x][temp.y] = OppoColor;
                                                                OppoStone++;
                                                                temp.x = 10;
                                                                N--;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                        }
                }
                SaveBoard(entireBoard_temp,tempBoard);
                for(int i = 0; i < MCMAX2; i++) {
                        LoadBoard(tempBoard,entireBoard_temp); // 每局初始参数
                        jie.x = jie_temp.x;
                        jie.y = jie_temp.y;
                        ChessZone = 0;
                        OppoZone = 0;
                        first = 1;
                        color = ChessColor;
                        //开始一次模拟
                        while(1) {
                                int flag1 = 0; //每次初始参数
                                int flag2 = 0;
                                if(calEnd(tempBoard,ChessColor,OppoColor,&ChessZone,&OppoZone)) {
                                        int a = isEnd(ChessColor,tempBoard,jie,board_know);
                                        int b = isEnd(OppoColor,tempBoard,jie,board_know);
                                        if(a == 0 && b == 0)
                                                break;
                                        if(a == 0)
                                                color = OppoColor;
                                        if(b == 0)
                                                color = ChessColor;
                                }
                                while(first) {
                                        flag1 = 0;
                                        N = 81;
                                        srand(rand()*(int)time(0)*17+rand());
                                        num = 1 + (int)(N*double(rand())/(RAND_MAX+1.0)); //随机选择棋盘上一子
                                        for (temp.x = 1; temp.x <= 9; temp.x++) {
                                                for (temp.y = 1; temp.y <= 9; temp.y++) {
                                                        if(num == 1) {
                                                                if(Check(temp,color,tempBoard,board_know)) { //如果此子是合法的
                                                                        if(jie.x != temp.x || jie.y != temp.y) { //如果此子不是劫
                                                                                if(!isEye(temp,color,tempBoard) || isSureFalseEye(temp,color,tempBoard)) {
                                                                                        beginner.x = temp.x;
                                                                                        beginner.y = temp.y;
                                                                                        totalBoard[beginner.x][beginner.y]++;
                                                                                        first = 0;
                                                                                        tempBoard[temp.x][temp.y] = ChessColor;
                                                                                        jie.x = 0;
                                                                                        jie.y = 0;
                                                                                        if(Clean(temp,OppoColor,tempBoard,&jie) != 1) {
                                                                                                jie.x = 0;
                                                                                                jie.y = 0;
                                                                                        }
                                                                                        color = color%2+1;
                                                                                } //如果此子不是己方眼或者是确定的假眼
                                                                        }
                                                                }
                                                                flag1 = 1;
                                                                break;
                                                        }
                                                        num--;
                                                }
                                                if(flag1)
                                                        break;
                                        }
                                }
                                N = SearchVoidPoint(tempBoard); //获取棋盘上空子数目
                                srand(rand()*(int)time(0)*17+rand());
                                num = 1 + (int)(N*double(rand())/(RAND_MAX+1.0)); //随机选择一空子c
                                for (temp.x = 1; temp.x <= 9; temp.x++) {
                                        for (temp.y = 1; temp.y <= 9; temp.y++) {
                                                //cout << "x: " << temp.x << " y: " << temp.y << endl;
                                                if(tempBoard[temp.x][temp.y] == NOSTONE) {
                                                        if(num <= 1) {
                                                                if(Check(temp,color,tempBoard,board_know) == 1 || Check(temp,color,tempBoard,board_know) == 2) {
                                                                        if(jie.x != temp.x || jie.y != temp.y) {
                                                                                if(!isEye(temp,color,tempBoard) || isSureFalseEye(temp,color,tempBoard)) {
                                                                                        if(color == ChessColor) {
                                                                                                tempBoard[temp.x][temp.y] = ChessColor;
                                                                                                jie.x = 0;
                                                                                                jie.y = 0;
                                                                                                if(Clean(temp,OppoColor,tempBoard,&jie) != 1) {
                                                                                                        jie.x = 0;
                                                                                                        jie.y = 0;
                                                                                                }
                                                                                                color = color%2+1;
                                                                                                flag2 = 1;
                                                                                                break;
                                                                                        }
                                                                                        else if(color == OppoColor) {
                                                                                                tempBoard[temp.x][temp.y] = OppoColor;
                                                                                                jie.x = 0;
                                                                                                jie.y = 0;
                                                                                                if(Clean(temp,ChessColor,tempBoard,&jie) != 1) {
                                                                                                        jie.x = 0;
                                                                                                        jie.y = 0;
                                                                                                }
                                                                                                color = color%2+1;
                                                                                                flag2 = 1;
                                                                                                break;
                                                                                        }
                                                                                } //如果此子不是己方眼或者是确定的假眼
                                                                        }
                                                                }
                                                        }
                                                        num--;
                                                }
                                        }
                                        if(flag2)
                                                break;
                                }
                        }
                        calEnd(tempBoard,ChessColor,OppoColor,&ChessZone,&OppoZone);
                        if (ChessZone > OppoZone)
                                winBoard[beginner.x][beginner.y]++;

                }
        }
        for(int i = 0; i <= 10; i++) {
                for(int j = 0; j <= 10; j++) {
                        data->totalBoard[i][j] = totalBoard[i][j];
                        data->winBoard[i][j] = winBoard[i][j];
                }
        }
        return 1;
}

static int MonteCarlo(Move *pMove){
        Move best;
        boardCopy1();
        int ChessZone = 0;
        int OppoZone = 0;
        std::vector<std::future<int>> futures;
        if(calEnd(tempBoard,ChessColor,OppoColor,&ChessZone,&OppoZone)) {
                if(isEnd(ChessColor,tempBoard,jie,board_know) == 0)
                        return 0;
        }
        memset(markBoard,0,sizeof(markBoard));
        Data *data = new Data[Thread_Count];
        for(int i = 0; i < Thread_Count; i++){
                data[i].ChessColor = ChessColor;
                data[i].OppoColor = OppoColor;
                data[i].OppoNum = OppoNum;
                data[i].OppoNumKnow = OppoNumKnow;
                data[i].jie.x = jie.x;
                data[i].jie.y = jie.y;
                memset(data[i].totalBoard,0,sizeof(data[i].totalBoard));
                memset(data[i].winBoard,0,sizeof(data[i].winBoard));
        }
        for(int index = 0; index < Thread_Count; index++){
                for(int i = 0; i <= 10; i++) {
                        for(int j = 0; j <= 10; j++) {
                                data[index].tempBoard[i][j] = board[i][j];
                                data[index].board_know[i][j] = board_know[i][j];
                        }
                }
        }
        for (int i = 0; i < Thread_Count; ++i){
		auto f = std::async(std::launch::async,
				    &MonteCarloSearch,
                    data + i);
		futures.push_back(std::move(f));
	}
	for (std::future<int> &future : futures){
		future.wait();
	}
        for(int index = 0; index < Thread_Count; index++){
                for(int i = 1; i <= 9; i++) {
                        for(int j = 1; j <= 9; j++) {
                                markBoard[i][j].total += data[index].totalBoard[i][j];
                                markBoard[i][j].win += data[index].winBoard[i][j];
                        }
                }
        }
        float rate = 0;
        float maxRate = 0;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        if(markBoard[i][j].total != 0) {
                                rate = (float)markBoard[i][j].win/markBoard[i][j].total;
                        }else
                                rate = 0;
                        if(rate >= maxRate  && markBoard[i][j].total > 0.1*MCMAX1) {
                                maxRate = rate;
                                best.x = i;
                                best.y = j;
                        }
                }
        }
        float r;
        cout << endl;
        cout << "total: "<< endl;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        cout << setw(6) << markBoard[i][j].total;
                }
                cout << endl;
        }
        cout << endl;
        cout << "win: " << endl;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        cout << setw(6) << markBoard[i][j].win;
                }
                cout << endl;
        }
        cout << endl;
        int zero = 0;
        int number = 0;
        float finally;
        float sum = 0;
        cout << "rate: " << endl;
        for(int i = 1; i <= 9; i++) {
                for(int j = 1; j <= 9; j++) {
                        if(markBoard[i][j].total != 0) {
                                number++;
                                r = (float)markBoard[i][j].win/markBoard[i][j].total;
                                sum += r;
                                finally = r;
                        }else
                                r = 0;
                        cout << setprecision(3) << setw(6) << r;
                        if(r == 0)
                                zero++;
                }
                cout << endl;
        }
        if(zero == 80) {
                if(finally < 0.1)
                        return false;
        }
        float average = sum/number;
        cout << endl;
        cout << "     average: " << average << endl;
        cout << "     x: " << best.x << " y: " << best.y << endl;
        cout << "     rate: " << maxRate << endl;
        if (maxRate >= believePoint) {     //评分大于信任值采用蒙特卡洛搜索算法
                *pMove = best;
                return true;
        }
        return false;
}

static bool SearchMove(Move* pMove){
        //(uint32_t seed, int mc_game_count_per_move, int thread_count)
        foolgo::UctPlayer<foolgo::MAIN_BOARD_LEN> temp(seed,History_Count_per_Move, Future_Count_Per_Move, Thread_Count);
        full_board.setOppoNum(OppoNum);
        full_board.setOppoNumKnow(OppoNumKnow);
        full_board.setChessColor(Chess);
        full_board.setOppoColor(Oppo);
        full_board.setjiex(jie.x);
        full_board.setjiey(jie.y);
        clearEye();
        int Zone = ChessNum + ChessEye + OppoNumKnow + OppoEye;
        cout << "Zone: " << Zone << endl;
        cout << "blazar is thinking …… " << endl;
        if(Zone >= 77) {
                terminal = 1;
        }
        if(terminal) {
                if(MonteCarlo(&bestMove))
                        return 1;
                else
                        return 0;
        }
        foolgo::PositionIndex result = temp.NextMoveWithPlayableBoard(full_board);
        if(result >= 0 && result <= 80) {
                int x = (result + 1)/9 + 1;
                int y = (result + 1)%9;
                if(y == 0) {
                        x--;
                        y = 9;
                }
                pMove->x = x;
                pMove->y = y;
                return 1;
        }else if(MonteCarlo(&bestMove))
                return 1;
        return 0;
}

#endif
