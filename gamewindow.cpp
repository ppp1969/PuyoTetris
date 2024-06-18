#include "gamewindow.h"
#include "gameoverwindow.h"
#include <QColor>

// GameWindow 생성자
GameWindow::GameWindow(QString gamename, QWidget *parent) : QWidget(parent)
{
    timer = startTimer(SPEED_MS);  // 타이머 시작
    GAMENAME = gamename;

    if (GAMENAME == "Tetris")
    {
        p_game = new TetrisGame();
        resize(500, 800);  // 테트리스 창 크기 조정
    }
    else if (GAMENAME == "Puyopuyo")
    {
        p_game = new PuyopuyoGame();
        resize(360, 720);  // 뿌요뿌요 창 크기 조정
    }
    else if (GAMENAME == "PuyopuyoTetris")
    {
        p_game = new PuyopuyoTetrisGame();
        resize(480, 720);  // 뿌요뿌요 테트리스 창 크기 조정
    }
    setWindowTitle(gamename);
}

// GameWindow 소멸자
GameWindow::~GameWindow()
{
    delete p_game;  // 게임 객체 삭제
    delete gameoverwindow;  // 게임 오버 창 삭제
}

// 색상 코드를 받아 QColor 객체를 반환
QColor GameWindow::getColorFromCode(int colorCode) {
    switch (colorCode) {
    case 1: return QColor(186, 85, 211);  // 보라색
    case 2: return QColor(255, 165, 0);  // 주황색
    case 3: return QColor(30, 144, 255);  // 파란색
    case 4: return QColor(60, 179, 113);  // 초록색
    case 5: return QColor(255, 69, 58);  // 빨간색
    case 6: return QColor(0, 206, 209);  // 하늘색
    case 7: return QColor(255, 215, 0);  // 노란색
    default: return QColor(255, 255, 255);  // 흰색
    }
}

// 그리기 이벤트 처리
void GameWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter.drawRect(MARGIN, MARGIN, p_game->COL * BLOCK_SIZE, p_game->ROW * BLOCK_SIZE);

    if (GAMENAME == "Tetris") {
        // 테트리스 그리기
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(p_game->COL * BLOCK_SIZE + 2 * MARGIN, MARGIN, BLOCK_SIZE * 5, BLOCK_SIZE), Qt::AlignCenter, "NEXT");

        std::queue<int> nextPieces = static_cast<TetrisGame*>(p_game)->nextPieces;
        for (size_t k = 0; k < 5; ++k)
        {
            if (nextPieces.empty())
                break;

            int piece = nextPieces.front();
            nextPieces.pop();
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (TetrisGame::SHAPES[piece][i][j])
                    {
                        QRect rect(p_game->COL * BLOCK_SIZE + 2 * MARGIN + j * BLOCK_SIZE / 2, (k + 1) * 4 * BLOCK_SIZE / 2 + i * BLOCK_SIZE / 2, BLOCK_SIZE / 2, BLOCK_SIZE / 2);
                        painter.setBrush(getColorFromCode(TetrisGame::COLORS[piece]));
                        painter.drawRect(rect);
                    }
                }
            }
        }

        for (int i = 0; i < p_game->ROW; i++)
            for (int j = 0; j < p_game->COL; j++)
            {
                painter.setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));
                painter.drawRect(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);

                if (p_game->board[i][j] != 0)
                {
                    painter.setBrush(QBrush(getColorFromCode(p_game->board[i][j]), Qt::SolidPattern));
                    painter.drawRect(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
                }
            }

        std::vector<std::pair<int, int>> coordinates = static_cast<TetrisGame*>(p_game)->getPieceCoordinates();
        for (auto coord : coordinates)
        {
            painter.setBrush(getColorFromCode(TetrisGame::COLORS[static_cast<TetrisGame*>(p_game)->currentPiece]));
            painter.drawRect(coord.second * BLOCK_SIZE + MARGIN, coord.first * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
        }

        // 점수찍기
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(MARGIN, p_game->ROW * BLOCK_SIZE + 2 * MARGIN, p_game->COL * BLOCK_SIZE, BLOCK_SIZE), Qt::AlignCenter, "line score: " + QString::number(p_game->line_score));
    }
    else if (GAMENAME == "Puyopuyo") {
        // 뿌요뿌요 그리기
        for (int i = 0; i < p_game->ROW; i++) {
            for (int j = 0; j < p_game->COL; j++) {
                painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
                painter.drawRect(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);

                if (p_game->board[i][j] != 0) {
                    QColor color = getColorFromCode(p_game->board[i][j]);
                    painter.setBrush(color);
                    painter.drawEllipse(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
                }
            }
        }

        std::vector<std::pair<int, int>> coordinates = static_cast<PuyopuyoGame*>(p_game)->getPieceCoordinates();
        for (auto coord : coordinates) {
            int row = coord.first - static_cast<PuyopuyoGame*>(p_game)->currentRow;
            int col = coord.second - static_cast<PuyopuyoGame*>(p_game)->currentCol;
            QColor color = getColorFromCode(static_cast<PuyopuyoGame*>(p_game)->currentShape[row][col]);
            painter.setBrush(color);
            painter.drawEllipse(coord.second * BLOCK_SIZE + MARGIN, coord.first * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
        }

        // 큐에 저장된 다음 뿌요들 그리기
        int nextX = p_game->COL * BLOCK_SIZE + 2 * MARGIN;
        int nextY = MARGIN;
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(nextX, nextY, BLOCK_SIZE * 2, BLOCK_SIZE), Qt::AlignCenter, "NEXT");

        std::queue<std::pair<int, int>> nextPieces = static_cast<PuyopuyoGame*>(p_game)->nextPieces;
        int offsetY = 0;
        while (!nextPieces.empty()) {
            auto nextPiece = nextPieces.front();
            nextPieces.pop();

            QColor color1 = getColorFromCode(nextPiece.first);
            QColor color2 = getColorFromCode(nextPiece.second);

            painter.setBrush(color1);
            painter.drawEllipse(nextX, nextY + offsetY + BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            painter.setBrush(color2);
            painter.drawEllipse(nextX, nextY + offsetY + 2 * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

            offsetY += 3 * BLOCK_SIZE;  // 간격을 충분히 주어 겹치지 않도록 함
        }

        // 점수찍기
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(MARGIN, p_game->ROW * BLOCK_SIZE + 2 * MARGIN, p_game->COL * BLOCK_SIZE, BLOCK_SIZE), Qt::AlignCenter, "chain score: " + QString::number(p_game->chain_score));
    }
    else if (GAMENAME == "PuyopuyoTetris") {
        // 뿌요뿌요 테트리스 그리기
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(p_game->COL * BLOCK_SIZE + 2 * MARGIN, MARGIN, BLOCK_SIZE * 5, BLOCK_SIZE), Qt::AlignCenter, "NEXT");

        std::queue<int> nextPieces = static_cast<PuyopuyoTetrisGame*>(p_game)->nextPieces;
        for (size_t k = 0; k < 5; ++k) {
            if (nextPieces.empty())
                break;

            int piece = nextPieces.front();
            nextPieces.pop();
            if (piece < 25) {
                // 뿌요 그리기
                int color1 = (piece % 5) + 1;
                int color2 = ((piece / 5) % 5) + 1;
                QColor color = getColorFromCode(color1);
                painter.setBrush(color);
                painter.drawEllipse(p_game->COL * BLOCK_SIZE + 2 * MARGIN, MARGIN + (k * 3 * BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE);
                color = getColorFromCode(color2);
                painter.setBrush(color);
                painter.drawEllipse(p_game->COL * BLOCK_SIZE + 2 * MARGIN, MARGIN + (k * 3 * BLOCK_SIZE) + BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            } else {
                // 테트리스 조각 그리기
                int tetrisPiece = piece - 25;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        if (TetrisGame::SHAPES[tetrisPiece][i][j]) {
                            QRect rect(p_game->COL * BLOCK_SIZE + 2 * MARGIN + j * (BLOCK_SIZE / 2), MARGIN + (k * 3 * BLOCK_SIZE) + i * (BLOCK_SIZE / 2), BLOCK_SIZE / 2, BLOCK_SIZE / 2);
                            painter.setBrush(Qt::gray);
                            painter.drawRect(rect);
                        }
                    }
                }
            }
        }

        // 게임 보드 그리기
        for (int i = 0; i < p_game->ROW; i++) {
            for (int j = 0; j < p_game->COL; j++) {
                painter.setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));
                painter.drawRect(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);

                if (p_game->board[i][j] != 0) {
                    QColor color = getColorFromCode(p_game->board[i][j]);
                    if (p_game->board[i][j] == -6) {
                        color = Qt::gray;  // 테트리스 블록을 회색으로
                    }
                    painter.setBrush(color);
                    if (p_game->board[i][j] > 0 && p_game->board[i][j] < 6) {
                        painter.drawEllipse(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);  // 뿌요는 원형으로
                    } else {
                        painter.drawRect(j * BLOCK_SIZE + MARGIN, i * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
                    }
                }
            }
        }

        // 현재 조각 그리기
        std::vector<std::pair<int, int>> coordinates = static_cast<PuyopuyoTetrisGame*>(p_game)->getPieceCoordinates();
        for (auto coord : coordinates) {
            QColor color = getColorFromCode(static_cast<PuyopuyoTetrisGame*>(p_game)->currentShape[coord.first - static_cast<PuyopuyoTetrisGame*>(p_game)->currentRow][coord.second - static_cast<PuyopuyoTetrisGame*>(p_game)->currentCol]);
            if (color == QColor(0, 206, 209)) {  // 청록색을 회색으로
                color = Qt::gray;
            }
            painter.setBrush(color);
            if (static_cast<PuyopuyoTetrisGame*>(p_game)->isPuyo) {
                painter.drawEllipse(coord.second * BLOCK_SIZE + MARGIN, coord.first * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);  // 뿌요는 원형으로
            } else {
                painter.drawRect(coord.second * BLOCK_SIZE + MARGIN, coord.first * BLOCK_SIZE + MARGIN, BLOCK_SIZE, BLOCK_SIZE);
            }
        }

        // 점수찍기
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(MARGIN, p_game->ROW * BLOCK_SIZE + 2 * MARGIN, p_game->COL * BLOCK_SIZE, BLOCK_SIZE), Qt::AlignCenter, "line score: " + QString::number(p_game->line_score));

        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(QRect(MARGIN, p_game->ROW * BLOCK_SIZE + 2 * MARGIN, p_game->COL * 2.5 * BLOCK_SIZE, BLOCK_SIZE), Qt::AlignCenter, "chain score: " + QString::number(p_game->chain_score));
    }
}

// 키보드 입력 이벤트 처리
void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (p_game->gameover)
        return;
    // 입력한 키 분기처리
    switch (event->key())
    {
    case Qt::Key_Down:
        p_game->moveDown();
        break;
    case Qt::Key_Left:
        p_game->moveLeft();
        break;
    case Qt::Key_Right:
        p_game->moveRight();
        break;
    case Qt::Key_Space:
        p_game->hardDrop();
        break;
    case Qt::Key_Z:
        p_game->rotateCounterClockwise();
        update();
        break;
    case Qt::Key_X:
        p_game->rotateClockwise();
        update();
        break;
    default:
        break;
    }
    update();
}

// 타이머 이벤트 처리
void GameWindow::timerEvent(QTimerEvent *event)
{
    if (p_game->gameover)
    {
        killTimer(timer);  // 타이머 종료
        gameoverwindow = new GameoverWindow;
        gameoverwindow->show();
        return;
    }

    if (event->timerId() == timer)
    {
        p_game->moveDown();
        if (p_game->checkGameOver())
        {
            p_game->gameover = true;
            killTimer(timer);
            gameoverwindow = new GameoverWindow;
            gameoverwindow->show();
        }
        update();
    }
}

// 다음 조각 그리기
void GameWindow::drawNext()
{
    // 게임오버
    if (p_game->gameover)
    {
        killTimer(timer);
        gameoverwindow = new GameoverWindow;
        gameoverwindow->show();
        return;
    }

    // 게임오버 ㅇ 아니면
    p_game->applyGravity();
    p_game->removePieces();
    p_game->generateNewPiece();
    if (p_game->checkGameOver())
    {
        p_game->gameover = true;
        killTimer(timer);
        gameoverwindow = new GameoverWindow;
        gameoverwindow->show();
    }
    update();
}
