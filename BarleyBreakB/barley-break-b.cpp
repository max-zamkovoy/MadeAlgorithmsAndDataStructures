#include <iostream>
#include <ctime>
#include <queue>
#include <vector>
#include <random>
//#include <bits/stdc++.h>
#include <chrono>

using namespace std;

typedef unsigned int ui;

bool isSolved = false;
int Size;
char Table[16][16];
char movableTable[16][4];
char path[85];
ui nodeCount = 0;

void MakeMovableTable(int size){
    int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int board[4][4];
    for(int y = 0; y<size; ++y){
        for(int x = 0; x<size; ++x){
            board[y][x] = x+y*size;
        }
    }
    int dx, dy, j;
    for(int y = 0; y<size; ++y){
        for(int x = 0; x<size; ++x){
            for(int i = 0; i<4; ++i){
                dx = moves[i][0];
                dy = moves[i][1];
                if(x+dx<0 || y+dy<0 || x+dx>=size || y+dy>=size){
                    j = -1;
                }
                else{
                    j = board[y+dy][x+dx];
                }
                movableTable[x+y*size][i] = j;
            }
        }
    }
}

void CreateTable(int size){
    for(int y = 0, i = 1; y<size*size; ++y, (++i)%(size*size)){
        for(int x = 0; x < size * size; ++x){
            if(i == 0){
                Table[i][x] = 0;
            }
            else{
                Table[i][x] = abs((y / size) - (x / size)) + abs((y % size) - (x % size));
            }
        }
    }
}

char GetDistance(int *row, char md, int nums){
    if (nums > 1){
        if (nums == 2){
            if (row[0] > row[1])
                md += 2;
        }
        else if (nums == 3){
            if (row[0] > row[1] || row[0] > row[2])
                md += 2;
            if (row[1] > row[2])
                md += 2;
        }
        else if (nums == 4){
            if (row[0] > row[1] || row[0] > row[2] || row[0] > row[3])
                md += 2;
            if (row[1] > row[2] || row[1] > row[3])
                md += 2;
            if (row[2] > row[3])
                md += 2;
        }
    }
    return md;
}

char GetManhattan(char* puzzle){
    int i, j, x, md = 0;
    int k, n;
    int temp[4];

    for(i = 0; i < Size*Size; ++i){
        md += Table[puzzle[i]][i];
    }
    for (i = 0, x = 1; i<Size; ++i, ++x){
        k = 0;
        for (j = 0; j<Size; ++j){
            n = puzzle[i*Size+j];
            if (n <= Size*x && n>Size*(x-1)){
                temp[k++]= n;
            }
        }
        md = GetDistance(temp, md, k);
    }
    for (i = 0, x = 1; i < Size; ++i, ++x){
        k = 0;
        for (j = 0; j < Size; ++j){
            n = puzzle[j * Size + i];
            if (n == x || n == x + Size || n == x + Size * 2 || n == x + Size * 3){
                temp[k++] = n;
            }
        }
        md = GetDistance(temp, md, k);
    }
    return md;
}

char GetBlank(char *puzzle){
    for(int i = 0; i < Size * Size; ++i){
        if (puzzle[i] == 0)
        {
            return i;
        }
    }
}

void PrintPath(char depth){
    cout << (int)depth << '\n';
    for(int i = 0; i < depth; ++i){
        if(path[i] == 0)
            cout << "R ";
        if(path[i] == 1)
            cout << "L ";
        if(path[i] == 2)
            cout << "D ";
        if(path[i] == 3)
            cout << "U ";
    }
    printf("\n\n");
}

char Solve(char depth, char maxDepth, char* puzzle, char lastMove, char blank){
    char h, f, min;
    nodeCount += 1;
    h = GetManhattan(puzzle);
    f = depth + h;
    if (f > maxDepth ){
        return f;
    }

    if (h == 0){
        isSolved = true;
        return f;
    }

    min = 127;
    for (int move = 0; move < 4; ++move){
        if(lastMove == -1 || (move + lastMove) % 4 != 1){
            char newBlank = movableTable[blank][move];
            if(newBlank == -1)
                continue;
            puzzle[blank] = puzzle[newBlank];
            puzzle[newBlank] = 0;
            f = Solve(depth + 1, maxDepth, puzzle, move, newBlank);
            puzzle[newBlank] = puzzle[blank];
            puzzle[blank] = 0;
            if (f < min){
                min = f;
            }
            if (isSolved){
                path[depth] = move;
                return min;
            }
        }
    }
    return min;
}

void Solve(char* puzzle){
    char h, depth;
    char lastMove = -1;
    char blank = GetBlank(puzzle);

    isSolved = false;
    ui totalCount = 0;

    h = GetManhattan(puzzle);
    depth = h;
    while (true){
        nodeCount = 0;
        depth = Solve(0, depth, puzzle, lastMove, blank);
        totalCount += nodeCount;
        if (isSolved){
            PrintPath(depth);
            return;
        }
        h = depth;
    }
}

int getInvCount(int arr[])
{
    int inv_count = 0;
    for (int i = 0; i < 4 * 4 - 1; i++)
    {
        for (int j = i + 1; j < 4 * 4; j++)
        {
            if (arr[j] && arr[i] && arr[i] > arr[j])
                inv_count++;
        }
    }
    return inv_count;
}
bool is_solvable(int initial[4][4]){
    int inv_count = getInvCount((int*)initial);
    int x_pos = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(initial[i][j] == 0)
                x_pos = i;
        }
    }
    return ((x_pos % 2 == 0 && inv_count % 2 == 1) || (x_pos % 2 == 1 && inv_count % 2 == 0));
}

int main(){
    int  size=4;
    char puzzle[16];
    for(int i = 0; i < 16; i++){
        int temp;
        cin >> temp;
        puzzle[i] = (char)temp;
    }
    int for_testing[4][4];
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            for_testing[i][j] = (int)puzzle[i * 4 + j];
        }
    }
    char p[16];
    CreateTable(size);
    MakeMovableTable(size);
    Size = size;

    if(!is_solvable(for_testing)){
        cout << -1;
    }
    else
        Solve(puzzle);
    return 0;
}