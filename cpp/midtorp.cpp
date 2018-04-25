#include<iostream>
#include<string>
using namespace std;
template<class T> class customStack {
private:
	T *arr;
	int top, size;

public:
	customStack(int tsize = 128) {
		size = tsize;
		arr = new T[tsize];
		top = 0;
	}
	bool push(T tar) {
		if (top == size) {
			return false;
		}
		arr[top] = tar;
		top++;
		return true;
	}
	T getTop() {
		if (top == 0) {
			return false;
		}
		return arr[top - 1];
	}
	bool pop(T& tar) {
		if (top == 0) {
			return false;
		}
		top--;
		tar = arr[top];
		return true;
	}
	int getSize() {
		return size;
	}
	int getLength() {
		return top;
	}
	virtual ~customStack() {
		delete arr;
	}
};
class doubleStack: public customStack<double> {
public:
	doubleStack(int size) :	customStack(size) {
	}
};
class charStack: public customStack<char> {
public:
	charStack(int size) :	customStack(size) {
	}
};
double tenpow(int x) {
	double tmp = 1;
	for (int i = 0; i < x; i++)
		tmp *= 10.0;
	return tmp;
}
double dotpow(int x) {
	double tmp;
	for (int i = 0; i < x; i++)
		tmp / 10.0;
	return tmp;
}
double caculate(double a, double b, char op) {
	if (op == '+')
		return a + b;
	if (op == '-')
		return a - b;
	if (op == '*')
		return a * b;
	if (op == '/')
		return a / b;
}

int main() {
	char tmpchar, op;
	string tmpstring;
	int stacksize = 64;
	double result, tmp;
	charStack opstack(stacksize);
	doubleStack numstack(stacksize);
	double a, b;
	getline(cin,tmpstring);
	for (int i = 0; i < tmpstring.size();) {
		cout<<"enter"<<tmpstring[i]<<endl;
		if (tmpstring[i] > '0' - 1 && tmpstring[i] < '9' + 1) {
			for (tmp = tmpstring[i] - '0';
					tmpstring[++i] > '0' - 1 && tmpstring[i] < '9' + 1;) {
				tmp *= 10.0;
				tmp += tmpstring[i] - '0';
			}
			numstack.push(tmp);
			if (tmpstring[i + 1] != '.')
				cout << numstack.getTop() <<' ';
		}
		if (tmpstring[i] == '.') {
			numstack.pop(tmp);
			for (int j = 1; tmpstring[++i] > '0' - 1 && tmpstring[i] < '9' + 1;
					j++) {
				tmp += (tmpstring[i] - '0') * dotpow(j);
			}
			numstack.push(tmp);
			cout << numstack.getTop()<< ' ';
		}
		if (tmpstring[i] == '(') {
			opstack.push('(');
			i++;
		}
		if (tmpstring[i] == ')') {
			while (opstack.getTop() != '(') {
				numstack.pop(a);
				numstack.pop(b);
				opstack.pop(op);
				numstack.push(caculate(a, b, op));
				cout << ' ' << op;
			}
			i++;
		}
		if (tmpstring[i] == '+') {
			while (opstack.getTop() == '*' || opstack.getTop() == '/') {
				numstack.pop(a);
				numstack.pop(b);
				opstack.pop(op);
				numstack.push(caculate(a, b, op));
				cout << ' ' << op;
			}
			opstack.push('+');
			i++;
		}
		if (tmpstring[i] == '-') {
			while (opstack.getTop() == '*' || opstack.getTop() == '/') {
				numstack.pop(a);
				numstack.pop(b);
				opstack.pop(op);
				numstack.push(caculate(a, b, op));
				cout << ' ' << op;
			}
			opstack.push('-');
			i++;
		}
		if (tmpstring[i] == '*') {
			opstack.push('*');
			i++;
		}
		if (tmpstring[i] == '/') {
			opstack.push('/');
			i++;
		}
	}
	while (opstack.getLength() != 0) {
		numstack.pop(a);
		numstack.pop(b);
		opstack.pop(op);
		numstack.push(caculate(a, b, op));
		cout << ' ' << op;
	}
	cout<< ' =';
	cout << numstack.getTop();
	return 0;
}
