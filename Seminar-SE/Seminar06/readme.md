# Шести семинар по обектно ориентирано програмиране.

## Голяма четворка
В уводния курс разбрахме, че ако не подадем по референция на него му се прави копие. Това важи както за примитивните типове от данни така и за класовете и структурите създадени от нас. Това е и причината на константните референции да се обръща специално внимание - при големи обекти копието е скъпо. Но как точно става копието при създадени от нас класове? Възможно ли е да имплементираме наша логика за копиране?

Да. С копирането на един обект в някакъв смисъл започва живота на друг, та логиката за копиране е естествено да се описва в конструктор. Когато създаваме клас или структура следните четири се генерират автоматично за нас:

* Конструктор по подразбиране
* Копиращ конструктор - съдържа логиката за копиране на обект
* Оператор за присвояване - съдържа логиката за присвояване на обект
* Деструктор

С конструктора по подразбиране и деструктора се занимавахме подробно миналия път.

```cpp
class Entity
{
    A a;
    B b;
    C ;
};

int main()
{
    Entity e; // Конструктор по подразбиране.
    Entity e1(e); // Копиращ конструктор.
    e = e1; // Оператор за присвояване.
} // Деструктор на e и e1.
```

### Как работят дефинираните от компилатора функции
Всяка една от тези функции **извиква 'рекурсивно' същите функции върху член-данните**.


### Конструктор за копиране - синтаксис и пример
В конструктора за копиране държим логиката по копиране на обекта (както името може да подскаже).
Нека разгледаме следния пример:

```cpp
#include <iostream>

class Entity
{
private:
	int x;
public:
	Entity(int x) : x(x) {}

	int get() const
	{
		return x;
	}
};

void f(Entity e)
{
	std::cout << e.get();
}

int main()
{
	Entity e(12);
	std::cout << e.get() << std::endl;
	f(e);
}
```
В случая на обекта Entity му се прави копие. копиращия конструктор отговаря за това копие а той е автоматично генериран от компилатора. Очакваното поведение е да копира всички член данни. В случая имаме една променлива от тип int (и знаем как се копират тези променливи). Следователно е ясно какво ще стане тук - ще се изведе два пъти 12.

Нека се опитаме (за примера) да променим малко кода. Нека в Entity класът добавим следния публичен ред:
```cpp
// конструктор за копиране
Entity(const Entity& other) : x(2 * other.x) {}
```

Това е конструктор, който приема константна референция към друг обект. **Обектът, който се създава копира подадения по референция обект!**. Все пак това е С++, та имаме контрол над това как се случва копирането. Какво ще изведе кодът след като добавихме този ред?

Ще изведе 12 и 24. След имплементирането на наша логика за копиране новосъздадения обект има стойност **два пъти по-голяма от обектът, който използва за да се копира.**

### Оператор за присвояване - синтаксис и пример.
В оператора за присвояване се съдържаа логиката за присвояване на обект (какво става когато напишем e1 = e2 където и двата обекта са вече изградени).

```cpp
#include <iostream>

class Entity
{
private:
	int x;
public:
	Entity(int x) : x(x) {}

	// конструктор за копиране
	Entity(const Entity& other) : x(2 * other.x) {}

	int get() const
	{
		return x;
	}
};

int main()
{
	Entity e(12);
	
    // Тук се използва оператор за присвояване, но се създава нов обект.
    // В случая се вика копиращ конструктор. Това е еквивалентно на 
    // Entity e1(e).
	Entity e1 = e;

	Entity e2(14);

    // Колко ще е стойността на e2.get()?
	e2 = e;

	std::cout << e.get() << " " << e1.get() << " " << e2.get();
}
```

В случая отново ще се случи очакваното - e има стойност 12, e1 има стойност 24 заради копиращия конструктор а e2 има стойност 12.
Можем ли да направим наша логика за присвояване? Да - нека добавим следния публичен ред в класа Entity:

```cpp
Entity& operator=(const Entity& other)
{
	x = other.x / 2;
    return *this;
}
```

Сега при присвояване обектът от лявата страна ще има два пъти по - малка стойност от десния. Нека забележим, че ```operator=``` връща референция към текущия обект. Това е очаквано, най-малкото искаме да можем да правим неща като ```e1 = e2 = e3```. Това са две изпълнения на ```operator=``` и за да се изпълни второто то има нужда от референция (която първото изпълнение връща).

### Проблем с генерираните от компилатора функции
Нека разгледаме следния пример от последния семинар:
```cpp
#include <iostream>
#include <cstring>

class Entity
{
private:
    char* label;

    void free()
    {
        delete[] label;
    }
public:
    Entity() : label(nullptr) {}

    Entity(const char* c) 
    {
        size_t len = strlen(c);
        label = new char[len + 1];
        strncpy(label, c, len);
        label[len] = '\0';
    }

    const char* getLabel() const
    {
        return label;
    }

    ~Entity()
    {
        free();
    }
};

int main()
{
    Entity e("label value");
    Entity e1(e);
}
```
Следния пример ни води до проблем. Къде е проблема???
* Създава се обект е - за него се заделя памет
* Създава се обект е1 - на него му се вика конструктор за копиране
* Конструктор за копиране рекурсивно копира всички член данни. Тук има една член данна.
* Сега e1::label е копие на e::label
* Два копирани указателя имат различен адрес в паметта, но **сочат към една и съща памет**
* Първо се вика деструктора на е - той трие паметта към кояно сочи e::label
* След това се вика деструктора на e1 - той трие паметта към която сочи e1::label, но **тя вече е изтрита!**. Това води до проблем.

Къде е проблема тук? Проблема е, че очакваме **друго поведение при копиране на указателя**. Това е така нареченото **shallow copy**. Вече знаем как да направим наша логика по копиране - нека оправим проблема:

```cpp
Entity(const Entity& other)
{
    label = new char[strlen(other.label) + 1] {};
    strcpy(label, other.label);
    // Ако имаме други член данни трябва да се погрижим и за техните стойности!!!
}
```
сега вече не правим копие на указателя а заделяме нова памет. Кодът обаче има още един проблем:

```cpp
int main()
{
    Entity e("label value");
    Entity e1(e);

    e = e1;
}
```
Това чупи програмата ни, но защо? Става нещо подобно както с конструктора. Отново се случва неправилно копиране на указателя и го трием два пъти. 

Проблема решаваме по следния начин:

```cpp
Entity& operator=(const Entity& other)
{
    if (this != &other)
    {
        free();
        label = new char[strlen(other.label) + 1] {};
        strcpy(label, other.label);
    }
    return *this;
}
```

Нека хвърлим малко светлина:
* Проверката ```this != &other``` ни спасява да не напишем нещо от сорта на ```e = e```. Сравнява се **по адрес а не по стойност** - ```this``` е указател &other връща адрес.
* free() - преди да заделим място за новата стойност трябва да изтрием старата. Иначе става отечка.
* Следващите два реда трябва да се изнесат във функция. Тази логика я имаме в копиращия конструктор както и в оператора за присвояване - нека не дублираме код.
* return *this - връща референция към текущия обект.

Финалната версия е качена в `big-four.cpp`. 

При липсата на външен ресурс често не разписваме тези функции - генерираните от компилатора ни вършат достатъчно добра работа.
Когато работим с такъв обаче е добре да разпишем четирите функции. Също следната конвенция важи:
* Логиката за изтриване **се извежда във отделна функция**
* Логиката за копиране **се извежда в отделна функция**
* Когато копираме **трябва да се порижим за всички член данни не само за динамичната памет**

## Допълнителни теми
* Какво става когато в копиращия конструктор не се подаде константна референция?
* Какво става при деструктор, който е private
* Още веднъж - кога разписваме голяма четворка?

## Задача първа
Реализирайте клас String, който се използва за работа с низове с произволна дължина. Трябва да поддържаме следния интерфейс:
* length - дължината на низа 
* c_str - c style указател към низа
* concat - конкатенация на два низа

## Задача втора
Напишете клас Set, който съдържа множество от числа (без повторения) в диапазона от 0 до n-1, където n е подадено в началото (1 <= n <= 1000). Класът трябва да пази дали съдържа дадено число в битове, т.е ако съдържа дадено число, съответвеният последователен бит ще бъде 1, в противен случай 0. Пример:

{3, 4, 6} => битове на множеството ще бъдат 00011010

{1, 8, 9} => 01000000 11000000

Класът трябва да има следните функции.

* Добавяне на число
* Проверка дали съдържа число
* Принтиране на всички числа, които съдържа
* Член-функция, която приема друго множество и връща тяхното обединение
* Член-функция, която приема друго множество и връща тяхното сечение