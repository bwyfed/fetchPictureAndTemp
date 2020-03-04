// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
#include <iostream>
using namespace std;

int Add(int a, int b) {
	int result = a + b;
	cout << "In dll..., Add function, a=" << a << ",b=" << b << ", result = " << result << endl;
	return result;
}