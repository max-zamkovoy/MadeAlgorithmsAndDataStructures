#include <iostream>
#include <queue>
#include <cstring>
#include <limits>
#include <cstdint>
#include <memory.h>
using namespace std;
#define N 4


struct Node{
    //Solution node
    Node* par_node;
    int position[4][4];
    int x,y;
    int cost;
    int num_of_moves;
    string history;
};

void swap(Node* node, int x, int y, int x_new, int y_new){
    //Function to swap elements in array
    int to_swap = node->position[x][y];
    node->position[x][y] = node->position[x_new][y_new];
    node->position[x_new][y_new] = to_swap;
}

Node* createNode(int position[4][4], int x, int y, int x_new, int y_new, int num_of_moves, Node* parent){
    //Node constructor
    Node* node = new Node;
    node->par_node = parent;
    memcpy(node->position,position,sizeof(node->position));
    swap(node->position[x][y],node->position[x_new][y_new]);
    node->cost = 400;
    node->num_of_moves = num_of_moves;
    node->x = x_new;
    node->y = y_new;
    if(parent == NULL)
        node->history = "";
    else
       node->history = parent->history;
    return node;
}

//Encoded directions of movement - up, right, down, left
int row[] = { 1, 0, -1, 0 };
int col[] = { 0, -1, 0, 1 };

struct comp{
    //Comparator for the
    bool operator()(const Node* left_node, const Node* right_node) const{
        return (left_node->cost + left_node->num_of_moves) > (right_node->cost + right_node->num_of_moves);
    }
};

int calcCost(int initial[4][4], int final[4][4]){
    //Cost of the node
    int count = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(initial[i][j] && initial[i][j] != final[i][j]){
                count++;
            }
        }
    }
    return count;
}

int isValid(int x, int y){
    //If coordinates are valid
    return (x >= 0 && x < 4 && y >= 0 && y < 4);
}
int getInvCount(int arr[])
{
    //Count number of inversions in flattened array
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
    //Boolean function
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


void solve(int initial[4][4], int final[4][4]){
    //Solving the puzzle
    int x,y;
    //Finding the zero coordinates
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(initial[i][j] == 0)
                x = i, y = j;
        }
    }

    //Creating the queue
    priority_queue<Node*, vector<Node*>, comp> solutions;
    Node* root_node = createNode(initial,x,y,x,y,0,NULL);
    root_node->cost = calcCost(initial,final);
    solutions.push(root_node);
    while(!solutions.empty()){
        //Exploring the search space, expanding the node with minimal cost, stop when the cost is equal to zero
        Node* optimal = solutions.top();
        solutions.pop();
        if(optimal->cost == 0){
            cout << optimal->num_of_moves << endl << optimal->history;
            return;
        }
        for(int i = 0; i < 4; i++){
            //Expanding the node in all possible directions
            if(isValid(optimal->x + row[i], optimal->y + col[i])){
                Node* new_node = createNode(optimal->position, optimal->x,
                                      optimal->y, optimal->x + row[i],
                                      optimal->y + col[i],
                                      optimal->num_of_moves + 1, optimal);
                new_node->cost = calcCost(new_node->position, final);
                if(i == 0)
                    new_node->history += "U ";
                if(i == 1)
                    new_node->history += "R ";
                if(i == 2)
                    new_node->history += "D ";
                if(i == 3)
                    new_node->history += "L ";


                // Add child to list of live nodes
                solutions.push(new_node);
            }
        }
    }
}



int main() {
    //15 Puzzle, Branch and Bound with the use of priority queue
    int start[4][4];
    for(int i = 0; i < 4; i++){
        string temp;
        getline(cin,temp);
        int pos = temp.find(' ');
        int ind = 0;
        while(pos != -1){
            start[i][ind] = stoi(temp.substr(0,pos));
            temp = temp.substr(pos + 1);
            pos = temp.find(' ');
            ind++;
        }
        start[i][N-1] = stoi(temp);
    }
    int final[N][N] =
            {
                    {1,2,3,4},
                    {5,6,7,8},
                    {9,10,11,12},
                    {13,14,15,0}
            };
    if(is_solvable(start)){
        solve(start,final);
    }
    else{
        cout << -1;
    }
    return 0;
}