#include <iostream>
#include <vector>
#include <math.h> 

int INFINUM = -1000000001;

// Этот класс описывает одну вершину биномиального дерева (биномиальной кучи)
class Node {
private:
	// В вершине есть 5 полей:
	//   1) parent   указатель на родителя, None в случае, если вершина корень
	//   2) value    хранимое значение
	//   3) degree   степень вершины = количество детей
	//   4) child    указатель на самого левого ребенка
	//   5) sibling  если вершина корень, то это указатель на следующий корень в куче
	//               если вершина не корень, то указатель на следуюшего ребенка у родителя вершины
	Node* _parent;
	Node* _child;
	Node* _sibling;
	int _value;
	int _degree;
public:
	// Конструктор
	Node(int elem) {
		_parent = NULL;
		_child = NULL;
		_sibling = NULL;
		_value = elem;
		_degree = 0;
	}

	// Деструктор
	// Используем рекурсивное удаление вершин:
	//   Сначала удаляем всех детей, потом всех сиблингов и только потом саму вершину
	~Node() {
		delete _child;
		delete _sibling;
	}

	// Этот метод объединяет два биномиальных дерева в одно,
	// приписывая к текущей вершине второй аргумент, в качестве самого левого ребенка
	void binomial_link(Node* other) {
		other->parent(this);
		other->sibling(this->child());
		this->child(other);
		_degree += 1;
	}

	// Это статический метод, рекурсивно обходящий биномиальное дерево
	// Результат записывается в список со списками ans[0..2**(степень корня)][]
	// Цель в том, чтобы в кажом из списков ans[i] был список всех вершин на i-ом уровне
	static void recursive_iterate(std::vector<std::vector<int> >& ans, Node* current_node, int depth) {
		if (current_node == NULL) {
			return;
		}
		// Если у вершины нет детей, то и сиблингов тоже
		if (current_node->child() == NULL) {
			ans[depth].push_back(current_node->value());
			return;
		}
		// Сначала перебирем всех детей
		Node::recursive_iterate(ans, current_node->_child, depth + 1);
		ans[depth].push_back(current_node->_value);
		// Потом сиблингов, если текущая вершина не корень
		if (current_node->parent() != NULL) {
			Node::recursive_iterate(ans, current_node->_sibling, depth);
		}
	}

	///////////////////GET/SET///////////////////
	const int value() const { return _value; }
	void value(const int value) { _value = value; }

	const int degree() const { return _degree; }
	void degree(const int degree) { _degree = degree; }

	Node* parent() const { return _parent; }
	void parent(Node* parent) { _parent = parent; }

	Node* child() const { return _child; }
	void child(Node* child) { _child = child; }

	Node* sibling() const { return _sibling; }
	void sibling(Node* sibling) { _sibling = sibling; }
	/////////////////////////////////////////////
};

// В этом классе хранится сама структура биномиальной кучи
// Напоминаю, что структура данных --- это данные и операции
class BinomialHeap {
private:
	// единственный атрибут --- приватный указатель на корневой список
	Node* head;
public:
	// Конструктор дает две возможности:
	//   1) Создать пустую кучу
	//   2) Создать кучу из одного элемента
	BinomialHeap(int elem = INFINUM) {
		head = NULL;
		if (elem > INFINUM) {
			head = new Node(elem);
		}
	}

	// Удаление структуры происходит рекурсивно, благодаря деструктору класса Node
	~BinomialHeap() {
		delete head;
	}

	// Метод считает размер кучи, перобегая корневой список
	int size() {
		Node* current = head;
		int ans = 0;
		while (current != NULL) {
			ans += (int)pow(2, current->degree());
			current = current->sibling();
		}
		return ans;
	}

	// Этот метод последовательно выводит на экран все биномиальные деревья, по уровням
	// Например:
	// Размер кучи: 8
	// Дерево степени 3
	// 1
	// 1 3 2
	// 12 25 10
	// 18
	// Этот вывод отвечает такому дереву:
	//           1
	//    1   3  2
	// 12 25  10
	// 18
	//
	void print() {
		std::cout << "Heap size: " << size() << std::endl;
		// перебираем все деревья, с помощью корневого списка
		Node* current = head;
		while (current != NULL) {
			std::cout << "Tree of degree " << current->degree() << std::endl;
			// создадим двумерный список, в который будем
			// по уровням записывать хранимые значения дерева
			std::vector<std::vector<int> > ans(current->degree() + 1);

			// Рекурсивный обход дерева
			Node::recursive_iterate(ans, current, 0);

			// Собственно вывод
			for (unsigned i = 0; i < ans.size(); ++i) {
				for (unsigned j = 0; j < ans[i].size(); ++j) {
					std::cout << ans[i][j] << ' ';
				}
				std::cout << std::endl;
			}
			current = current->sibling();
		}
	}

	// Этот метод объединяет два корневых списка в один
	// При этом ответ будет хранится в первом списке (self),
	// а второй список исчезает
	void merge_root_lists(BinomialHeap* other) {
		if (other->size() == 0) {
			return;
		}
		if (size() == 0) {
			head = other->head;
			other->head = NULL;
			return;
		}

		// heap1 и heap2 - указатели на рассматриваемый элемент в 1 и 2 списках соответственно
		Node* heap1 = head;
		Node* heap2 = other->head;

		// Первый список должен быть корректно определен
		if (heap1->degree() <= heap2->degree()) {
			head = heap1;
			heap1 = heap1->sibling();
		}
		else {
			head = heap2;
			heap2 = heap2->sibling();
		}

		// Второй список должен оказаться пуст
		other->head = NULL;

		// Собственно прцесс слияния:
		//   пока оба списка не пусты
		//       если степень текущего элемента первого списка меньше(=) степени текущего эл-та второго,
		//           то добавим эл-т первого списка к ответу и сдвинем указатель
		//       иначе
		//           аналогично, но со вторым списком
		//       не забудем сдвинуть указатель на списке с ответом
		//
		Node* current_node = head;
		while (heap1 != NULL && heap2 != NULL) {
			if (heap1->degree() <= heap2->degree()) {
				current_node->sibling(heap1);
				heap1 = heap1->sibling();
			}
			else {
				current_node->sibling(heap2);
				heap2 = heap2->sibling();
			}
			current_node = current_node->sibling();
		}
		// Добавим оставшийся "хвост"
		if (heap1 != NULL) {
			current_node->sibling(heap1);
		}
		if (heap2 != NULL) {
			current_node->sibling(heap2);
		}
		//print_root_list();
	}

	// метод выводи на экран корневой список
	void print_root_list() {
		Node* current = head;
		while (current != NULL) {
			std::cout << current->value() << ' ';
			current = current->sibling();
		}
		std::cout << std::endl;
	}

	// Метод объединет две кучи, ответ будет хранится в первой, а вторая (other) обнулится
	void add(BinomialHeap* other) {
		// 1. Объединим корневые списки
		merge_root_lists(other);
		// 2. Если куча пустая, то выходим
		if (head == NULL) {
			return;
		}
		// В процессе слияния куч будем отслеживать три вершины:
		//   1) текущую (current)
		//   2) предыдущую (previous)
		//   3) Следующую (next)
		//
		Node* current = head;
		Node* previous = NULL;
		Node* next = head->sibling();

		// Цикл будет работать до тех пор, пока текущая вершина
		// не станет последней в корневом списке
		while (next != NULL) {
			// Здесь рассматриваются 2 случая:
			//   1) Если степени текущей и следующей вершины различны,
			//       то ничего делать не нужно, просто сдвинемся по корневому списку
			//   2) Если подряд идут три дерева одинакового размера (больше 3 быть не может)
			//       В этом случае самое левое из одинаковых деревьев должно остаться на месте,
			//       а оставшиеся 2 нужно объединить
			//       Поэтому сдвиенмся по корневому списку
			//
			if ((current->degree() != next->degree()) ||
				((next->sibling() != NULL) && (current->degree() == next->sibling()->degree()))) {
				previous = current;
				current = next;
			}
			else {
				// Это случай, когда степени текущей и следующих вершин одинаковые
				//   (причем степень next.sibling отлична, т.е. НЕ 3 одинаковых дерева)
				//
				// Значение в текущей вершине меньше или равно,
				// а значит припишем next к current в качестве самого левого ребенка
				//
				if (current->value() <= next->value()) {
					current->sibling(next->sibling());
					current->binomial_link(next);
				}
				else {
					// Значение в текущей вершине больше,
					// а значит припишем current к next в качестве самого левого ребенка
					//
					// Здесь возникает необходимость правильным образом переписать
					// поле sibling у предыдущей вершины
					//
					if (previous == NULL) {
						head = next;
					}
					else {
						previous->sibling(next);
					}
					// подвесим current к next
					next->binomial_link(current);
					current = next;
				}
			}
			// на всякий случай обновим значение переменной next
			next = current->sibling();
		}
	}
	// Этот метод должен возвращать вершину с минимальным значением
	// Для этого нужно перебрать корневой список и найти манимум в нем
	Node* minimum() {
		Node* ans = head;
		///////////////////////////////
		///////////////////////////////
		///////ДОПИШИТЕ КОД////////////
		///////////////////////////////
		///////////////////////////////
		return ans;
	}

	// Этот метод должен добавлять к биномиальной куче вершину со значением elem
	void insert(int elem) {
		BinomialHeap* new_heap = new BinomialHeap(elem);
		this->add(new_heap);
		return;
	}

	// Этот метод должен изъять из кучи вершину с минимальным ключом(значением)
	Node* extract_min() {
		if (head == NULL) {
			return NULL;
		}
		// Найдём вершину с минимальным значением
		// На самом деле здесь нужно "изъять" всё соответстыующее биномиальне дерево,
		// т.е.чтобы корневой список не включал в себя минимальную вершину
		Node* ans = head;
		Node* prev_ans = NULL;
		Node* current = head;
		Node* previous = NULL;
		while (current != NULL) {
			if (current->value() < ans->value()) {
				ans = current;
				prev_ans = previous;
			}
			previous = current;
			current = current->sibling();
		}
		if (prev_ans == NULL) {
			head = ans->sibling();
		}
		else {
			//std::cout << ans->value() << ' ' << prev_ans->value() << ' ';
			prev_ans->sibling(ans->sibling()); 
		}
		//this->print();
		// Моздадим пустую кучу
		BinomialHeap* new_heap = new BinomialHeap();
		// Здесь должен быть код, который обращает порядок в списке детей вершины ans
		//   Затем надо поместить в new_heap.head начало получившегося списка
		//
		//   Тогда в new_heap будет хранится биномиальная куча из детей минимальной вершины исходной кучи
		//
		current = ans->child();
		ans->child(NULL);
		ans->sibling(NULL);
		Node* next = NULL;
		previous = NULL;
		while (current != NULL) {
			//std::cout << current->value() << ' ';
			next = current->sibling();
			current->parent(NULL);
			if (previous == NULL) {
				previous = current;
				current->sibling(NULL);
				current = next;
				continue;
			}
			current->sibling(previous);
			previous = current;
			current = next;
		}
		new_heap->head = previous;
		//new_heap->print();
		//this->print();
		this->add(new_heap);
		//this->print();
		return ans;
	}

	// Этот метод уменьшает значение переданной вершины, присваивая новое значение
	// В этом месте используется sift_up, аналогичный бинарной куче
	//
	void decrease_key(Node* node, int new_value) {
		///////////////////////////////
		///////////////////////////////
		///////ДОПИШИТЕ КОД////////////
		///////////////////////////////
		///////////////////////////////
		return;
	}


	// Этот метод удаляет переданную вершину из кучи,
	//   предполагается, что все значения в куче не отирцательны
	//   т.е. -1 - это "минус бесконечность"
	void delete_elem(Node* node) {
		this->decrease_key(node, INFINUM);
		this->extract_min();
		return;
	}


};


int main()
{
	\\ ЗАДАНИЯ:
	\\ 1) Допишите этот код	
	\\ 2) Перепишите код так, чтобы он отвечал требованию инкапсуляции (что это? почему сейчас инуапсуляция не выполнена в полной мере?)
}
