#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include "head.h"

#define A 0.01
#define EPS1 0.0001
#define EPS2 0.0001

int iter;  // число итераций при поиске корня уравнения

float left, right;  // границы отрезка

typedef float (*pf)(float);  // указатель на функцию

// _____Основной функционал_____

// Поиск корня уравнения F(x) = f(x) - g(x) = 0 методом касательных.
// Отрезок подобран так, что уравнение на нем имеет корень.
// Получает указатели на функции f и g, их производные первого и второго порядков,
// границы отрезка, точность вычисления.
// Возвращает корень уравнения - абсциссу точки пересечения.
float root(pf f, pf g, pf df, pf dg, pf d2f, pf d2g, float a, float b, float eps1);

// Нахождение отрезка, на котором уравнение F(x) = f(x) - g(x) = 0 будет иметь корень.
// Точнее, его правой границы, т.к. левая по условию > 0 и ее можно зафиксировать.
// Получает указатели на функции f и g.
// Возвращает правую границу отрезка.
float find_sec(pf f, pf g);

// Вычисление определенного интеграла методом прямоугольников.
// Получает указатель на функцию, границы отрезка, точность вычисления.
// Возвращает значение интеграла.
float integral(pf f, float a, float b, float eps2);

// _____Вспомогательные функции_____

// Вычисление точек пересечения графиков.
// Получает значение флага:
// 1 - печать абсцисс точек пересечения
// 0 - печать числа итераций при вычислении корня уравнения F(x) = 0.
void points(char func1, char func2, float a, float b, float eps, int flag);

// Тестирование функции интеграла.
// Получает номер функции, границы отрезка, точность вычисления.
// Выводит значение определенного интеграла.
void test(char f, float a, float b, float eps);

// Решение задачи на нахождение площади. Основная задача программы.
// Ничего не получает, ничего не возвращает. Считает площадь между
// графиками функций и выводит ее.
void square(void);

// Проверяет, есть ли корень уравнения F(x) = f(x) - g(x) на этом отрезке.
// Получает номера функций, границы отрезка.
// Возвращает 1, если есть корень и 0, если нет корня.
int test_sec(char f, char g, float a, float b);

// Функция-заглушка, для нахождения корня функции.
float empty(float x)
{
  return 0.0 * x;
}

float min(float a, float b)
{
  return a < b ? a : b;
}

float max(float a, float b)
{
  return a > b ? a : b;
}


int main(int argc, char *argv[])
{
  #ifndef __unix__
  system("chcp 65001");
  #endif

  char res = 0;
  char func1, func2;
  float a, b, eps;

  // обработка аргументов командной строки
  while ((res = getopt(argc, argv, "hfma:n:i:")) != -1)
  {
    switch (res)
    {
      case 'h':
           printf("=========================================|HELP|=========================================\n"
                  "-f вывести функции, заданные вариантом\n"
                  "-m вывести искомую площадь\n"
                  "-a <func1(1,2,3), func2(1,2,3), a, b, eps> вывести абсциссы точек пересечения функций\n"
                  "-n <func1(1,2,3), func2(1,2,3), a, b, eps> вывести число итераций при вычислении корня\n"
                  "-i <func(1,2,3), a, b, eps> вычисление интеграла для функций на заданном отрезке\n"
                  "========================================================================================\n");
           break;
      case 'f':
           printf("Функции, заданные вариантом:\n"
                  "1) f1 = 3 * (0.5 / (x + 1) + 1)\n"
                  "2) f2 = 2.5x - 9.5\n"
                  "3) f3 = 5 / x, где x > 0\n");
           break;
      case 'm':
           square();
           break;
      case 'a':
           func1 = argv[optind-1][0];
           func2 = argv[optind][0];
           a = atof(argv[++optind]);
           b = atof(argv[++optind]);
           eps = atof(argv[++optind]);
           points(func1, func2, a, b, eps, 1);
           break;
      case 'n':
           func1 = argv[optind-1][0];
           func2 = argv[optind][0];
           a = atof(argv[++optind]);
           b = atof(argv[++optind]);
           eps = atof(argv[++optind]);
           points(func1, func2, a, b, eps, 0);
           break;
      case 'i':
           func1 = argv[optind-1][0];  // optind указывает на следующий аргумент (после вызова getopt)
           a = atof(argv[optind]);
           b = atof(argv[++optind]);
           eps = atof(argv[++optind]);
           test(func1, a, b, eps);
           break;
      default: break;
    }
  }

  return 0;
}



int test_sec(char f, char g, float a, float b)
{
  float res = 1.0;
  left = a;
  right = b;

  if (f == '1' && g == '1')
  {
    // точка разрыва - -1
    if (a < -1)
    {
      b = min(b, -1.001);
      right = b;
      res = f1(a)*f1(b);
    }
    else if (b > -1)
    {
      a = max(a, -1.001);
      left = a;
      res = f1(a)*f1(b);
    }
  }
  else if (f == '2' && g == '2') res = f2(a)*f2(b);  // непрерывная ф-ия
  else if (f == '3' && g == '3') res = 1.0;  // нет решений
  else if ((f == '1' && g == '2') || (f == '2' && g == '2'))
  {
    // точка разрыва - -1
    if (a < -1)
    {
      b = min(b, -1.001);
      right = b;
      res = (f1(a)-f2(a))*(f1(b) - f2(b));
    }
    else if (b > -1.001)
    {
      a = max(a, -1);
      left = a;
      res = (f1(a)-f2(a))*(f1(b) - f2(b));
    }
  }
  else if ((f == '1' && g == '3') || (f == '3' && g == '1'))
  {
    // точки разрыва - -1; 0
    if (a < -1)
    {
      b = min(b, -1.001);
      right = b;
      res = (f1(a)-f3(a))*(f1(b) - f3(b));
    }
    else if (b > 0)
    {
      a = max(a, 0.001);
      left = a;
      res = (f1(a)-f3(a))*(f1(b) - f3(b));
    }
  }
  else if ((f == '2' && g == '3') || (f == '3' && g == '2'))
  {
    // точка разрыва - 0
    if (a < 0)
    {
      b = min(b, -0.001);
      right = b;
      res = (f2(a)-f3(a))*(f2(b) - f3(b));
    }
    else if (b > 0)
    {
      a = max(a, 0.001);
      left = a;
      res = (f2(a)-f3(a))*(f2(b) - f3(b));
    }
  }

  if (res <= 0)
  {
    return 1;
  }
  return 0;
}



void points(char func1, char func2, float a, float b, float eps, int flag)
{
  float temp;

  if (func1 != '1' && func1 != '2' && func1 != '3' && func2 != '1' && func2 != '2' && func2 != '3')
  {
    printf("Ошибка ввода\n");
    return;
  }

  if (!test_sec(func1, func2, a, b))
  {
    printf("На этом отрезке нет корня\n");
    return;
  }

  switch (func1)
  {
    case '1':
         switch (func2)
         {
           case '1':
                temp = root(f1, empty, df1, empty, d2f1, empty, left, right, eps);
                if (flag) printf("Корень уравнения f1 = 0 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f1 = 0 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           case '2':
                temp = root(f1, f2, df1, df2, d2f1, d2f2, left, right, eps);
                if (flag) printf("Корень уравнения f1 = f2 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f1 = f2 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           case '3':
                temp = root(f1, f3, df1, df3, d2f1, d2f3, left, right, eps);
                if (flag) printf("Корень уравнения f1 = f3 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f1 = f3 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           default:
                printf("Ошибка ввода\n");
                break;
         }
         break;
    case '2':
         switch (func2)
         {
           case '1':
                temp = root(f2, f1, df2, df1, d2f2, d2f1, left, right, eps);
                if (flag) printf("Корень уравнения f2 = f1 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f2 = f1 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           case '2':
                temp = root(f2, empty, df2, empty, d2f2, empty, left, right, eps);
                if (flag) printf("Корень уравнения f2 = 0 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f2 = 0 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           case '3':
                temp = root(f2, f3, df2, df3, d2f2, d2f3, left, right, eps);
                if (flag) printf("Корень уравнения f2 = f3 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f2 = f3 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                     a, b, eps, iter);
                break;
           default:
                printf("Ошибка ввода\n");
                break;
         }
         break;
    case '3':
         switch (func2)
         {
           case '1':
                temp = root(f3, f1, df3, df1, d2f3, d2f1, left, right, eps);
                if (flag) printf("Корень уравнения f3 = f1 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f3 = f1 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                            a, b, eps, iter);
                break;
           case '2':
                temp = root(f3, f2, df3, df2, d2f3, d2f2, left, right, eps);
                if (flag) printf("Корень уравнения f3 = f2 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f3 = f2 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                            a, b, eps, iter);
                break;
           case '3':
                temp = root(f3, empty, df3, empty, d2f3, empty, left, right, eps);
                if (flag) printf("Корень уравнения f3 = 0 на отрезке [%.3f;%.3f] с точностью %f: %f\n",
                          a, b, eps, temp);
                else printf("Число итераций при поиске корня уравнения f3 = 0 на отрезке [%.3f;%.3f] с точностью %f: %d\n",
                            a, b, eps, iter);
                break;
           default:
                printf("Ошибка ввода\n");
                break;
         }
         break;
    default:
         printf("Ошибка ввода\n");
         break;
  }
}



void test(char f, float a, float b, float eps)
{
  switch (f)
  {
    case '1':
         printf("Интеграл функции f1 на отрезке [%.3f;%.3f] с точностью %f: %f\n", a, b, eps, integral(f1, a, b, eps));
         break;
    case '2':
         printf("Интеграл функции f2 на отрезке [%.3f;%.3f] с точностью %f: %f\n", a, b, eps, integral(f2, a, b, eps));
         break;
    case '3':
         printf("Интеграл функции f3 на отрезке [%.3f;%.3f] с точностью %f: %f\n", a, b, eps, integral(f3, a, b, eps));
         break;
    default:
         printf("Ошибка ввода\n");
         break;
  }
}



void square(void)
{
  // точка пересечения f1 и f2
  float b12 = find_sec(f1, f2);  // вычисление границ отрезка
  float x12 = root(f1, f2, df1, df2, d2f1, d2f2, A, b12, EPS1);

  // точка пересечения f1 и f3
  float b13 = find_sec(f1, f3);  // вычисление границ отрезка
  float x13 = root(f1, f3, df1, df3, d2f1, d2f3, A, b13, EPS1);

  // точка пересечения f2 и f3
  float b23 = find_sec(f3, f2);  // вычисление границ отрезка
  float x23 = root(f3, f2, df3, df2, d2f3, d2f2, A, b23, EPS1);

  // вычисление площадей под графиками функций
  float sq1 = integral(f1, x13, x12, EPS2);  // площадь под графиком f1
  float sq2 = integral(f2, x23, x12, EPS2);  // площадь под графиком f2
  float sq3 = integral(f3, x13, x23, EPS2);  // площадь под графиком f3

  float s = sq1 - sq2 - sq3;  // итоговая площадь
  printf("Искомая площадь: %.3f\n", s);
}



float find_sec(pf f, pf g)
{
  float b = 1.0;  // начальное значение правой границы
  float left = f(A) - g(A);

  while (left*(f(b) - g(b)) > 0)
  {
    b += 0.5;
  }
  return b;
}



float root(pf f, pf g, pf df, pf dg, pf d2f, pf d2g, float a, float b, float eps1)
{
  float x0, xn, xn1;  // x0 - начальное значение итерационной последовательности
                      // xn - n значение итерационной последовательности
                      // xn1 - n+1 значение итерационной последовательности
  iter = 0;  // обнуляем счетчик итераций

  // выбираем начальную точку
  if ((f(a) - g(a))*(d2f(a) - d2g(a)) > 0)
  {
    x0 = a;
  }
  else
  {
    x0 = b;
  }

  // первое приближение
  xn = x0;
  xn1 = x0 - (f(x0) - g(x0)) / (df(x0) - dg(x0));

  while (fabs(xn - xn1) >= eps1)
  {
    xn = xn1;
    xn1 = xn - (f(xn) - g(xn)) / (df(xn) - dg(xn));
    iter++;
  }
  return xn1;
}



float integral(pf f, float a, float b, float eps2)
{
  float n = 10.0;  // начальное разбиение отрезка
  float In = 0.0, I2n = 0.0;  // приближенное значение интеграла на отрезках разбиения n и 2n
  float h = (b - a) / n;  // длина отрезка разбиения

  // нахождение приближенного значения интеграла для начального разбиения
  for (float i = 0.0; i < n; i++)
  {
    float Fi = f(a + (i + 0.5) * h);
    I2n += h * Fi;
  }

  // пока не нашли нужного разбиения
  while (fabs(In - I2n) / 3.0 >= eps2)
  {
    In = I2n;
    I2n = 0.0;
    h = (b - a) / n;

    // нахождение приближенного значения интеграла
    for (float i = 0.0; i < n; i++)
    {
      float Fi = f(a + (i + 0.5) * h);
      I2n += h * Fi;
    }

    n *= 2.0;
  }

  return I2n;
}
