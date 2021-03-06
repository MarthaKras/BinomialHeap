# Этот класс описывает одну вершину биномиального дерева (биномиальной кучи)
class Node:
    # В вершине есть 5 полей:
    #   1) parent   указатель на родителя, None в случае, если вершина корень
    #   2) value    хранимое значение
    #   3) degree   степень вершины = количество детей
    #   4) child    указатель на самого левого ребенка
    #   5) sibling  если вершина корень, то это указатель на следующий корень в куче
    #               если вершина не корень, то указатель на следуюшего ребенка у родителя вершины
    def __init__(self, elem):
        self.parent = None
        self.value = elem
        self.degree = 0
        self.child = None
        self.sibling = None

    # Используем рекурсивное удаление вершин:
    #   Сначала удаляем всех детей, потом всех сиблингов и только потом саму вершину
    def __del__(self):
        del self.child
        del self.sibling

    # Этот метод объединяет два биномиальных дерева в одно,
    # приписывая к текущей вершине второй аргумент, в качестве самого левого ребенка
    def binomial_link(self, other):
        other.parent = self
        other.sibling = self.child
        self.child = other
        self.degree += 1

    # Это статический метод, рекурсивно обходящий биномиальное дерево
    # Результат записывается в список со списками ans[0..2**(степень корня)][]
    # Цель в том, чтобы в кажом из списков ans[i] был список всех вершин на i-ом уровне
    @staticmethod
    def recursive_iterate(ans, current_node, depth):
        if current_node is None:
            return
        # Если у вершины нет детей, то и сиблингов тоже
        if current_node.child is None:
            ans[depth].append(current_node.value)
            return
        # Сначала перебирем всех детей
        Node.recursive_iterate(ans, current_node.child, depth + 1)
        ans[depth].append(current_node.value)
        # Потом сиблингов, если текущая вершина не корень
        if current_node.parent is not None:
            Node.recursive_iterate(ans, current_node.sibling, depth)


# В этом классе хранится сама структура биномиальной кучи
# Напоминаю, что структура данных - это данные и операции
class BinomialHeap:
    # Конструктор дает две возможности:
    #   1) Создать пустую кучу
    #   2) Создать кучу из одного элемента
    def __init__(self, elem=None):
        self.head = None
        if elem is not None:
            self.head = Node(elem)

    # Удаление структуры происходит рекурсивно, благодаря деструктору класса Node
    def __del__(self):
        del self.head

    # Метод считает размер кучи, перобегая корневой список
    def __len__(self):
        current = self.head
        ans = 0
        while current is not None:
            ans += 2 ** current.degree
            current = current.sibling
        return ans

    # Этот метод последовательно выводит на экран все биномиальные деревья, по уровням
    # Например:
    # Размер кучи: 8
    # Дерево степени 3
    # 1
    # 1 3 2
    # 12 25 10
    # 18
    # Этот вывод отвечает такому дереву:
    #           1
    #    1   3  2
    # 12 25  10
    # 18
    #
    def print(self):
        print('Размер кучи:', len(self))
        # перебираем все деревья, с помощью корневого списка
        current = self.head
        while current is not None:
            print('Дерево степени', current.degree)
            # создадим двумерный список, в который будем
            # по уровням записывать хранимые значения дерева
            tree_lines = []
            for i in range(current.degree + 1):
                tree_lines.append(list())

            # Рекурсивный обход дерева
            Node.recursive_iterate(tree_lines, current, 0)

            # Собственно вывод
            for lines in tree_lines:
                print(*lines)

            current = current.sibling

    # Этот метод объединяет два корневых списка в один
    # При этом ответ будет хранится в первом списке (self),
    # а второй список исчезает
    def merge_root_lists(self, other):
        if len(other) == 0:
            return
        if len(self) == 0:
            self.head = other.head
            other.head = None
            return

        # heap1 и heap2 - указатели на рассматриваемый элемент в 1 и 2 списках соответственно
        heap1 = self.head
        heap2 = other.head

        # Первый список должен быть корректно определен
        if heap1.degree <= heap2.degree:
            self.head = heap1
            heap1 = heap1.sibling
        else:
            self.head = heap2
            heap2 = heap2.sibling

        # Второй список должен оказаться пуст
        other.head = None

        # Собственно прцесс слияния:
        #   пока оба списка не пусты
        #       если степень текущего элемента первого списка меньше(=) степени текущего эл-та второго,
        #           то добавим эл-т первого списка к ответу и сдвинем указатель
        #       иначе
        #           аналогично, но со вторым списком
        #       не забудем сдвинуть указатель на списке с ответом
        #
        current_node = self.head
        while heap1 is not None and heap2 is not None:
            if heap1.degree <= heap2.degree:
                current_node.sibling = heap1
                heap1 = heap1.sibling
            else:
                current_node.sibling = heap2
                heap2 = heap2.sibling
            current_node = current_node.sibling

        # Добавим оставшийся "хвост"
        if heap1 is not None:
            current_node.sibling = heap1
        if heap2 is not None:
            current_node.sibling = heap2

    # метод выводи на экран корневой список
    def print_root_list(self):
        current = self.head
        while current is not None:
            print(current.value, end=' ')
            current = current.sibling
        print()

    # Метод объединет две кучи, ответ будет хранится в первой (self), а вторая (other) обнулится
    def __add__(self, other):
        # 1. Объединим корневые списки
        self.merge_root_lists(other)
        # 2. Если куча пустая, то выходим
        if self.head is None:
            return

        # В процессе слияния куч будем отслеживать три вершины:
        #   1) текущую (current)
        #   2) предыдущую (previous)
        #   3) Следующую (next)
        #
        current = self.head
        previous = None
        next = self.head.sibling

        # Цикл будет работать до тех пор, пока текущая вершина
        # не станет последней в корневом списке
        while next is not None:
            # Здесь рассматриваются 2 случая:
            #   1) Если степени текущей и следующей вершины различны,
            #       то ничего делать не нужно, просто сдвинемся по корневому списку
            #   2) Если подряд идут три дерева одинакового размера (больше 3 быть не может)
            #       В этом случае самое левое из одинаковых деревьев должно остаться на месте,
            #       а оставшиеся 2 нужно объединить
            #       Поэтому сдвиенмся по корневому списку
            #
            if (current.degree != next.degree) or \
                    ((next.sibling is not None) and
                     (current.degree == next.sibling.degree)):
                previous = current
                current = next
            # Это случай, когда степени текущей и следующих вершин одинаковые
            #   (причем степень next.sibling отлична, т.е. НЕ 3 одинаковых дерева)
            #
            else:
                # Значение в текущей вершине меньше или равно,
                # а значит припишем next к current в качестве самого левого ребенка
                #
                if current.value <= next.value:
                    current.sibling = next.sibling
                    current.binomial_link(next)
                else:
                    # Значение в текущей вершине больше,
                    # а значит припишем current к next в качестве самого левого ребенка
                    #
                    # Здесь возникает необходимость правильным образом переписать
                    # поле sibling у предыдущей вершины
                    #
                    if previous is None:
                        self.head = next
                    else:
                        previous.sibling = next
                    # подвесим current к next
                    next.binomial_link(current)
                    current = next
            # на всякий случай обновим значение переменной next
            next = current.sibling
        return self

    # Этот метод должен возвращать вершину с минимальным значением
    # Для этого нужно перебрать корневой список и найти манимум в нем
    def minimum(self):
        ans = self.head
        ############################################################################################################
        ############################################################################################################
        ###     ЗДЕСЬ МОГЛА БЫ БЫТЬ ВАША РЕКЛАМА
        ############################################################################################################
        ############################################################################################################
        return ans

    # Этот метод должен добавлять к биномиальной куче вершину со значением elem
    def insert(self, elem):
        ############################################################################################################
        ############################################################################################################
        ###     ЗДЕСЬ МОГЛА БЫ БЫТЬ ВАША РЕКЛАМА
        ############################################################################################################
        ############################################################################################################
        return

    # Этот метод должен изъять из кучи вершину с минимальным ключом(значением)
    def extract_min(self):
        # Найдём вершину с минимальным значением
        # На самом деле здесь нужно "изъять" всё соответстыующее биномиальне дерево,
        # т.е. чтобы корневой список не включал в себя минимальную вершину

        ans = self.minimum()
        # Моздадим пустую кучу
        new_heap = BinomialHeap()
        # Здесь должен быть код, который обращает порядок в списке детей вершины ans
        #   Затем надо поместить в new_heap.head начало получившегося списка
        #
        #   Тогда в new_heap будет хранится биномиальная куча из детей минимальной вершины исходной кучи
        #
        ############################################################################################################
        ###     ЗДЕСЬ МОГЛА БЫ БЫТЬ ВАША РЕКЛАМА
        ############################################################################################################
        self += new_heap
        return ans

    # Этот метод уменьшает значение переданной вершины, присваивая новое значение
    # В этом месте используется sift_up, аналогичный бинарной куче
    #
    def decrease_key(self, node, new_value):
        ############################################################################################################
        ############################################################################################################
        ###     ЗДЕСЬ МОГЛА БЫ БЫТЬ ВАША РЕКЛАМА
        ############################################################################################################
        ############################################################################################################
        return

    # Этот метод удаляет переданную вершину из кучи,
    #   предполагается, что все значения в куче не отирцательны
    #   т.е. -1 - это "минус бесконечность"
    def delete(self, node):
        self.decrease_key(node, -1)
        self.extract_min()
        return


bh = BinomialHeap()
bh.print()
for i in range(100):
    #bh.insert(i)
    bh += BinomialHeap(i)
bh.print()
