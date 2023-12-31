**Задача №0. Темы: строки, структуры, файлы, вещественные числа**

Реализовать два типа данных:

1) Строки - структура, содержащая указатель на null-терминированную строку в динамической памяти, длину строки и ёмкость памяти. Определить для этого типа все операции общего вида, необходимые для решения задачи, аналогичные операциям string.h. Строка должна быть динамически расширяемой с эффективной реаллокацией памяти (увеличение ёмкости в два раза).
1) Динамический массив строк - структура, содержащая указатель на массив строк (объявленных в предыдущем пункте) в динамической памяти, длину и ёмкость. Также определить все необходимые операции общего вида и реализовать эффективную реаллокацию.

С помощью этих типов данных выполнить следующую задачу:

В файле записан текст произвольной длины, состоящий из слов, разделённых знаками препинания и разделителями. *Разделители* - пробел и перевод строки. *Слова* состоят из непрерывных последовательностей латинских букв (заглавных и строчных) и цифр, и могут начинаться с символов ‘+’ или ‘-’. Все остальные символы текста считаются *знаками препинания*. Если после символа ‘+’ или ‘-’ идёт не латинская буква и не цифра, этот символ считается знаком препинания.

Программа должна прочитать текст и сохранить его в динамическом массиве строк, где каждый элемент является либо словом текста, либо разделителем, либо знаком препинания.

Затем необходимо модифицировать текст по следующим правилам:

1) Слово является записью температуры в Фаренгейтах, если оно удовлетворяет следующему формату: (+/-)0-9[0-9]\*tF и не превышает 10 символов в длину. Все слова, являющиеся записью температуры в Фаренгейтах, должны быть переведены в запись в Цельсиях с точностью до одного знака после запятой (формат отличается конечным символом C). Например:
   1) +10tF => -12,2tC

1) Все слова, состоящие только из латинских букв, являющиеся палиндромами, должны быть заменены на слово PALINDROM. Например:
   1) aaa abc bbb a1a => PALINDROM abc PALINDROM a1a

1) Пробел является *лишним*, если идёт после другого разделителя или в начале текста. Перевод строки является *лишним*, если идёт после двух других переводов строки или в начале текста. Из текста надо убрать все лишние разделители. Например:
   1) aaa   bbb ccc => aaa bbb ccc

1) Знаки препинания круглые скобки, стоящие в правильной последовательности вокруг одного слова (возможно с разделителями), должны быть убраны. Например:
   1) (aaa) ( bbb ) (aaa bbb) => aaa  bbb  (aaa bbb)

1) Математические выражения, состоящие из знака препинания из множества {+, -, \*, /} такие, что слева и справа (возможно с разделителями) от них стоят слова, состоящие только из цифр, длиной не больше 9 символов, должны быть вычислены (в типе int) и заменены на результат соответствующей операции (либо на слово ERROR, если операция делит на ноль). Например:
   1) 1 + 2 => 3
   1) 1 plus 2 => 1 plus 2
   1) 1 / 0 => ERROR

Правила должны применяться к тексту *до неподвижной точки*, то есть текст должен быть сведён к такому состоянию, в котором ни одно правило не может быть применено. Если результат зависит от последовательности применения правил или от последовательности применения их к словам, такой текст считается некорректным. Например, текст “2 + 2 \* 2” является некорректным, так как в зависимости от порядка применения сводится либо к тексту “6” либо “8”, а текст “(2 + 2) \* 2” является корректным, так как вне зависимости от порядка применения сводится только к тексту “8”. Определения некорректности текста делать **не нужно**, таких текстов просто не будет в тестах.

После преобразования текста его нужно будет вывести в файл.

Имена файлов ввода и вывода задаются аргументами командной строки.


**Задача №1. Темы: макросы**

Модифицировать задачу №0, убрав дублирование кода между типами данных строки и динамического массива строк с помощью function-like макросов, генерирующих код, необходимый для этих типов данных.

Все остальные условия совпадают с задачей №0.

**Задача №2. Темы: аллокатор**

Реализовать аллокатор - собственный механизм выделения и освобождения динамической памяти, аналогичный стандартным функциям malloc, realloc, calloc и free, работающий следующим образом:

При создании аллокатор выделяет в динамическом классе памяти большой объём заданного размера (MAX_SIZE)
Каждая функция выделения памяти (аналоги malloc, realloc, calloc) возвращает либо уникальный указатель на участок памяти внутри этого объёма, либо NULL, если свободной памяти внутри объёма нет
Функция освобождения памяти (аналог free) может сделать участок памяти переиспользованным

Для того, чтобы протестировать созданный вами аллокатор, нужно модифицировать задачу №0, заменив в ней все вызовы malloc, calloc, realloc и free на ваш аллокатор, и пройти тесты, используя MAX_SIZE равный 100MB.