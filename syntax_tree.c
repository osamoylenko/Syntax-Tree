#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <assert.h>

typedef struct elem_ {
	int type; // -1 - ошибка, 0 - число, 1 +, 2 -, 3 *, 4 /
	int num;
} elem;

// узел синтаксического дерева
typedef struct node_ {
	elem el;
	struct node_* left; // левый операнд
	struct node_* right; // правый операнд
} node;

// текущий символ
char c;
void get_char() {
	do {
		scanf("%c", &c);
	} while (c == ' ');
}

int get_num() {
	int res = 0;
	do {
		res *= 10;
		res += c - '0';
		scanf("%c", &c);
	} while (c >= '0' && c <= '9');
	if (c == ' ')
		get_char();
	return res;
}

elem create_elem(int type, int num) {
	elem el;
	el.type = type;
	el.num = num;
	return el;
}

node* create_node(elem el, node* left, node* right) {
	node* res;
	res = (node*)malloc(sizeof(node));
	res->el = el;
	res->left = left;
	res->right = right;
	return res;
}

// взаимнорекурсивные функции sum, term, factor для синтаксического разбора
// sum -> term
// sum -> term+sum
// sum -> term-sum
//
// term -> factor
// term -> factor*term
// term -> factor/term
//
// factor -> (sum)
// factor -> [+-]число

node* term();
node* factor();

// сумма
node* sum() {
	node *op1, *op2;

	op1 = term();
	while (c == '+' || c == '-') {
		if (c == '+') {
			get_char();
			op2 = term();
			op1 = create_node(create_elem(1, 0), op1, op2);
		}
		else {
			get_char();
			op2 = term();
			op1 = create_node(create_elem(2, 0), op1, op2);
		}
	}
	return op1;
}

// слагаемое
node* term() {
	node *op1, *op2;

	op1 = factor();
	while (c == '*' || c == '/') {
		if (c == '*') {
			get_char();
			op2 = factor();
			op1 = create_node(create_elem(3, 0), op1, op2);
		}
		else {
			get_char();
			op2 = factor();
			op1 = create_node(create_elem(4, 0), op1, op2);
		}
	}
	return op1;
}

// множитель
node* factor() {
	node *op1 = 0;
	int s = 1;
	if (c == '(') {
		get_char();
		op1 = sum();
		get_char(); // закрывающая скобка
	}
	else {
		// знак числа
		if (c == '-' || c == '+') {
			s = (c == '+') ? 1 : -1;
			get_char();
		}

		if (c >= '0' && c <= '9')
			op1 = create_node(create_elem(0, s * get_num()), 0, 0);
	}
	assert(op1 != 0); // если 0, значит в выражении ошибка
	return op1;
}

// вычисление выражения
double calc_tree(node* cur) {
	assert(cur != 0);

	// если число, его возвращаем
	if (cur->el.type == 0)
		return cur->el.num;

	if (cur->el.type == 1) 	// +
		return calc_tree(cur->left) + calc_tree(cur->right);

	if (cur->el.type == 2)	// -
		return calc_tree(cur->left) - calc_tree(cur->right);

	if (cur->el.type == 3)	// *
		return calc_tree(cur->left) * calc_tree(cur->right);

	if (cur->el.type == 4)	// /
		return calc_tree(cur->left) / calc_tree(cur->right);

	return 0;
}

void print_elem(elem el) {
	if (el.type == 0)
		printf(" %d ", el.num);
	else if (el.type == 1)
		printf(" + ");
	else if (el.type == 2)
		printf(" - ");
	else if (el.type == 3)
		printf(" * ");
	else if (el.type == 4)
		printf(" / ");
}

// префиксный обход дерева -- прямая польская нотация
void prefix_tree(node* cur) {
	if (cur == 0)
		return;
	print_elem(cur->el);
	prefix_tree(cur->left);
	prefix_tree(cur->right);
}

// постфиксный обход дерева -- обратная польская нотация
void postfix_tree(node* cur) {
	if (cur == 0)
		return;
	postfix_tree(cur->left);
	postfix_tree(cur->right);
	print_elem(cur->el);
}

void free_tree(node* cur) {
	if (cur == 0)
		return;
	free_tree(cur->left);
	free_tree(cur->right);
	free(cur);
}


int main() {
	node* st;
	
	setlocale(0, "rus");

	printf("Введите выражение для разбора:\n");
	get_char();
	st = sum();
	printf("\n");

	printf("Префиксная нотация:\n");
	prefix_tree(st);
	printf("\n\n");

	printf("Постфиксная нотация:\n");
	postfix_tree(st);
	printf("\n\n");

	printf("Значение выражения:\n");
	printf("%lf", calc_tree(st));
	printf("\n\n");

	free_tree(st);

	system("pause");

	return 0;
}
