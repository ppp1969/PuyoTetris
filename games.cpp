#include "games.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <stack>
using namespace std;

// 테트리스 블록 모양 초기화 -> 초기값으로 지정하고 불러서 사용
const char TetrisGame::SHAPES[7][4][4] = {
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

// 테트리스 블록 색상 초기화 -> int로 유지
const int TetrisGame::COLORS[7] = {
    1,  // I - 보라색
    2,  // J - 주황색
    3,  // L - 파란색
    4,  // O - 초록색
    5,  // S - 빨간색
    6,  // Z - 하늘색
    7   // T - 노란색
};

// Game 클래스 생성자
Game::Game(int row, int col, int length)
{
    ROW = row;
    COL = col;
    LENGTH = length;

    board = new int*[ROW];
    for (int i = 0; i < ROW; i++)
        board[i] = new int[COL];

    next = new int[length];
    gameover = false;

    std::srand(std::time(nullptr));  // 난수 시드 초기화
}

// Game 클래스 소멸자
Game::~Game()
{
    delete[] next;

    for (int i = 0; i < ROW; i++)
        delete[] board[i];
    delete[] board;
}

// TetrisGame 클래스 생성자
TetrisGame::TetrisGame() : Game(20, 10, 5)
{
    for (int i = 0; i < 5; ++i)
        nextPieces.push(std::rand() % 7);

    generateNewPiece();

    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            board[i][j] = 0;

    line_score = 0;
    gameover = false;
}

// TetrisGame 클래스 소멸자
TetrisGame::~TetrisGame() {}

// 왼쪽으로 이동
void TetrisGame::moveLeft() {
    if (isValidMove(currentRow, currentCol - 1)) {
        currentCol--;
    }
}

// 오른쪽으로 이동
void TetrisGame::moveRight() {
    if (isValidMove(currentRow, currentCol + 1)) {
        currentCol++;
    }
}

// 아래로 이동
void TetrisGame::moveDown() {
    if (isValidMove(currentRow + 1, currentCol)) {
        currentRow++;
    } else {
        lockPiece();
        clearLines();
        generateNewPiece();
    }
}

// 하드 드롭 -> 반복하면서 내리기
void TetrisGame::hardDrop() {
    while (isValidMove(currentRow + 1, currentCol)) {
        currentRow++;
    }
    lockPiece();
    clearLines();
    generateNewPiece();
}

// 시계 방향 회전
void TetrisGame::rotateClockwise() {
    rotatePiece(true);
}

// 반시계 방향 회전
void TetrisGame::rotateCounterClockwise() {
    rotatePiece(false);
}

// 중력 적용
void TetrisGame::applyGravity() {
    moveDown();
}

// 새로운 조각 생성
void TetrisGame::generateNewPiece() {
    // 큐에서 빼서 사용
    currentPiece = nextPieces.front();
    nextPieces.pop();
    // 큐에 랜덤생성한 int값 넣기
    nextPieces.push(std::rand() % 7);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            currentShape[i][j] = SHAPES[currentPiece][i][j];
    currentRow = 0;
    currentCol = COL / 2 - 2;
    if (!isValidMove(currentRow, currentCol)) {
        gameover = true;
    }
}

// 조각 제거
void TetrisGame::removePieces() {
    clearLines();
}

// 게임 오버 확인
bool TetrisGame::checkGameOver() {
    for (int col = 0; col < COL; ++col) {
        if (board[0][col] != 0) {
            return true;
        }
    }
    return gameover;
}

// 유효한 이동인지 확인
bool TetrisGame::isValidMove(int row, int col) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                int newRow = row + i;
                int newCol = col + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 유효한 회전인지 확인
bool TetrisGame::isValidRotation(int newShape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (newShape[i][j]) {
                int newRow = currentRow + i;
                int newCol = currentCol + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 조각을 보드에 고정 -> 내려가지 못하도록
void TetrisGame::lockPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                board[currentRow + i][currentCol + j] = COLORS[currentPiece];
            }
        }
    }
}

// 라인 제거
void TetrisGame::clearLines() {
    for (int i = ROW - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < COL; j++) {
            if (board[i][j] == 0) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) { // 해당 행-라인이 꽉 찼을때
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COL; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            for (int j = 0; j < COL; j++) {
                board[0][j] = 0;
            }
            line_score++;
            i++;
        }
    }
}

// 현재 조각의 좌표 반환
vector<pair<int, int>> TetrisGame::getPieceCoordinates() {
    vector<pair<int, int>> coordinates;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                coordinates.push_back({currentRow + i, currentCol + j});
            }
        }
    }
    return coordinates;
}

// 조각 회전
void TetrisGame::rotatePiece(bool clockwise) {
    int newShape[4][4] = {0};

    // 네모 블록(O-블록)은 회전하지 않도록 설정
    if (currentPiece == 3) {
        return;
    }

    // 중심 좌표 설정
    float centerRow, centerCol;

    if (currentPiece == 0) { // 일자 모양 블록
        centerRow = 1.5;
        centerCol = 1.5;  // 일자 모양의 중심을 (1.5, 1.5)로 설정
    } else {
        centerRow = 1;
        centerCol = 1;  // 다른 블록들은 중심을 (1, 1)로 설정
    }

    if (clockwise) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (currentShape[i][j]) {
                    // 형변환과 좌표계산을 통한 일반화
                    int newRow = static_cast<int>(centerRow + (j - centerCol));
                    int newCol = static_cast<int>(centerCol - (i - centerRow));
                    newShape[newRow][newCol] = currentShape[i][j];
                }
            }
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (currentShape[i][j]) {
                    int newRow = static_cast<int>(centerRow - (j - centerCol));
                    int newCol = static_cast<int>(centerCol + (i - centerRow));
                    newShape[newRow][newCol] = currentShape[i][j];
                }
            }
        }
    }

    if (isValidRotation(newShape)) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                currentShape[i][j] = newShape[i][j];
            }
        }
    }
}

// PuyopuyoGame 클래스 생성자
PuyopuyoGame::PuyopuyoGame() : Game(12, 6, 5)
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            currentShape[i][j] = 0;
        }
    }

    generateNewPiece();

    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            board[i][j] = 0;

    chain_score = 0;
    gameover = false;

    nextPieces.push({std::rand() % 5 + 1, std::rand() % 5 + 1});
}

// PuyopuyoGame 클래스 소멸자
PuyopuyoGame::~PuyopuyoGame() {}

// 왼쪽으로 이동
void PuyopuyoGame::moveLeft() {
    if (isValidMove(currentRow, currentCol - 1)) {
        currentCol--;
    }
}

// 오른쪽으로 이동
void PuyopuyoGame::moveRight() {
    if (isValidMove(currentRow, currentCol + 1)) {
        currentCol++;
    }
}

// 아래로 이동
void PuyopuyoGame::moveDown() {
    if (isValidMove(currentRow + 1, currentCol)) {
        currentRow++;
    } else {
        lockPiece();
        clearGroups();
        applyGravity();
        generateNewPiece();
    }
}

// 하드 드롭
void PuyopuyoGame::hardDrop() {
    while (isValidMove(currentRow + 1, currentCol)) {
        currentRow++;
    }
    lockPiece();
    clearGroups();
    applyGravity();
    generateNewPiece();
}

// 시계 방향 회전
void PuyopuyoGame::rotateClockwise() {
    rotatePiece(true);
}

// 반시계 방향 회전
void PuyopuyoGame::rotateCounterClockwise() {
    rotatePiece(false);
}

// 중력 적용
void PuyopuyoGame::applyGravity() {
    bool moved;
    do {
        moved = false;
        for (int col = 0; col < COL; ++col) {
            for (int row = ROW - 1; row >= 0; --row) {
                if (board[row][col] != 0) {
                    int newRow = row;
                    while (newRow + 1 < ROW && board[newRow + 1][col] == 0) {
                        newRow++;
                    }
                    if (newRow != row) {
                        board[newRow][col] = board[row][col];
                        board[row][col] = 0;
                        moved = true;
                    }
                }
            }
        }
    } while (moved);
    clearGroups();
}

// 새로운 뿌요 생성
void PuyopuyoGame::generateNewPiece() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            currentShape[i][j] = 0;
        }
    }

    if (nextPieces.empty()) {
        nextPieces.push({std::rand() % 5 + 1, std::rand() % 5 + 1});
    }
    // 큐에서 뽑아서 사용
    auto nextPiece = nextPieces.front();
    nextPieces.pop();
    // 값 뽑아오기
    currentShape[0][1] = nextPiece.first;  // 가운데 조각
    currentShape[1][1] = nextPiece.second;  // 가운데 아래 조각

    currentRow = 0;
    currentCol = 1;
    if (!isValidMove(currentRow, currentCol)) {
        gameover = true;
    }

    // 큐에 새로 넣기
    if (nextPieces.size() < 3) {
        nextPieces.push({std::rand() % 5 + 1, std::rand() % 5 + 1});
    }
}

// 뿌요 제거
void PuyopuyoGame::removePieces() {
    clearGroups();
}

// 게임 오버 확인
bool PuyopuyoGame::checkGameOver() {
    if (board[0][2] != 0) {
        return true;
    }

    return gameover;
}

// 유효한 이동인지 확인
bool PuyopuyoGame::isValidMove(int row, int col) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (currentShape[i][j]) {
                int newRow = row + i;
                int newCol = col + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 유효한 회전인지 확인
bool PuyopuyoGame::isValidRotation(int newShape[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (newShape[i][j]) {
                int newRow = currentRow + i;
                int newCol = currentCol + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 뿌요를 보드에 고정
void PuyopuyoGame::lockPiece() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (currentShape[i][j]) {
                board[currentRow + i][currentCol + j] = currentShape[i][j];
            }
        }
    }
}

// 그룹 제거
void PuyopuyoGame::clearGroups() {
    bool groupCleared;
    int chainCount = 0;
    do {
        groupCleared = false;
        vector<vector<bool>> visited(ROW, vector<bool>(COL, false));
        vector<pair<int, int>> toClear;
        vector<vector<pair<int, int>>> allGroups;

        for (int i = 0; i < ROW; ++i) {
            for (int j = 0; j < COL; ++j) {
                if (board[i][j] != 0 && !visited[i][j]) {
                    vector<pair<int, int>> group;
                    if (dfsClear(i, j, board[i][j], visited, group)) { // 지워야하는 경우라면

                        allGroups.push_back(group);
                        toClear.insert(toClear.end(), group.begin(), group.end());

                    }
                }
            }
        }

        if (!allGroups.empty()) {
            for (auto cell : toClear) { // 보드에서 지우기
                board[cell.first][cell.second] = 0;
            }
            // 지웠으면, score 추가
            groupCleared = true;
            chainCount++;
            chain_score += 1;
            applyGravity();
        }
    } while (groupCleared);
}

// 깊이 우선 탐색을 통해 그룹 제거 - stack사용
bool PuyopuyoGame::dfsClear(int row, int col, int color, vector<vector<bool>> &visited, vector<pair<int, int>> &group) {
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    stack<pair<int, int>> s;

    // 스택에 초기값 넣기
    s.push({row, col});
    // 방문 추적 초기화
    visited[row][col] = true;

    while (!s.empty()) {
        // 스택에서 top에서 뽑아와서
        auto [r, c] = s.top(); s.pop();
        // 그룹에 좌표 넣기
        group.push_back({r, c});
        // 상하좌우 방향으로 체크
        for (auto [dr, dc] : directions) {
            int nr = r + dr;
            int nc = c + dc;
            // 방향에 같은 색의 값이고, 방문안한 노드면 스택에 넣기. 결과적으로 그룹에 들어감.
            if (nr >= 0 && nr < ROW && nc >= 0 && nc < COL && !visited[nr][nc] && board[nr][nc] == color) {
                s.push({nr, nc});
                visited[nr][nc] = true;
            }
        }
    }
    // 그룹 사이즈가 4이상이면 true리턴 -> 지워야하는 group임을 알려줌
    return group.size() >= 4;
}

// 현재 뿌요의 좌표 반환
vector<pair<int, int>> PuyopuyoGame::getPieceCoordinates() {
    vector<pair<int, int>> coordinates;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (currentShape[i][j]) {
                coordinates.push_back({currentRow + i, currentCol + j});
            }
        }
    }
    return coordinates;
}

// 뿌요 회전
void PuyopuyoGame::rotatePiece(bool clockwise) {
    int newShape[3][3] = {0};

    int centerRow = 1;
    int centerCol = 1;

    if (clockwise) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (currentShape[i][j]) {
                    int newRow = centerRow + (j - centerCol);
                    int newCol = centerCol - (i - centerRow);
                    newShape[newRow][newCol] = currentShape[i][j];
                }
            }
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (currentShape[i][j]) {
                    int newRow = centerRow - (j - centerCol);
                    int newCol = centerCol + (i - centerRow);
                    newShape[newRow][newCol] = currentShape[i][j];
                }
            }
        }
    }

    if (isValidRotation(newShape)) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; j++) {
                currentShape[i][j] = newShape[i][j];
            }
        }
    }
}

// PuyopuyoTetrisGame 클래스 생성자
PuyopuyoTetrisGame::PuyopuyoTetrisGame() : Game(16, 8, 5)
{
    for (int i = 0; i < 5; ++i)
        nextPieces.push(rand() % 32);

    generateNewPiece();

    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            board[i][j] = 0;

    chain_score = 0;
    line_score = 0;
    gameover = false;
}

// PuyopuyoTetrisGame 클래스 소멸자
PuyopuyoTetrisGame::~PuyopuyoTetrisGame() {}

// 왼쪽으로 이동
void PuyopuyoTetrisGame::moveLeft() {
    if (isValidMove(currentRow, currentCol - 1)) {
        currentCol--;
    }
}

// 오른쪽으로 이동
void PuyopuyoTetrisGame::moveRight() {
    if (isValidMove(currentRow, currentCol + 1)) {
        currentCol++;
    }
}

// 아래로 이동
void PuyopuyoTetrisGame::moveDown() {
    if (isPuyo) {
        if (isValidMove(currentRow + 1, currentCol)) {
            currentRow++;
        } else {
            lockPiece();
            clearGroups();
            applyGravity();
            generateNewPiece();
        }
    } else {
        if (isValidMove(currentRow + 1, currentCol)) {
            currentRow++;
        } else {
            lockPiece();
            clearLines();
            generateNewPiece();
        }
    }
}

// 하드 드롭
void PuyopuyoTetrisGame::hardDrop() {
    if (!isPuyo) { // 뿌요가 아닌 경우!
        while (isValidMove(currentRow + 1, currentCol)) {
            currentRow++;
        }
        swapPuyosWithTetromino();  // 뿌요 <-> 테트리스 교환 함수 호출
        clearLines();
        generateNewPiece();
    } else {
        while (isValidMove(currentRow + 1, currentCol)) {
            currentRow++;
        }
        lockPiece();
        clearGroups();
        applyGravity();
        generateNewPiece();
    }
}

// 시계 방향 회전
void PuyopuyoTetrisGame::rotateClockwise() {
    rotatePiece(true);
}

// 반시계 방향 회전
void PuyopuyoTetrisGame::rotateCounterClockwise() {
    rotatePiece(false);
}

// 중력 적용
void PuyopuyoTetrisGame::applyGravity() {
    if (isPuyo) {  // 뿌요에 중력 적용
        bool moved;
        do {
            moved = false;
            for (int col = 0; col < COL; ++col) {
                for (int row = ROW - 1; row >= 0; --row) {
                    if (board[row][col] > 0 && board[row][col] < 6) {  // 뿌요 블록인 경우
                        int newRow = row;
                        while (newRow + 1 < ROW && board[newRow + 1][col] == 0) {
                            newRow++;
                        }
                        if (newRow != row) {
                            board[newRow][col] = board[row][col];
                            board[row][col] = 0;
                            moved = true;
                        }
                    }
                }
            }
        } while (moved);
        clearGroups();
    } else {
        moveDown();
    }
}

// 새로운 조각 생성
void PuyopuyoTetrisGame::generateNewPiece() {
    int piece = nextPieces.front();
    nextPieces.pop();
    nextPieces.push(rand() % 32);

    isPuyo = piece < 25;  // 뿌요인지 확인
    if (isPuyo) {
        // 뿌요 생성
        int color1 = (piece % 5) + 1;
        int color2 = ((piece / 5) % 5) + 1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                currentShape[i][j] = 0;
            }
        }
        currentShape[0][1] = color1;
        currentShape[1][1] = color2;
        currentRow = 0;
        currentCol = 3;
    } else {
        // 테트리스 조각 생성
        currentPiece = piece - 25;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                currentShape[i][j] = TetrisGame::SHAPES[currentPiece][i][j] ? 6 : 0;
            }
        }
        currentRow = 0;
        currentCol = COL / 2 - 2;
    }

    if (!isValidMove(currentRow, currentCol)) {
        gameover = true;
    }
}

// 조각 제거
void PuyopuyoTetrisGame::removePieces() {
    if (isPuyo) {
        clearGroups();
    } else {
        clearLines();
    }
}

// 게임 오버 확인
bool PuyopuyoTetrisGame::checkGameOver() {
    for (int col = 2; col <= 5; ++col) {
        if (board[0][col] != 0) {
            return true;
        }
    }
    return gameover;
}

// 유효한 이동인지 확인
bool PuyopuyoTetrisGame::isValidMove(int row, int col) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                int newRow = row + i;
                int newCol = col + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 유효한 회전인지 확인 -> 3x3 = 뿌요
bool PuyopuyoTetrisGame::isValidRotation(int newShape[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (newShape[i][j]) {
                int newRow = currentRow + i;
                int newCol = currentCol + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 유효한 회전인지 확인 -> 4x4 = 테트리스
bool PuyopuyoTetrisGame::isValidRotation(int newShape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (newShape[i][j]) {
                int newRow = currentRow + i;
                int newCol = currentCol + j;
                if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL || board[newRow][newCol] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 조각을 보드에 고정
void PuyopuyoTetrisGame::lockPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                if (isPuyo) {
                    board[currentRow + i][currentCol + j] = currentShape[i][j];
                } else {
                    board[currentRow + i][currentCol + j] = -6;  // 테트리스 블록을 회색으로 표시
                }
            }
        }
    }
}

// 라인 제거
void PuyopuyoTetrisGame::clearLines() {
    for (int i = ROW - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < COL; j++) {
            if (board[i][j] != -6) {  // 테트리스 블록은 6으로 표시
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < COL; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            for (int j = 0; j < COL; j++) {
                board[0][j] = 0;
            }
            line_score++;
            i++;
        }
    }
}

// 그룹 제거
void PuyopuyoTetrisGame::clearGroups() {
    bool groupCleared;
    int chainCount = 0;
    do {
        groupCleared = false;
        std::vector<std::vector<bool>> visited(ROW, std::vector<bool>(COL, false));
        std::vector<std::pair<int, int>> toClear;
        std::vector<std::vector<std::pair<int, int>>> allGroups;

        for (int i = 0; i < ROW; ++i) {
            for (int j = 0; j < COL; ++j) {
                if (board[i][j] > 0 && !visited[i][j] && board[i][j] < 6) {  // 뿌요
                    std::vector<std::pair<int, int>> group;
                    if (dfsClear(i, j, board[i][j], visited, group)) {
                        allGroups.push_back(group);
                        toClear.insert(toClear.end(), group.begin(), group.end());
                    }
                }
            }
        }

        if (!allGroups.empty()) {
            for (auto cell : toClear) {
                board[cell.first][cell.second] = 0;
            }
            groupCleared = true;  // 그룹 제거
            chainCount++;
            chain_score++;
            applyGravity();  // 중력 적용
        }
    } while (groupCleared);
}

// 깊이 우선 탐색을 통해 그룹 제거
bool PuyopuyoTetrisGame::dfsClear(int row, int col, int color, std::vector<std::vector<bool>> &visited, std::vector<std::pair<int, int>> &group) {
    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    std::stack<std::pair<int, int>> s;

    s.push({row, col});
    visited[row][col] = true;

    while (!s.empty()) {
        auto [r, c] = s.top(); s.pop();
        group.push_back({r, c});

        for (auto [dr, dc] : directions) {
            int nr = r + dr;
            int nc = c + dc;

            if (nr >= 0 && nr < ROW && nc >= 0 && nc < COL && !visited[nr][nc] && board[nr][nc] == color) {
                s.push({nr, nc});
                visited[nr][nc] = true;
            }
        }
    }

    return group.size() >= 4;
}

// 현재 조각의 좌표 반환
std::vector<std::pair<int, int>> PuyopuyoTetrisGame::getPieceCoordinates() {
    std::vector<std::pair<int, int>> coordinates;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentShape[i][j]) {
                coordinates.push_back({currentRow + i, currentCol + j});
            }
        }
    }
    return coordinates;
}

// 조각 회전
void PuyopuyoTetrisGame::rotatePiece(bool clockwise) {
    if (isPuyo) { // 뿌요일때 회전, 아래 조각이 중심이 된다.
        int newShape[3][3] = {0};

        int centerRow = 1;
        int centerCol = 1;

        if (clockwise) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (currentShape[i][j]) {
                        int newRow = centerRow + (j - centerCol);
                        int newCol = centerCol - (i - centerRow);
                        newShape[newRow][newCol] = currentShape[i][j];
                    }
                }
            }
        } else {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (currentShape[i][j]) {
                        int newRow = centerRow - (j - centerCol);
                        int newCol = centerCol + (i - centerRow);
                        newShape[newRow][newCol] = currentShape[i][j];
                    }
                }
            }
        }

        if (isValidRotation(newShape)) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; j++) {
                    currentShape[i][j] = newShape[i][j];
                }
            }
        }
    } else {
        int newShape[4][4] = {0};

        // 네모 블록(O-블록)은 회전하지 않도록 설정
        if (currentPiece == 3) {
            return;
        }

        // 중심 좌표 설정
        float centerRow, centerCol;

        if (currentPiece == 0) {  // 일자 모양 블록
            centerRow = 1.5;
            centerCol = 1.5;  // 일자 모양의 중심을 (1.5, 1.5)로 설정
        } else {
            centerRow = 1;
            centerCol = 1;  // 다른 블록들은 중심을 (1, 1)로 설정
        }

        if (clockwise) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (currentShape[i][j]) {
                        int newRow = static_cast<int>(centerRow + (j - centerCol));
                        int newCol = static_cast<int>(centerCol - (i - centerRow));
                        newShape[newRow][newCol] = currentShape[i][j];
                    }
                }
            }
        } else {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (currentShape[i][j]) {
                        int newRow = static_cast<int>(centerRow - (j - centerCol));
                        int newCol = static_cast<int>(centerCol + (i - centerRow));
                        newShape[newRow][newCol] = currentShape[i][j];
                    }
                }
            }
        }

        if (isValidRotation(newShape)) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    currentShape[i][j] = newShape[i][j];
                }
            }
        }
    }
}

// 뿌요와 테트로미노 교환
void PuyopuyoTetrisGame::swapPuyosWithTetromino() {
    bool movedDown = true;

    while (movedDown) {
        movedDown = false;

        // 테트로미노 아래로 이동
        if (isValidMove(currentRow + 1, currentCol)) {
            currentRow++;
            movedDown = true;
        }

        for (int i = 3; i >= 0; --i) {  // 위에서 아래로 검사
            for (int j = 0; j < 4; ++j) {
                if (currentShape[i][j] == 6) {  // 테트로미노 블록인 경우
                    int newRow = currentRow + i;
                    int newCol = currentCol + j;

                    // 보드 경계 확인 및 유효한 위치 확인
                    if (newRow + 1 < ROW) {
                        int belowRow = newRow + 1;
                        if (board[belowRow][newCol] > 0 && board[belowRow][newCol] < 6) {  // 아래에 뿌요가 있는 경우
                            // 뿌요와 테트리스 블록의 위치를 교환
                            std::swap(board[newRow][newCol], board[belowRow][newCol]);
                            movedDown = true;
                        }
                    }
                }
            }
        }
    }

    // 최종 위치에서 테트리스 블록을 보드에 고정
    lockPiece();

    // 뿌요 블록에 중력 적용
    applyGravityToPuyos();
}

// 뿌요 블록에 중력 적용
void PuyopuyoTetrisGame::applyGravityToPuyos() {
    bool moved;
    do {
        moved = false;
        for (int col = 0; col < COL; ++col) {
            for (int row = ROW - 2; row >= 0; --row) {  // 아래에서 위로 검사
                if (board[row][col] > 0 && board[row][col] < 6) {  // 뿌요 블록인 경우
                    int newRow = row;
                    while (newRow + 1 < ROW && board[newRow + 1][col] == 0) {
                        std::swap(board[newRow][col], board[newRow + 1][col]);
                        newRow++;
                        moved = true;
                    }
                }
            }
        }
    } while (moved);
}
