#pragma once		//告诉编译器仅包含一次本头文件
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//隐藏控制台
#include<gl\glut.h>
#include<iostream>
#include<string>
#include<Windows.h>
#include<math.h>

//主函数
int calculator(int argc, char* argv[]);
//初始化
void init();
//初始窗口绘制
void display1();
//窗口控制函数
void winctrl(int x, int y);
//打印窗口信息
void print(GLfloat x, GLfloat y);
//由glRasterPos2f(GLfloat x,GLfloat Y);指定位置，然后drawString绘制文字
void drawString(const char* str);
//设置字体和大小
void selectFont(int size, int charset, const char* face);
//窗口大小发生改变时实时修改全局变量
void windowChange();
//在指定位置绘制文本串
void printTextBox(double x, double y, std::string str);
//绘制按键
void key(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const GLfloat* color, std::string str);
//键盘函数
void keyboard(unsigned char key, int x, int y);
//鼠标函数
void mouse(int button, int state, int x, int y);
//将string类型字符串转换为整型数字,mirror:=0说明字符从左到右排列，否则从右到左排列，str：待转换字符串
double str_to_int(const char mirror, std::string str);
//将str中的等式运算出结果
double opernum(std::string str);
//处理按键输入的字符
void processInput(char keyValue);
//令指定位置的按键变色
void keyDown(int x, int y);
//恢复所有按键
void keyUp(int a, int b);

//保护宏，如果没有#pragma once	，则需要启用下面的宏
#ifndef CALCULATOR_H
#define CALCULATOR_H

#define WIN_COLOR 0.8,0.8,0.8,0.8	//背景颜色
#define MAXNUMBER 9			//每个数字字符串最大长度

//参数：从左下角计算，POS（0，0）即表示左下角第一个按键，指定按键绘制的位置
#define POS(x,y) (((x)*(keyWidth+interval)+interval)*2/winWidth-1.0),(((y)*(keyHeight+interval)+interval)*2/winHeight-1.0)

#endif