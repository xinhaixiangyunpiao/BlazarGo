#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFont>
#include <QTimer>
#include <thread>
#include <QHBoxLayout>
#include "convey.h"

convey con;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = nullptr;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags); // 设置禁止最大化
    setFixedSize(1055,720); // 禁止改变窗口大小
    QScrollArea* scrollArea = new QScrollArea(ui->centralWidget);
    scrollArea->setWidget(ui->label_7);    //设置为你想添加滚动条的widget
    scrollArea->setWidgetResizable(true);
    scrollArea->setGeometry(QRect(715, 400, 321, 281));
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    InitGame();
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    timer->start(10);
    /** 建立Slider与label的连接 **/
    connect(ui->horizontalSlider,&QSlider::valueChanged,
            [=]{
                History_Count_per_Move = (ui->horizontalSlider)->sliderPosition();
                (ui->label_2)->setText(QString::number(History_Count_per_Move));
            }
            );
    connect(ui->horizontalSlider_2,&QSlider::valueChanged,
            [=]{
                Future_Count_Per_Move = (ui->horizontalSlider_2)->sliderPosition();
                (ui->label_4)->setText(QString::number(Future_Count_Per_Move));
            }
            );
    connect(ui->horizontalSlider_3,&QSlider::valueChanged,
            [=]{
                Thread_Count = (ui->horizontalSlider_3)->sliderPosition();
                (ui->label_6)->setText(QString::number(Thread_Count));
            }
            );
    connect(ui->action_4,&QAction::triggered,
            [=]{
                info = ">>  new\n>>  black\n";
                ChessColor = BLACK;
                OppoColor = WHITE;
                Chess = foolgo::BLACK_FORCE;
                Oppo = foolgo::WHITE_FORCE;
                std::thread MoveThread(move,this);
                MoveThread.detach();
            }
            );
    connect(ui->action_6,&QAction::triggered,
            [=]{
                info = ">>  new\n>>  white\n";
                ChessColor = WHITE;
                OppoColor = BLACK;
                Chess = foolgo::WHITE_FORCE;
                Oppo = foolgo::BLACK_FORCE;
            }
            );
    /* 按钮事件 */
    connect(ui->pushButton,&QPushButton::released,
            [=]{
                if(board[bestMove.x][bestMove.y] == ChessColor) {

                }else if(board[bestMove.x][bestMove.y] == NOSTONE) {   //如果是空
                        board[bestMove.x][bestMove.y] = ChessColor;
                        DisplayBoard[bestMove.x][bestMove.y] = ChessColor + '0';
                        full_board.PlayMove(foolgo::Move(Chess,convert(bestMove)));
                        ChessNum++;
                        Step++;
                }else if(board[bestMove.x][bestMove.y] == OppoColor) {  //如果是对手棋子
                        board[bestMove.x][bestMove.y] = ChessColor;
                        DisplayBoard[bestMove.x][bestMove.y] = ChessColor + '0';
                        ChessNum++;
                        Step++;
                        OppoNumKnow--;
                        OppoNum--;
                }
                jie.x = 0;
                jie.y = 0;
                info += ">>  accept\n";
            }
            );
    connect(ui->pushButton_2,&QPushButton::released,
            [=]{
                if(board[bestMove.x][bestMove.y] != OppoColor) {
                        board[bestMove.x][bestMove.y] = OppoColor;
                        DisplayBoard[bestMove.x][bestMove.y] = OppoColor + '0';
                        OppoNumKnow++;
                }
                board_know[bestMove.x][bestMove.y]++;      //试探次数加一
                full_board.PlayMove4(foolgo::Move(Oppo,convert(bestMove)));
                info += ">>  refuse\n";
                std::thread MoveThread(move,this);
                MoveThread.detach();
            }
            );
    connect(ui->pushButton_3,&QPushButton::released,
            [=]{
                record();
                jie.x = 0;
                jie.y = 0;          //清空劫的棋盘
                OppoNum++;
                std::thread MoveThread(move,this);
                MoveThread.detach();
            }
            );
    connect(ui->pushButton_4,&QPushButton::released,
            [=]{
                undo();
            }
            );
    connect(ui->pushButton_5,&QPushButton::released,
            [=]{
                if(takeflag){
                    (ui->centralWidget)->setMouseTracking(false);
                    (ui->frame_2)->setMouseTracking(false);
                    setMouseTracking(false);
                    takeflag = 0;
                    int num = 0;
                    char msg[256];
                    int index = 0;
                    for(int i = 1; i <= 9; i++){
                        for(int j = 1; j <= 9; j++){
                            if(TokenBoard[i][j]){
                                num++;
                                msg[index++] = j + 'A' - 1;
                                msg[index++] = i + 'A' - 1;
                            }
                        }
                    }
                    info += ">>  take ";
                    for(int i = 0; i < num; i++){
                        info += msg[2*i+1];
                        info += 'A'+'9'-msg[2*i];
                    }
                    info += "\n";
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
                                    DisplayBoard[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1]=NOSTONE+'0';
                            }else if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == NOSTONE) {
                                    OppoNum--;
                                    board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                                    DisplayBoard[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE+'0';
                            }
                            ssmove.x = msg[2*i] - 'A' + 1;
                            ssmove.y = msg[2*i+1] - 'A' + 1;
                            foolgo::PositionIndex a = convert(ssmove);
                            if(full_board.GetPointState(a) != Oppo)
                                    full_board.PlayMove2(foolgo::Move(Oppo,a));
                    }
                }else{
                    (ui->centralWidget)->setMouseTracking(true);
                    (ui->frame_2)->setMouseTracking(true);
                    setMouseTracking(true);
                    takeflag = 1;
                    (ui->pushButton_5)->setText("确定");
                }
                for(int i = 0; i <= 10; i++){
                    for(int j = 0; j <= 10; j++){
                        TokenBoard[i][j] = 0;
                    }
                }
                current_mouse_index_x = 0;
                current_mouse_index_y = 0;
            }
            );
    connect(ui->pushButton_6,&QPushButton::released,     //  taked
            [=]{
                if(takedflag){
                    (ui->centralWidget)->setMouseTracking(false);
                    (ui->frame_2)->setMouseTracking(false);
                    setMouseTracking(false);
                    takedflag = 0;
                    int num = 0;
                    char msg[256];
                    int index = 0;
                    for(int i = 1; i <= 9; i++){
                        for(int j = 1; j <= 9; j++){
                            if(TokenBoard[i][j]){
                                num++;
                                msg[index++] = j + 'A' - 1;
                                msg[index++] = i + 'A' - 1;
                            }
                        }
                    }
                    info += ">>  taked ";
                    for(int i = 0; i < num; i++){
                        info += msg[2*i+1];
                        info += 'A'+'9'-msg[2*i];
                    }
                    info += "\n";
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
                                            DisplayBoard[x - 1][y] = OppoColor+'0';
                                            OppoNumKnow++;
                                    }
                                    if (board[x + 1][y] == NOSTONE)
                                    {
                                            ppmove.x = x + 1;
                                            ppmove.y = y;
                                            foolgo::PositionIndex a = convert(ppmove);
                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
                                            board[x + 1][y] = OppoColor;
                                            DisplayBoard[x + 1][y] = OppoColor+'0';
                                            OppoNumKnow++;
                                    }
                                    if (board[x][y - 1] == NOSTONE)
                                    {
                                            ppmove.x = x;
                                            ppmove.y = y - 1;
                                            foolgo::PositionIndex a = convert(ppmove);
                                            full_board.PlayMove3(foolgo::Move(Oppo,a));
                                            board[x][y - 1] = OppoColor;
                                            DisplayBoard[x][y - 1] = OppoColor+'0';
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
                                            DisplayBoard[x][y + 1] = OppoColor+'0';
                                            OppoNumKnow++;
                                    }
                            }
                    }
                    for(int i = 0; i < num; i++) {
                            if(board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] == ChessColor) {
                                    board[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                                    DisplayBoard[msg[2*i] - 'A' + 1][msg[2*i+1] - 'A' + 1] = NOSTONE;
                            }
                    }
                    OppoNum++;
                    for(int i = 0; i <= 10; i++){
                        for(int j = 0; j <= 10; j++){
                            TokenBoard[i][j] = 0;
                        }
                    }
                    current_mouse_index_x = 0;
                    current_mouse_index_y = 0;
                    std::thread MoveThread(move,this);
                    MoveThread.detach();
                }else{
                    (ui->centralWidget)->setMouseTracking(true);
                    (ui->frame_2)->setMouseTracking(true);
                    setMouseTracking(true);
                    takedflag = 1;
                    (ui->pushButton_6)->setText("确定");
                }
            }
            );

}

void MainWindow::record(){
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

void MainWindow::InitGame(){
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

foolgo::PositionIndex MainWindow::convert(Move pMove){
        int x = pMove.x;
        int y = pMove.y;
        foolgo::PositionIndex result = static_cast<foolgo::PositionIndex>((x - 1)*9 + y - 1);
        return result;
}

void MainWindow::undo(){
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

void MainWindow::paintEvent(QPaintEvent*){
    /* 绘制棋盘 */
    p = new QPainter(this);
    u_int16_t begin = 60;
    u_int16_t end = 620;
    float step = static_cast<float>(end - begin)/8;
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    p->setPen(pen);
    int Row[9],Column[9];
    int8_t cnt = 1;
    for(int i = begin; i <=end; i+=step){
        p->drawLine(begin+15,i + 28,end+15,i + 28);
        p->drawLine(i+15,begin + 28,i+15,end + 28);
        Row[cnt] = i+15;
        Column[cnt++] = i+28;
    }
    QFont font1("宋体",15,QFont::Bold,false);
    font1.setLetterSpacing(QFont::AbsoluteSpacing,56);
    p->setFont(font1);
    p->setPen(Qt::black);
    p->drawText(68,60,tr("123456789"));
    font1.setLetterSpacing(QFont::AbsoluteSpacing,54);
    p->setFont(font1);
    p->drawText(68,695,tr("ABCDEFGHI"));
    cnt = 0;
    for(int i = 95; i < 710; i += 70){
        p->drawText(30,i,QString('9'-(cnt++)));
    }
    cnt = 0;
    for(int i = 95; i < 710; i += 70){
        p->drawText(665,i,QString('I'-(cnt++)));
    }

    /* 绘制选中提示线 */
    pen.setColor(QColor::fromRgb(242, 152, 177));
    p->setPen(pen);
    p->drawLine(Row[current_mouse_index_x]-20,Column[current_mouse_index_y]-20,Row[current_mouse_index_x]-20,Column[current_mouse_index_y]-10);
    p->drawLine(Row[current_mouse_index_x]-20,Column[current_mouse_index_y]-20,Row[current_mouse_index_x]-10,Column[current_mouse_index_y]-20);
    p->drawLine(Row[current_mouse_index_x]-20,Column[current_mouse_index_y]+20,Row[current_mouse_index_x]-20,Column[current_mouse_index_y]+10);
    p->drawLine(Row[current_mouse_index_x]-20,Column[current_mouse_index_y]+20,Row[current_mouse_index_x]-10,Column[current_mouse_index_y]+20);
    p->drawLine(Row[current_mouse_index_x]+20,Column[current_mouse_index_y]+20,Row[current_mouse_index_x]+20,Column[current_mouse_index_y]+10);
    p->drawLine(Row[current_mouse_index_x]+20,Column[current_mouse_index_y]+20,Row[current_mouse_index_x]+10,Column[current_mouse_index_y]+20);
    p->drawLine(Row[current_mouse_index_x]+20,Column[current_mouse_index_y]-20,Row[current_mouse_index_x]+10,Column[current_mouse_index_y]-20);
    p->drawLine(Row[current_mouse_index_x]+20,Column[current_mouse_index_y]-20,Row[current_mouse_index_x]+20,Column[current_mouse_index_y]-10);
    p->setPen(Qt::black);
    /* 绘制棋子 */
    int Radius = 50;
    QBrush brush1(QColor(0,0,0), Qt::SolidPattern);
    QBrush brush2(QColor(255,255,255), Qt::SolidPattern);
    QBrush brush3(QColor(255,25,25), Qt::Dense4Pattern);
    QBrush brush4(QColor(255,225,225), Qt::Dense4Pattern);
    for(int i = 1; i <= 9; i++){
        for(int j = 1; j <= 9; j++){
            switch(DisplayBoard[j][i]){
                case '0':  //空子

                break;
                case '1':  //黑子
                    p->setBrush(brush1);
                    p->drawEllipse(Row[i]-Radius/2,Column[j]-Radius/2,Radius,Radius);
                break;
                case '2':  //白子
                    p->setBrush(brush2);
                    p->drawEllipse(Row[i]-Radius/2,Column[j]-Radius/2,Radius,Radius);
                break;
                case '3':  //黑子未落
                    p->setBrush(brush3);
                    p->drawEllipse(Row[i]-Radius/2,Column[j]-Radius/2,Radius,Radius);
                break;
                case '4':  //白子未落
                    p->setBrush(brush4);
                    p->drawEllipse(Row[i]-Radius/2,Column[j]-Radius/2,Radius,Radius);
                break;
                default: break;
            }
            if(TokenBoard[i][j]){
                pen.setColor(Qt::red);
                p->setPen(pen);
                p->drawLine(Row[i]-20,Column[j]-20,Row[i]-20,Column[j]-10);
                p->drawLine(Row[i]-20,Column[j]-20,Row[i]-10,Column[j]-20);
                p->drawLine(Row[i]-20,Column[j]+20,Row[i]-20,Column[j]+10);
                p->drawLine(Row[i]-20,Column[j]+20,Row[i]-10,Column[j]+20);
                p->drawLine(Row[i]+20,Column[j]+20,Row[i]+20,Column[j]+10);
                p->drawLine(Row[i]+20,Column[j]+20,Row[i]+10,Column[j]+20);
                p->drawLine(Row[i]+20,Column[j]-20,Row[i]+10,Column[j]-20);
                p->drawLine(Row[i]+20,Column[j]-20,Row[i]+20,Column[j]-10);
                p->setPen(Qt::black);
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *m)
{
    int X_min = 75;
    int X_max = 635;
    int Y_min = 88;
    int Y_max = 648;
    float x_step = (X_max - X_min)/8;
    float y_step = (Y_max - Y_min)/8;
    if(takeflag || takedflag){
        int x = m->pos().x();
        int y = m->pos().y();
        if(x >= X_min-x_step/2 && x <= X_max+y_step/2 && y <= Y_max+y_step/2 && y >= Y_min-y_step/2){
            current_mouse_index_x = (x - (X_min - x_step/2))/x_step+1;
            current_mouse_index_y = (y - (Y_min - y_step/2))/y_step+1;
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *m)
{
    int X_min = 75;
    int X_max = 635;
    int Y_min = 88;
    int Y_max = 648;
    float x_step = (X_max - X_min)/8;
    float y_step = (Y_max - Y_min)/8;
    if(takeflag || takedflag){
        int x = m->pos().x();
        int y = m->pos().y();
        if(x >= X_min-x_step/2 && x <= X_max+y_step/2 && y <= Y_max+y_step/2 && y >= Y_min-y_step/2){
            int x_index = (x - (X_min - x_step/2))/x_step+1;
            int y_index = (y - (Y_min - y_step/2))/y_step+1;
            TokenBoard[x_index][y_index] = 1;
        }
    }
}

void MainWindow::timerUpDate(){
    (ui->progressBar)->setValue(con.GetCurrentPercent());
    (ui->label_7)->setText(info);
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}
