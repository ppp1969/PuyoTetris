#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimerEvent>
#include "games.h"
#include "gameoverwindow.h"

// GameWindow 클래스: 게임 창을 관리하는 클래스
class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QString gamename, QWidget *parent = nullptr);
    ~GameWindow();

protected:
    // paintEvent: 그리기 이벤트 처리 함수
    void paintEvent(QPaintEvent *event);
    // keyPressEvent: 키보드 입력 이벤트 처리 함수
    void keyPressEvent(QKeyEvent *event);
    // timerEvent: 타이머 이벤트 처리 함수
    void timerEvent(QTimerEvent *event);

private:
    // drawNext: 다음 나올 조각을 그리는 함수
    void drawNext();
    // getColorFromCode: 색상 코드를 받아 QColor 객체를 반환하는 함수
    QColor getColorFromCode(int colorCode);

    Game *p_game;  // 현재 게임 객체
    QString GAMENAME;  // 게임 이름
    int timer;  // 타이머 ID
    GameoverWindow *gameoverwindow;  // 게임 오버 창

    // 상수들
    static const int BLOCK_SIZE = 30;  // 블록 크기
    static const int MARGIN = 5;  // 여백 크기
    static const int SPEED_MS = 500;  // 타이머 속도 (밀리초 단위)
};

#endif // GAMEWINDOW_H
