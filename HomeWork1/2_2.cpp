#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// Куча целых чисел, реализованная с помощью массива.
class Heap
{
public:
    explicit Heap(int size);
    ~Heap() { delete Mas; };
    int Size() { return Number; };
    int GetFirst();
    void AddElement(int);		//Добавление элемента
private:
    int* Mas;	//Массив данных
    int SizeMas;	//Размер кучи
    int Number;	//Количество уже заполненных элементов
    void Swap(int a, int b);		//Смена мест элементов с заданными индексами
    void SiftUp(int el);
    void SiftDown(int el);
};

int Heap::GetFirst()
{
    int temp = Mas[0];
    Swap(0, --Number);
    SiftDown(0);
    return temp;
}

// Проталкивание элемента вниз.
void Heap::SiftDown(int i)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    // Ищем меньшего сына, если такой есть.
    int largest = i;
    if (left < Number && Mas[left] < Mas[i])
        largest = left;
    if (right < Number && Mas[right] < Mas[largest])
        largest = right;
    // Если меньший сын есть, то проталкиваем корень в него.
    if (largest != i)
    {
        Swap(i, largest);
        this->SiftDown(largest);
    }
}


// Проталкивание элемента наверх.
void Heap::SiftUp(int i)
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

void Heap::Swap(int i, int j)
{
    Mas[i] = Mas[i] ^ Mas[j];
    Mas[j] = Mas[i] ^ Mas[j];
    Mas[i] = Mas[i] ^ Mas[j];
}

Heap::Heap(int size) : SizeMas(size), Number(0)
{
    Mas = new int[SizeMas]; // Создаем буфер.
}

void Heap::AddElement(int Data)
{
    Mas[Number] = Data;
    SiftUp(Number);
    Number++;
}

//Расчет времени
int CalculateTime(Heap* IntMas)
{
    int time = 0;
    int Sum = 0;
    while (IntMas->Size() > 1)
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
    //Получение количство слогаемых
    int Number = 0;
    std::cin >> Number;

    Heap IntMas(Number);

    int num;
    for (int i = 0; i < Number; ++i)
    {
        std::cin >> num;
        IntMas.AddElement(num);
    }

    std::cout << CalculateTime(&IntMas) << std::endl;
    return 0;
}