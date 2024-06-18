#ifndef GAMES_H
#define GAMES_H

#include <string>
#include <vector>
#include <queue>

// Game 클래스: 게임의 기본 인터페이스를 제공하는 추상 클래스
class Game
{
public:
    Game(int row, int col, int length);
    virtual ~Game();

    // 가상 함수들: 파생 클래스에서 구현해야 함
    virtual void moveLeft() = 0;  // 왼쪽으로 이동
    virtual void moveRight() = 0;  // 오른쪽으로 이동
    virtual void moveDown() = 0;  // 아래로 이동
    virtual void hardDrop() = 0;  // 하드 드롭
    virtual void rotateClockwise() = 0;  // 시계 방향 회전
    virtual void rotateCounterClockwise() = 0;  // 반시계 방향 회전
    virtual void applyGravity() = 0;  // 중력 적용
    virtual void generateNewPiece() = 0;  // 새로운 조각 생성
    virtual void removePieces() = 0;  // 조각 제거
    virtual bool checkGameOver() = 0;  // 게임 오버 확인

    int ROW;  // 게임 보드의 행 수
    int COL;  // 게임 보드의 열 수
    int LENGTH;  // 길이

    double axis_row;  // 축의 행 좌표
    double axis_col;  // 축의 열 좌표
    // 기존의 score없애고, score2개로 유지
    int chain_score;  // 체인 점수
    int line_score;  // 라인 점수
    bool gameover;  // 게임 오버 상태

    int** board;  // 게임 보드
    int* next;  // 다음 조각
};

// TetrisGame 클래스: 테트리스 게임을 구현하는 클래스
class TetrisGame : public Game
{
public:
    TetrisGame();
    ~TetrisGame();

    void moveLeft();
    void moveRight();
    void moveDown();
    void hardDrop();
    void rotateClockwise();
    void rotateCounterClockwise();
    void applyGravity();
    void generateNewPiece();
    void removePieces();
    bool checkGameOver();
    std::vector<std::pair<int, int>> getPieceCoordinates();

    static const char SHAPES[7][4][4];  // 테트리스 블록 모양
    static const int COLORS[7];  // 테트리스 블록 색상
    std::queue<int> nextPieces;  // 다음 나올 조각의 인덱스를 저장하는 큐
    int currentPiece;  // 현재 조각
    void rotatePiece(bool clockwise);
private:
    bool isValidMove(int row, int col);  // 유효한 이동인지 확인
    bool isValidRotation(int newShape[4][4]);  // 유효한 회전인지 확인
    void lockPiece();  // 조각을 고정
    void clearLines();  // 라인을 제거

    int currentShape[4][4];  // 현재 조각 모양
    int currentRow, currentCol;  // 현재 조각의 위치

    friend class PuyopuyoTetrisGame;
};

// PuyopuyoGame 클래스: 뿌요뿌요 게임을 구현하는 클래스
class PuyopuyoGame : public Game
{
public:
    PuyopuyoGame();
    ~PuyopuyoGame();

    void moveLeft();
    void moveRight();
    void moveDown();
    void hardDrop();
    void rotateClockwise();
    void rotateCounterClockwise();
    void applyGravity();
    void generateNewPiece();
    void removePieces();
    bool checkGameOver();
    std::vector<std::pair<int, int>> getPieceCoordinates();

    int currentShape[3][3];  // 현재 뿌요 모양
    int currentRow, currentCol;  // 현재 뿌요의 위치
    std::queue<std::pair<int, int>> nextPieces;  // 다음 뿌요를 저장할 큐

private:
    bool isValidMove(int row, int col);  // 유효한 이동인지 확인
    bool isValidRotation(int newShape[3][3]);  // 유효한 회전인지 확인
    void lockPiece();  // 뿌요를 고정
    void clearGroups();  // 그룹을 제거
    bool dfsClear(int row, int col, int color, std::vector<std::vector<bool>> &visited, std::vector<std::pair<int, int>> &group);  // 깊이 우선 탐색을 통해 그룹 제거
    void rotatePiece(bool clockwise);

    friend class PuyopuyoTetrisGame;
};

// PuyopuyoTetrisGame 클래스: 뿌요뿌요 테트리스 게임을 구현하는 클래스
class PuyopuyoTetrisGame : public Game
{
public:
    PuyopuyoTetrisGame();
    ~PuyopuyoTetrisGame();

    void moveLeft();
    void moveRight();
    void moveDown();
    void hardDrop();
    void rotateClockwise();
    void rotateCounterClockwise();
    void applyGravity();
    void applyGravityToPuyos();
    void generateNewPiece();
    void removePieces();
    bool checkGameOver();
    std::vector<std::pair<int, int>> getPieceCoordinates();

    int currentShape[4][4];  // 현재 조각 모양
    int currentRow, currentCol;  // 현재 조각의 위치
    int currentPiece;  // 현재 조각 인덱스
    std::queue<int> nextPieces;  // 다음 나올 조각의 인덱스를 저장하는 큐
    bool isPuyo;  // 현재 조각이 뿌요인지 여부

    PuyopuyoGame puyoGame;
    TetrisGame tetrisGame;

private:
    bool isValidMove(int row, int col);  // 유효한 이동인지 확인
    bool isValidMoveForSwap(int row, int col);  // 교환을 위한 유효한 이동인지 확인
    bool isValidRotation(int newShape[4][4]);  // 유효한 회전인지 확인
    bool isValidRotation(int newShape[3][3]);  // 유효한 회전인지 확인
    void lockPiece();  // 조각을 고정
    void clearLines();  // 라인을 제거
    void clearGroups();  // 그룹을 제거
    bool dfsClear(int row, int col, int color, std::vector<std::vector<bool>> &visited, std::vector<std::pair<int, int>> &group);  // 깊이 우선 탐색을 통해 그룹 제거
    void rotatePiece(bool clockwise);  // 조각을 회전
    void swapPuyosWithTetromino();  // 뿌요와 테트로미노를 교환 -> 테트리스 hard drop시 사용
    void lockPuyoPiece(int row, int col);
};

#endif // GAMES_H
