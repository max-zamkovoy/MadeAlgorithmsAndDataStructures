#include <iostream>
#include <assert.h>
#include <deque>
using std::cout;
using std::cin;
using std::deque;

bool expectedValue(int n) 
{
	deque<int> a;
	int command = 0, value = 0, realValue = 0;
	for (int i = 0; i < n; ++i) 
	{
		cin >> command >> value;
		if (command == 1)
		{
			a.push_front(value);
		}
		else if (command == 3) 
		{
			a.push_back(value);
		}
		else 
		{
			if (a.empty())
			{
				realValue = -1;
			}
			else if (command == 2)
			{
				realValue = a.front();
				a.pop_front();
			}
			else 
			{
				realValue = a.back();
				a.pop_back();
			}
			if (realValue != value)
			{
				return false;
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
	system("pause");
	return 0;
}