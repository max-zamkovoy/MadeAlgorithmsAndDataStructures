#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

class CArray
{
public:
	CArray(int size);
	~CArray() { delete Mas; };
	int Amount() { return Number; };
	int GetFirst();
	void SiftUp(int);
	void SiftDown(int);
	void AddElement(int);
private:
	int* Mas;
	int SizeMas;
	int Number;
	void Swap(int, int);
};

int CArray::GetFirst()
{
	int temp = Mas[0];
	Swap(0, --Number);
	SiftDown(0);
	return temp;
}

void CArray::SiftDown(int i)
{
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	int largest = i;
	if (left < Number && Mas[left] < Mas[i])
		largest = left;
	if (right < Number && Mas[right] < Mas[largest])
		largest = right;
	if (largest != i)
	{
		Swap(i, largest);
		this->SiftDown(largest);
	}
}


void CArray::SiftUp(int i)
{
	int parent;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (Mas[i] <= Mas[parent])
			Swap(i, parent);
		i = parent;
	}
}

void CArray::Swap(int i, int j)
{
	Mas[i] = Mas[i] ^ Mas[j];
	Mas[j] = Mas[i] ^ Mas[j];
	Mas[i] = Mas[i] ^ Mas[j];
}

CArray::CArray(int size) : SizeMas(size), Number(0)
{
	Mas = new int[SizeMas];
}

void CArray::AddElement(int Data)
{
	Mas[Number] = Data;
	SiftUp(Number);
	Number++;
}

int CalculateTime(CArray* IntMas)
{
	int time = 0;
	int Sum = 0;
	while (IntMas->Amount() > 1)
	{
		Sum = IntMas->GetFirst();
		Sum += IntMas->GetFirst();
		IntMas->AddElement(Sum);
		time += Sum;
	}
	return time;
}

int main()
{
	int Number = 0;
	std::cin >> Number;

	CArray IntMas(Number);

	int num;
	for (int i = 0; i < Number; ++i)
	{
		std::cin >> num;
		IntMas.AddElement(num);
	}

	std::cout << CalculateTime(&IntMas) << std::endl;
	return 0;
}