#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <string.h>
#include <QMouseEvent>
#include "search.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void record();
    void InitGame();
    foolgo::PositionIndex convert(Move pMove);
    static void move(MainWindow* w){
        w->info += ">>  move\n";
        if(SearchMove(&bestMove)) {    //搜索到最优步
                w->DisplayBoard[bestMove.x][bestMove.y] = ChessColor + '2';
                w->info += QString::fromStdString(con.GetInformation());
                w->info += "move " + QString(bestMove.y + 'A' - 1) + QString(9 - bestMove.x + '1') + "\n";
        }else{                         //搜索失败
                printf("move pass\n");
        }

    }
    void undo();
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *m);
    void mouseReleaseEvent(QMouseEvent *m);

private slots:
    void timerUpDate();

private:
    Ui::MainWindow *ui;
    QPainter *p;
    int step = 0;                                                   //总步数
    char DisplayBoard[11][11] = {0};                                //输出显示接口   0空白 1黑子 2白子 3黑子未落 4白子未落
    QString info;                                                   //控制台显示接口
    bool takeflag = 0;
    bool takedflag = 0;
    bool TokenBoard[11][11] = {0};
    int current_mouse_index_x = 0;
    int current_mouse_index_y = 0;
    struct STATE{
        int board[SIZE][SIZE];
        int board_know[SIZE][SIZE];
        int ChessNum;
        int OppoNum;
        int OppoNumKnow;
        int ChessColor;
        int OppoColor;
        Move jie;
        Move bestMove;
        char DisplayBoard[11][11];
        QString info;
        bool takeflag;
        bool takedflag;
        bool TokenBoard[11][11];
        foolgo::FullBoard<foolgo::MAIN_BOARD_LEN> full_board;
    };
    STATE GAMING[300];
};

#endif // MAINWINDOW_H
