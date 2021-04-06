#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
using namespace std;

typedef vector<vector<char> > Board;

const int N = 9;

class SudokuPlayer
{
private:
    int rowUsed[N];
    int columnUsed[N];
    int blockUsed[N];

public:
    vector<Board> result;
    vector<pair<int, int> > spaces;

public:
    SudokuPlayer()
    {
        initState();
    }

    void initState()
    {
        memset(rowUsed, 0, sizeof(rowUsed));
        memset(columnUsed, 0, sizeof(columnUsed));
        memset(blockUsed, 0, sizeof(blockUsed));
        spaces.clear();
        result.clear();
    }

    void addResult(Board &board)
    {
        vector<vector<char> > obj(board);
        result.push_back(obj);
    }

    void printBoard(Board &board)
    {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                cout << board[i][j] << " ";
            }
            cout << "\n";
        }
    }

    void getResult()
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            Board board = result[i];
            printBoard(board);
        }
    }

    void flip(int i, int j, int digit)
    {
        rowUsed[i] ^= (1 << digit);
        columnUsed[j] ^= (1 << digit);
        blockUsed[(i / 3) * 3 + j / 3] ^= (1 << digit);
    }

    void solveSudoku(Board board)
    {
        initState();
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (board[i][j] == '.')
                {
                    spaces.push_back(pair<int, int>(i, j));
                }
                else
                {
                    int digit = board[i][j] - '1';
                    flip(i, j, digit);
                }
            }
        }
        DFS(board, 0);
    }

    void DFS(Board &board, int pos)
    {
        if (pos == spaces.size())
        {
            addResult(board);
            return;
        }
        int i = spaces[pos].first;
        int j = spaces[pos].second;
        int mask = ~(rowUsed[i] | columnUsed[j] | blockUsed[(i / 3) * 3 + j / 3]) & 0x1ff;
        int digit = 0;
        while (mask)
        {
            if (mask & 1)
            {
                flip(i, j, digit);
                board[i][j] = '1' + digit;
                DFS(board, pos + 1);
                flip(i, j, digit);
            }
            mask = mask >> 1;
            digit++;
        }
    }

    bool checkBoard(Board &board)
    {
        initState();
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] != '.')
                {
                    int digit = board[i][j] - '1';
                    if ((rowUsed[i] | columnUsed[j] | blockUsed[(i / 3) * 3 + j / 3]) & (1 << digit))
                    {
                        return false;
                    }
                    flip(i, j, digit);
                }
            }
        }
        return true;
    }
};

char data[9][9] = {
    {'5', '3', '.', '.', '7', '.', '.', '.', '.'},
    {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
    {'.', '9', '8', '.', '.', '.', '.', '6', '.'},
    {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
    {'4', '.', '.', '8', '.', '3', '.', '.', '1'},
    {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
    {'.', '6', '.', '.', '.', '.', '2', '8', '.'},
    {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
    {'.', '.', '.', '.', '8', '.', '.', '7', '9'}};

void test()
{
    SudokuPlayer player;
    vector<vector<char> > board(N, vector<char>(N, '.'));

    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            board[i][j] = data[i][j];
        }
    }
    bool check = player.checkBoard(board);
    if (check)
        cout << "checked" << endl;

    player.solveSudoku(board);
    player.getResult();

    cout << endl;
}

int getRand()
{
    return rand() % 9;
}

vector<int> getRand9()
{
    vector<int> result;
    int digit = 0;
    while (result.size() != 9)
    {
        int num = getRand();
        if ((1 << num) & digit)
        {
            continue;
        }
        else
        {
            result.push_back(num);
            digit ^= (1 << num);
        }
    }
    return result;
}

void copySquare(Board &board, int src_x, int src_y, bool isRow)
{
    int rand_tmp = rand() % 2 + 1;
    int order_first[3] = {1, 2, 0};
    int order_second[3] = {2, 0, 1};
    if (rand_tmp == 2)
    {
        order_first[0] = 2;
        order_first[1] = 0;
        order_first[2] = 1;
        order_second[0] = 1;
        order_second[1] = 2;
        order_second[2] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        if (isRow)
        {
            board[src_x][i] = board[src_x + order_first[0]][src_y + i];
            board[src_x + 1][i] = board[src_x + order_first[1]][src_y + i];
            board[src_x + 2][i] = board[src_x + order_first[2]][src_y + i];
            board[src_x][i + 6] = board[src_x + order_second[0]][src_y + i];
            board[src_x + 1][i + 6] = board[src_x + order_second[1]][src_y + i];
            board[src_x + 2][i + 6] = board[src_x + order_second[2]][src_y + i];
        }
        else
        {
            board[i][src_y] = board[src_x + i][src_y + order_first[0]];
            board[i][src_y + 1] = board[src_x + i][src_y + order_first[1]];
            board[i][src_y + 2] = board[src_x + i][src_y + order_first[2]];
            board[i + 6][src_y] = board[src_x + i][src_y + order_second[0]];
            board[i + 6][src_y + 1] = board[src_x + i][src_y + order_second[1]];
            board[i + 6][src_y + 2] = board[src_x + i][src_y + order_second[2]];
        }
    }
}

Board generateBoard(int emptySize)
{
    vector<vector<char> > board(N, vector<char>(N, '.'));
    vector<int> row = getRand9();
    for (int i = 0; i < 3; i++)
    {
        board[3][i + 3] = row[i] + '1';
        board[4][i + 3] = row[i + 3] + '1';
        board[5][i + 3] = row[i + 6] + '1';
    }
    copySquare(board, 3, 3, true);
    copySquare(board, 3, 3, false);
    copySquare(board, 3, 0, false);
    copySquare(board, 3, 6, false);
    SudokuPlayer player;

    while (emptySize)
    {
        int x = rand() % 9;
        int y = rand() % 9;
        if (board[x][y] == '.')
            continue;
        char tmp = board[x][y];
        board[x][y] = '.';

        player.solveSudoku(board);
        if (player.result.size() == 1)
        {
            emptySize--;
        }
        else
        {
            board[x][y] = tmp;
        }
    }
    player.printBoard(board);
    // cout << "spaces " << player.spaces.size() << "\n";
    if (!player.checkBoard(board))
    {
        cout << "wrong board" << endl;
    }

    return board;
}

void test2()
{
    generateBoard(50);
    return;
    for (int i = 20; i < 56; i++)
    {
        generateBoard(i);
        cout << "--------------------------" << i << endl;
    }
}

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));

    // test2();
    int opt = 0;
    string outputFile, inputFile;
    int compeleteBoardCount;
    int gameNumber, gameLevel;
    int digRangeStart, digRangeEnd;
    vector<int> range;
    while ((opt = getopt(argc, argv, "c:s:n:m:r:u")) != -1)
    {
        switch (opt)
        {
        case 'c':
            compeleteBoardCount = atoi(optarg);
            if (compeleteBoardCount < 1 || compeleteBoardCount > 1000000)
            {
                printf("生成数独终盘数量范围在1～1000000之间\n");
            }
            else
            {
                ofstream outfile;
                outfile.open("game.txt", ios::out | ios::trunc);
                for (int i = 0; i < compeleteBoardCount; i++)
                {
                    Board b = generateBoard(0);
                    for (int i = 0; i < b.size(); i++)
                    {
                        for (int j = 0; j < b[i].size(); j++)
                        {
                            outfile << b[i][j] << " ";
                        }
                        outfile << "\n";
                    }
                    outfile << "----------------------------" << endl;
                }
                outfile.close();
            }
            break;
        case 's':
            break;
        case 'n':
            gameNumber = atoi(optarg);
            if (gameNumber < 1 || gameNumber > 10000)
            {
                printf("生成数独终盘数量范围在1～10000之间\n");
            }
            else
            {
            }
            break;
        case 'm':
            gameLevel = atoi(optarg);
            if (gameLevel < 1 || gameLevel > 3)
            {
                printf("生成数独终盘数量范围在1～3之间\n");
            }
            else
            {
            }
            break;
        case 'r':
            char *p;
            p = strtok(optarg, "~");
            while (p)
            {
                range.push_back(atoi(p));
                p = strtok(NULL, "~");
            }
            if (range.size() != 2)
            {
                printf("请输入一个范围参数");
            }

            break;
        case 'u':
            break;
        }
    }
    return 0;

    test();

    for (int i = 0; i < 100; i++)
        cout << getRand() << '\t';
    return 0;
}
