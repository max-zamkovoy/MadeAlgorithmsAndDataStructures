#include <iostream>

template<class T>
size_t selectPivot(T* data, size_t l, size_t r) 
{
	size_t half = (l + r - 1) / 2; 

	if (data[l] < data[half]) 
	{
		if (data[r] < data[l]) 
		{
			return l;
		}
		else if (data[r] < data[half]) 
		{
			return r;
		}
		return half;
	}
	else
	{
		if (data[r] < data[half]) 
		{
			return half;
		}
		else if (data[r] < data[l]) 
		{
			return r;
		}
		return l;
	}
}

template<class T>
size_t partition(T* data, size_t l, size_t r) 
{
	size_t pivotPos = selectPivot(data, l, r);

	if (pivotPos != r - 1) 
	{
		std::swap(data[r - 1], data[pivotPos]);
	}

	size_t i = l, j = l;
	T pivot = data[r - 1];
	while (j < r - 1) 
	{
		if (data[j] <= pivot) 
		{
			std::swap(data[i++], data[j]);
		}
		j++;
	}
	if (i != r - 1) 
	{ 
		std::swap(data[i], data[r - 1]);
	}
	return i;
}

template<class T>
T findKStatistics(T* data, size_t l, size_t r, size_t k)
{
	size_t lastPivotPos = 0, left = l, right = r;
	while (left < right) {
		if ((lastPivotPos = partition(data, left, right)) == k) 
		{ 
			return data[lastPivotPos];
		}
		else if (lastPivotPos > k) 
		{
			right = lastPivotPos;
		}
		else
		{
			left = lastPivotPos + 1;
		}
	}
	return data[lastPivotPos];
}

int main() 
{
	size_t n = 0, k = 0; 

	std::cin >> n >> k;

	int* numbers = new int[n];
	for (size_t i = 0; i < (size_t)n; i++) {
		std::cin >> numbers[i];
	}

	std::cout << findKStatistics(numbers, 0, (size_t)n, k);

	delete[] numbers;
}