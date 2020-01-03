#include <iostream>
using namespace std;
string z_func(string some_string, int start){
//Same function for z-func values, except that this time the '?' symbols are added
    int str_len = some_string.length();
    int z_vals[str_len];
    for(int i = 0; i < str_len; i++){
        z_vals[i] = 0;
    }
    int r_border = 0;
    int l_border = 0;
    string res = "";
//Calculating z-values with respect to '?' in substring
    for(int i = 1; i < str_len; i++){
        z_vals[i] = 0;
        if(i <= r_border){
            z_vals[i] = min(r_border-i+1,z_vals[i-l_border]);
        }
        while(i+z_vals[i] < str_len && (some_string[z_vals[i]] == some_string[i+z_vals[i]] || some_string[z_vals[i]] == '?')){
            z_vals[i] += 1;
        }
        if(i + z_vals[i] - 1 > r_border){
            l_border = i;
            r_border = i + z_vals[i] - 1;
        }
        if(i > start && z_vals[i] == start){
            res += to_string(i - start - 1) + " ";
        }
    }
    return res.substr(0,res.length() - 1);
}

int main() {
    string substr;
    string main_string;
    cin >> substr;
    cin >> main_string;
    int substr_len = substr.length();
    substr += "$" + main_string;
    cout << z_func(substr,substr_len);
}



















