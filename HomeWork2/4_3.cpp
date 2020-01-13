#include <iostream>
using namespace std;
int getPartition(int* array, int first, int last) {
    int middle = (first + last) / 2;
    if (array[middle] < array[first]) {
        swap(array[first], array[middle]);
    }
    if (array[last] < array[first]) {
        swap(array[first], array[last]);
    }
    if (array[middle]<array[last]) {
        swap(array[middle], array[last]);
    }
    int pivot = array[last];
    int i=last, j=i-1;
    while (true) {
        if (j == first - 1) {
            swap(array[last], array[i]);
            return i;
        }
        if (array[j] < pivot)
            j--;
        else
            swap(array[--i], array[j--]);
    }
}

int find_order_stat_k(int* array, int arr_size, int k) {
    int left = 0;
    int right = arr_size - 1;
    while (true) {
        int m = getPartition(array, left, right);
        if (m == k) {
            return array[m];
        }
        if (m > k) {
            right = m;
        }
        else {
            left = m + 1;
        }
    }
}

int main() {
    int n=0, k=0;
    cin >> n >> k;
    int* array = new int[n];
    for (int i = 0; i < n; i++) {
        cin >> array[i];
    }
    cout << find_order_stat_k(array, n, k) << endl;
    return 0;
}