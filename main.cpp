#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <map>
using namespace std;

typedef vector<vector<char> > Board;

const int N = 9;

class SudokuPlayer
{
private:
    // 使用位运算来表示某个数是否出现过
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

    void flip(int i, int j, int digit)
    {
        rowUsed[i] ^= (1 << digit);
        columnUsed[j] ^= (1 << digit);
        blockUsed[(i / 3) * 3 + j / 3] ^= (1 << digit);
    }

    vector<Board> solveSudoku(Board board)
    {
        initState();
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (board[i][j] == '$')
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
        return result;
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

    void getResult()
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            Board board = result[i];
            printBoard(board);
        }
    }

    bool checkBoard(Board &board)
    {
        initState();
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] != '$')
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

    Board generateBoard(int digCount)
    {
        vector<vector<char> > board(N, vector<char>(N, '$'));
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

        while (digCount)
        {
            int x = rand() % 9;
            int y = rand() % 9;
            if (board[x][y] == '$')
                continue;
            char tmp = board[x][y];
            board[x][y] = '$';

            solveSudoku(board);
            if (result.size() == 1)
            {
                digCount--;
            }
            else
            {
                board[x][y] = tmp;
            }
        }
        // printBoard(board);
        // cout << "spaces " << player.spaces.size() << "\n";
        if (!checkBoard(board))
        {
            cout << "wrong board" << endl;
        }

        return board;
    }

    vector<int> getRand9()
    {
        vector<int> result;
        int digit = 0;
        while (result.size() != 9)
        {
            int num = rand() % 9;
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

vector<Board> readFile(string filePath)
{
    ifstream infile;
    vector<Board> boards;
    infile.open(filePath);
    char data[100];
    Board tmp;
    vector<char> row;
    while (!infile.eof())
    {
        infile.getline(data, 100);
        if (data[0] == '-')
        {
            boards.push_back(Board(tmp));
            tmp.clear();
            continue;
        }
        for (int i = 0; i < strlen(data); i++)
        {
            if (('1' <= data[i] && data[i] <= '9') || data[i] == '$')
            {
                row.push_back(data[i]);
            }
        }
        tmp.push_back(vector<char>(row));
        row.clear();
    }
    infile.close();
    return boards;
}

void writeFile(vector<Board> boards, ofstream &f)
{
    for (int k = 0; k < boards.size(); k++)
    {
        for (int i = 0; i < boards[k].size(); i++)
        {
            for (int j = 0; j < boards[k][i].size(); j++)
            {
                f << boards[k][i][j] << " ";
            }
            f << "\n";
        }
        f << "------- " << k << " -------" << endl;
    }
}

// 解析输入参数
map<char, string> parse(int argc, char *argv[])
{
    map<char, string> params;
    int compeleteBoardCount, gameNumber, gameLevel;
    vector<int> range;
    string inputFile;
    char opt = 0;
    while ((opt = getopt(argc, argv, "c:s:n:m:r:u")) != -1)
    {
        switch (opt)
        {
        case 'c':
            compeleteBoardCount = atoi(optarg);
            if (compeleteBoardCount < 1 || compeleteBoardCount > 1000000)
            {
                printf("生成数独终盘数量范围在1～1000000之间\n");
                exit(0);
            }
            params[opt] = string(optarg);
            break;
        case 's':
            inputFile = string(optarg);
            if (access(optarg, 0) == -1)
            {
                printf("file does not exist\n");
                exit(0);
            }
            params[opt] = string(optarg);
            break;
        case 'n':
            gameNumber = atoi(optarg);
            if (gameNumber < 1 || gameNumber > 10000)
            {
                printf("生成数独游戏数量范围在1～10000之间\n");
                exit(0);
            }
            params[opt] = string(optarg);
            break;
        case 'm':
            gameLevel = atoi(optarg);
            if (gameLevel < 1 || gameLevel > 3)
            {
                printf("生成游戏难度的范围在1～3之间\n");
                exit(0);
            }
            params[opt] = string(optarg);
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
                printf("请输入一个范围参数\n");
                exit(0);
            }
            if ((range[0] >= range[1]) || range[0] < 20 || range[1] > 55)
            {
                printf("请输入合法范围20～55\n");
                exit(0);
            }
            params[opt] = string(optarg);
            break;
        case 'u':
            params[opt] = string();
            break;
        default:
            printf("请输入合法参数\n");
            exit(0);
            break;
        }
    }
    return params;
}

void generateGame(int gameNumber, int gameLevel, vector<int> digCount, ofstream &outfile, SudokuPlayer &player)
{
    for (int i = 0; i < gameNumber; i++)
    {
        int cnt = 0;
        if (digCount.size() == 1)
        {
            cnt = digCount[0];
        }
        else
        {
            cnt = rand() % (digCount[1] - digCount[0] + 1) + digCount[0];
        }
        Board b = player.generateBoard(cnt);
        vector<Board> bs;
        bs.push_back(b);
        writeFile(bs, outfile);
    }
    outfile.close();
}

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    SudokuPlayer player;

    map<char, string> params = parse(argc, argv);
    map<char, string>::iterator it, tmp;

    int opt = 0;

    vector<int> range;
    int gameNumber;
    int gameLevel = 0;
    int solution_count = 0;

    vector<Board> boards;
    ofstream outfile;

    it = params.begin();
    while (it != params.end())
    {
        switch (it->first)
        {
        case 'c':
            outfile.open("game.txt", ios::out | ios::trunc);
            range.push_back(0);
            generateGame(atoi(it->second.c_str()), 0, range, outfile, player);
            range.clear();
            break;

        case 's':
            outfile.open("sudoku.txt", ios::out | ios::trunc);
            boards = readFile(it->second);
            for (int i = 0; i < boards.size(); i++)
            {
                vector<Board> result = player.solveSudoku(boards[i]);
                writeFile(result, outfile);
            }
            outfile.close();
            break;

        case 'n':
        case 'm':
        case 'r':
        case 'u':
            tmp = params.find('n');
            if (tmp == params.end())
            {
                printf("缺少参数 n \n");
                exit(0);
            }

            gameNumber = atoi(tmp->second.c_str());

            tmp = params.find('u');
            if (tmp != params.end())
            {
                solution_count = 1;
            }

            tmp = params.find('m');
            if (tmp != params.end())
            {
                gameLevel = atoi(tmp->second.c_str());
            }

            tmp = params.find('r');
            if (tmp != params.end())
            {
                char *p;
                char *pc = new char[100];
                strcpy(pc, tmp->second.c_str());
                p = strtok(pc, "~");
                while (p)
                {
                    range.push_back(atoi(p));
                    p = strtok(NULL, "~");
                }
            }
            else
            {
                // 根据不同级别采取挖空数量不同
                if (gameLevel == 1)
                {
                    range.push_back(20);
                    range.push_back(30);
                }
                else if (gameLevel == 2)
                {
                    range.push_back(30);
                    range.push_back(40);
                }
                else if (gameLevel == 3)
                {
                    range.push_back(40);
                    range.push_back(55);
                }
                else
                {
                    range.push_back(20);
                    range.push_back(55);
                }
            }

            outfile.open("game.txt", ios::out | ios::trunc);
            generateGame(gameNumber, gameLevel, range, outfile, player);
            range.clear();
            break;
        }
        // cout << it->first << ' ' << it->second << endl;
        it++;
    }

    return 0;
}
