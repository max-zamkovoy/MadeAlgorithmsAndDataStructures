#include <iostream>
#include <cassert>

using namespace std;

class Deque {
public:
    void pushBack(int value);
    void pushFront(int value);
    int popBack();
    int popFront();
    bool isEmpty() const;
    ~Deque();

private:
    int capacity = 8;
    int size = 0;
    int head = 0;
    int tail = 0;
    int *buffer = new int[capacity];
    void handleBuffer();
};

void Deque::pushBack(int value) {
    handleBuffer();
    if (isEmpty()) {
        buffer[tail] = value;
    } else {
        if (tail == 0) {
            tail = capacity;
        }
        tail--;
        buffer[tail] = value;
    }
    size++;
}

void Deque::pushFront(int value) {
    handleBuffer();
    if (isEmpty()) {
        buffer[head] = value;
    } else {
        if (head == capacity - 1) {
            head = -1;
        }
        head++;
        buffer[head] = value;
    }
    size++;
}

int Deque::popBack() {
    assert(!isEmpty());
    size--;
    if (isEmpty()) {
        return buffer[tail];
    } else {
        if (tail == capacity - 1) {
            tail = 0;
            return buffer[capacity - 1];
        }
    }
    return buffer[tail++];
}

int Deque::popFront() {
    assert(!isEmpty());
    size--;
    if (head == tail) {
        return buffer[head];
    } else {
        if (head == 0) {
            head = capacity - 1;
            return buffer[0];
        }
    }
    return buffer[head--];
}

bool Deque::isEmpty() const{
    return size == 0;
}

Deque::~Deque() {
    delete[] buffer;
}

void Deque::handleBuffer() {
    if (size == capacity) {
        capacity *= 2;
        int *tmp = new int[capacity];
        copy(buffer + tail, buffer + size, tmp);
        if (head < tail) {
            copy(buffer, buffer + tail, tmp + size - tail);
        }
        delete[] buffer;
        buffer = tmp;
        tail = 0;
        head = size - 1;
    }
}



bool expectedValue(int n)
{
    Deque deque;
    for (int i = 0; i < n; i++) {
        int command = 0;
        int value = 0;
        cin >> command >> value;
        if (command == 1) {
            deque.pushFront(value);
        } else if (command == 3) {
            deque.pushBack(value);
        } else {
            if (deque.isEmpty()) {
                if (value != -1) {
                    return false;
                }
            } else {
                if (((command == 2) && (deque.popFront() != value)) || ((command == 4) && (deque.popBack() != value))) {
                    return false;
                }
            }
        }
    }
    return true;
}

int main()
{
    int n = 0;
    cin >> n;
    if (expectedValue(n))
    {
        cout << "YES";
    }
    else
    {
        cout << "NO";
    }
    return 0;
}