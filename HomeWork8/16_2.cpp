#include <iostream>
#include <vector>
using namespace std;

vector<int> findPattern(const string &pattern, const string &some_string){
    int z_func[pattern.length()];
    for(int i = 0; i < pattern.length(); i++){
        z_func[i] = 0;
    }
    vector<int> positions;
    int l_border = 0, r_border = 0;
    for(int i = 1; i < pattern.length(); i++){
        int z_val = 0;
        if( i <= r_border){
            z_val = min(r_border - i + 1, z_func[i - l_border]);
        }
        while(i + z_val < pattern.length() && pattern[z_val] == pattern[i + z_val]){
            z_val++;
        }
        if (i + z_val - 1 > r_border) {
            l_border = i;
            r_border = i + z_val - 1;
        }
        z_func[i] = z_val;

    }
    l_border = 0, r_border = 0;
    for(int i = 0; i < some_string.length(); i++){
        int z_val = 0;
        if( i <= r_border){
            z_val = min(r_border - i + 1, z_func[i - l_border]);
        }
        while (i + z_val < some_string.length() && pattern[z_val] == some_string[i + z_val]) {
            z_val++;
        }
        if (i + z_val - 1 > r_border) {
            l_border = i;
            r_border = i + z_val - 1;
        }
        if(z_val == pattern.length()) positions.push_back(i);
    }
    return positions;
}


int main() {
    string substr;
    string main_string;
    cin >> substr;
    cin >> main_string;
    vector<int> positions = findPattern(substr,main_string);
    for(auto ind: positions){
        cout << ind << " ";
    }
    return 0;
}


