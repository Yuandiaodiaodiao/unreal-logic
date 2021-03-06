// CudaTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "kernel.cuh"
#include <string>
using namespace std;
int main()
{
    string s="2";
    string a="1";
    s.append(a);
    cout << s << endl;
    initCuda();
    int**xx= new int* [6];
    int arraySize = 32;
    for (int a = 0; a <= 5; ++a) {
        xx[a] = new int[arraySize];
    }
    const char* ss= doPreformanceTest(xx[0],xx[1],xx[2],xx[3],xx[4],xx[5],arraySize,100);
    
    std::cout << "Hello World!\n" << ss;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
