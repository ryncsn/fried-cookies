#include<iostream>
#include<string>
using namespace std;
double pow(double base,int exp){
	double tmp=base;
	while(--exp>0)
		tmp*=base;
	return tmp;
}
template<class T>
class stack{
private:
	T *data;
	int size,length;
public:
	stack(int size=128){
		this->size=size;
		length=0;
		data=new T[128];
	}
	bool push(T tar)
	{
		if(length==size)
			return false;
		data[length]=tar;
		length++;
		return true;
	}
	bool pop(T& tar)
	{
		if(length==0)
			return false;
		length--;
		tar=data[length];
		return true;
	}
	T getTop(){
		return data[length];
	}
	virtual ~stack(){
		delete data;
	}
	bool isEmpty(){
		return length==0;
	}
};
class opStack : public stack <char>{
public:
	opStack(int size):stack(size){}
};
class numStack : public stack <double>{
public:
	numStack(int size):stack(size){}
};
double caculate(double a,double b,char op){
	if(op=='+')
		return a+b;
	if(op=='-')
		return a-b;
	if(op=='*')
		return a*b;
	if(op=='/')
		return a/b;
	if(op=='^')
		return pow(a,b);
}
int pri(char a){
	if(a=='^')
		return 2;
	if(a=='*'||a=='/')
		return 1;
	if(a=='+'||a=='-')
		return 0;
}
double dot(double num,int pos){
	while(pos>0)
		num*=0.1;
	return num;
}
class expression {
private:
	string data;
	int pos;
public:
	expression(string tar){
		data=tar;
		pos=0;
	}
	bool isEmpty(){
		if(pos>=data.length())
			return true;
		return false;
	}
	bool getNextNum(double& tar){
		if(data[pos]<'0'||data[pos]>'9')
			return false;
		double tmp;
		for(tmp=0;data[pos]>'0'-1&&data[pos]<'9'+1;pos++)
		{
			tmp*=10;
			tmp+=data[pos]-'0';
		}
		if(data[pos]=='.')
		{
			int dep=0;
			while(data[++pos]>'0'-1&&data[pos]<'9'+1)
				tmp+=dot(data[pos]-'0',++dep);
		}
		tar=tmp;
		return true;
	}
	bool getNextOp(char& tar){
		if(	data[pos]=='*'||
			data[pos]=='/'||
			data[pos]=='-'||
			data[pos]=='+'||
			data[pos]=='^')
		{
			tar=data[pos];
			pos++;
			return true;
		}
		return false;
	}
};
int main(){
	string tmp;
	opStack ops(128);
	numStack nums(128);
	int pos;
	char tmpc,op;
	double tmpd,a,b;
	getline(cin,tmp);
	expression exp(tmp);
	while(!exp.isEmpty){
		if(exp.getNextNum(tmpd))
		{
			cout<<"get Num "<< tmpd << endl;
			cout<<tmpd;
			nums.push(tmpd);
		}
		if(exp.getNextOp(tmpc))
		{
			if(tmpc==')')
			{
				while(ops.getTop()!='(')
				{
					nums.pop(b);
					nums.pop(a);
					ops.pop(op);
					nums.push(caculate(a,b,op));
					cout<<op<<' ';
				}
				ops.pop(op);//erase '('
			}
			else{
				while(pri(tmpc)<pri(ops.getTop())&&ops.getTop()!='(')
				{
					nums.pop(b);
					nums.pop(a);
					ops.pop(op);
					nums.push(caculate(a,b,op));
					cout<<op<<' ';
				}// only keep '-' and '+'
				ops.push(tmpc);
			}
		}
	}
	while(!ops.isEmpty()){
		nums.pop(b);
		nums.pop(a);
		ops.pop(op);
		nums.push(caculate(a,b,op));
		cout<<op<<' ';
	}
	cout<< "= " <<nums.getTop();
	return 0;
}
