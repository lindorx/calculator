#include"calculator.h"

const GLfloat winWidth = 500, winHeight = 600;//初始化窗口大小
GLfloat ww = winWidth, wh = winHeight;//ww储存实时窗口的宽度，wh储存高度
const GLfloat keyWidth = 120, keyHeight = 108;//初始化按键
GLfloat keyW = 120, keyH = 108;//实时按键大小
const GLfloat keyposw = (GLfloat)(keyW * 2.0 / ww), keyposh = (GLfloat)(keyH * 2.0 / wh);//按键占比
GLfloat _keyposw = keyposw, _keyposh = keyposh;//实时按键占比大小

const GLfloat textBoxW=winWidth*0.92,textBoxH=120;//初始显示区位置
GLfloat tbw = textBoxW, tbh=textBoxH;//实时显示区
double tbwp =tbw/ww,tbhp= 0.6;

const GLfloat interval = 4.0;//按键之间的间隔
GLfloat val = interval;//储存实时间隔

const GLfloat KEY_COLOR[] = { 1,1,1 };//按键颜色
const GLfloat KEY_DOWN_COLOR[] = { 0.75,0.75,0.75 };//按键按下颜色
static char mouseIsDown = 0;//鼠标按下，置1，弹起后置0

static int mx, my;//保存按下的键
static double numberNum = 0;//储存字符串字符数
static int divNum = 0;
static int addNum = 0;
static int subNum = 0;
static int mulNum = 0;

const double CInterval[5] = { 0.1164,0.12,0.068,0.104,0.056 };//初始化默认占比
static double charInter=0.1164;//数字占比
static double addInter = 0.12;//"+"
static double subInter = 0.068;//"-"
static double mulInter = 0.104;//"*"
static double divInter = 0.056;//"/"

static GLfloat printx = tbwp, printy = tbhp;//时刻指向字符串加载的位置

std::string formula;//实时储存显示在界面上的算式字符串

const std::string keystr[4][4] = {//显示数组
	{"C","0","=","/"},
	{"1","2","3","x"},
	{"4","5","6","-"},
	{"7","8","9","+"}
};

int mouseClickBuf[4][4][2];//根据窗口变化实时储存按键点击范围，

using namespace std;

int calculator(int argc, char* argv[])
{
	glutInit(&argc, argv);//初始化
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);//单缓存，reg颜色，深度缓冲区
	glutInitWindowPosition(100, 100);//窗口在桌面的位置
	glutInitWindowSize(winWidth, winHeight);//窗口大小
	glutCreateWindow("calculator");//窗口标题

	init();
	glutDisplayFunc(display1);//窗口内容绘制
	glutReshapeFunc(winctrl);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	print(1000, 1000);
	glutMainLoop();
	return 0;
}

void selectFont(int size, int charset, const char* face)
{
	//createfonta创建字体
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	//设置字体对象
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	//删除对象
	DeleteObject(hOldFont);
}

void winctrl(int x, int y)
{
	glViewport(0, 0, x, y);
	ww = (GLfloat)x;
	wh = (GLfloat)y;
	windowChange();
}

//初始化窗口界面
void init()
{
	glClearColor(WIN_COLOR);//将窗口背景设置为白色，透明度80%
}

#define STROFFX 0.18
#define STROFFY 0.14
//绘制按键
void key(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const GLfloat * color, std::string str)
{
	//绘制按键
	glColor3fv(color);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(width + x, y);
	glVertex2f(width + x, height + y);
	glVertex2f(x, height + y);
	glEnd();

	//绘制文字
	glColor3f(0, 0, 0);
	//设定文字的显示位置
	glRasterPos2f(x + STROFFX, y + STROFFY);
	//打印文字
	drawString(str.c_str());
	glutSwapBuffers();
}

void display1()
{
	glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲区
	glClearColor(WIN_COLOR);

	selectFont(60, ANSI_CHARSET, "");
	int i, j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			key(POS(j, i), keyposw, keyposh, KEY_COLOR, keystr[i][j]);
		}
	}
	glRasterPos2f(printx, printy);
	drawString(formula.c_str());
	glFlush();
}

void print(GLfloat x, GLfloat y)
{
	cout << "=====================================" << endl;
	cout << "x=" << x << endl;
	cout << "y=" << y << endl;
	cout << "按键占比大小:" << _keyposw << "," << _keyposh << endl;
	cout << "按键大小" << keyW << "," << keyH << endl;
	cout << "窗口大小:" << ww << "," << wh << endl;
	cout << "鼠标状态：" << mouseIsDown << endl;
	cout << "间隔：" << val << endl;
	cout << "字符占比:" << charInter << endl;
	cout << "=====================================" << endl;
}

#define MAX_CHAR 128	//128个ascii字符

void drawString(const char* str)
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) { // 如果是第一次调用，执行初始化
						 // 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;

		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_CHAR);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}
//键盘处理
void keyboard(unsigned char key, int x, int y)
{
	int i, j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			if (keystr[i][j][0] == key) {
				keyDown(i, j);
				mx = i; my = j;
				break;
			}
			else if (key == 8) {//退格键
				mx = 0; my = 0;
				break;
			}
			else if (key == 0x0d) {//回车键
				mx = 0; my = 2;
				break;
			}
		}
	}
	//读取key并处理
	processInput(key);
	//延时，展示效果
	Sleep(100);
	//还原窗口
	keyUp(mx, my);
}

//窗口大小改变，ww和wh值将会被修改，这个函数负责实现按键范围的取值工作，同时修改按键大小keyW，keyH,间隔interval
void windowChange()
{
	//从ww，wh读取数据，首先修改interval
	GLfloat tw = (ww) / winWidth,th=(wh)/winHeight;//获得增加倍数

	cout << "增加倍数:" << tw <<","<<th<< endl;
	val = (GLfloat)(int)(interval * (sqrt(tw*tw+th*th)));
	cout << "修正间隔：interval=" << val << endl;
	//修改按键大小
	keyW = (GLfloat)(int)(keyWidth * tw);
	keyH = (GLfloat)(int)(keyHeight * th);
	cout << "修改按键宽：" << keyW << ",修改按键高：" << keyH << endl;
	_keyposw = (GLfloat)(keyW * 2 / ww);
	_keyposh = (GLfloat)(keyH * 2 / wh);
	//修改文本框大小
	tbw = (GLfloat)(int)(tbw * tw);
	tbh = (GLfloat)(int)(tbw* th);
	//修改字符占比
	double s = 0.1164 * 250;
	charInter = (double)CInterval[0] * winWidth / ww;
	addInter = (double)CInterval[1] * winWidth / ww;
	subInter = (double)CInterval[2] * winWidth / ww;
	mulInter = (double)CInterval[3] * winWidth / ww;
	divInter = (double)CInterval[4] * winWidth / ww;
	
	//填充按键点击数组，数组共有16个坐标，分别对应每个按键左上角的顶点
	int i, j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			mouseClickBuf[i][j][0] = val + j * (val + keyW);
			mouseClickBuf[i][j][1] = wh - (i + 1) * (val + keyH);
			cout << "[" << i << "][" << j << "]=" << mouseClickBuf[i][j][0] << "," << mouseClickBuf[i][j][1] << endl;
		}
	}
	print(ww, wh);
	keyUp(mx,my);
}

//button:三个按键=GLUT_LEFT_BUTTON，GLUT_MIDDLE_BUTTON，GLUT_RIGHT_BUTTON
//state:按下或弹起=GLUT_DOWN，GLUT_UP
//x,y:坐标，原点是窗口左上角
void mouse(int button, int state, int x, int y)
{
	int i = 0, j = 0, a = 0, b = 0;
	char keyValue = 0;//保存按下的键值，与mouseIsDown一起使用
	if (button == GLUT_LEFT_BUTTON) {//左键按下
		std::cout << "鼠标左键按下" << endl;
		std::cout << "mousex=" << x << ",mousey" << y << endl;
		if (state == GLUT_DOWN) {//鼠标左键按下，令按键变色,调用key
			//判断按键，根据mouseClickBuf查找合适的位置
			for (i = 0; i < 4; ++i)
				for (j = 0; j < 4; ++j) {
					//找到合适位置
					if (x > mouseClickBuf[i][j][0] && y > mouseClickBuf[i][j][1] && x < mouseClickBuf[i][j][0] + keyW && y < mouseClickBuf[i][j][1] + keyH) {
						keyValue = keystr[i][j][0];
						keyDown(i, j);
						break;
					}
				}

		}
		else if (state == GLUT_UP) {
			if (mouseIsDown == 1) {
				keyUp(my,my);
				//print(mx, my);
				std::cout << "鼠标弹起" << std::endl;
				mouseIsDown = 0;
			}
		}
	}
	//按下键的位置存入mx，my
	if (mouseIsDown == 1) {
		processInput(keyValue);
	}
}

void printTextBox(double x,double y,std::string str)
{
	//打印字符串到文本框
	int a = 0, b = 0;
	printx = x; printy = y;
	glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲区
	glClearColor(WIN_COLOR);
	selectFont(60, ANSI_CHARSET, "");
	glRasterPos2f(x,y);
	cout << "文字绘制位置:" <<x<<","<<y<< endl;
	if(str.length()>0)drawString(str.c_str());
	if (mouseIsDown == 1 && mx < 4 && my < 4) {
		for (a = 0; a < 4; ++a) {
			for (b = 0; b < 4; ++b) {
				if (a == mx && b == my) {//令指定位置变色
					key(POS(b, a), _keyposw, _keyposh, KEY_DOWN_COLOR, keystr[mx][my]);
					continue;
				}
				key(POS(b, a), keyposw, keyposh, KEY_COLOR, keystr[a][b]);
			}
		}
		glFlush();
	}
	
}


//运算出结果
double opernum(string str)
{
	int temp = 0, i = 0, j = 0;
	string a="";
	//首先转换出所有数字
	double num[50] = { 0 };
	int firstpos[20] = { 0 };//储存第一优先级符号在pos的位置
	int firstnum = 0;
	int secondpos[20]={0};//储存第二优先级符号早pos的位置
	int secondnum = 0;
	char minus = 0,recip=0;//判断负数

	for (i = 0; i < str.length()+1;++i) {
		if (str[i] >= 0x30 && str[i] <= 0x39) {
			a.push_back(str[i]);
		}
		else {
			if (minus > 0) {
				num[j] = -str_to_int(0, a); minus = 0;
			}
			else if (recip > 0) {
				num[j] = (double)1 / str_to_int(0, a); recip = 0;
			}
			else {
				num[j] = str_to_int(0, a);
			}
			a.clear();
			switch (str[i]) {

			case '-': //之后的一个数为负数
				minus = 1;
			case '+':
				secondpos[secondnum++] = j; break;
			case '/':
				recip = 1;
			case 'x':
				firstpos[firstnum++] = j; break;
			}
			j++;
		}
	}
	for (i = 0; i < firstnum; ++i) {
		num[firstpos[i]] *= num[firstpos[i] + 1];
		//将num[firstpos[i] + 1]向前平移一段
		for (temp=firstpos[i]+1; temp < j; ++temp) {
			num[temp] = num[temp + 1];
		}
		--j; firstpos[i + 1]= firstpos[i + 1]-1-i;
	}
	for (i = 0; i < secondnum; ++i) {
		num[0] += num[i+1];
	}
	cout << "计算结果："<<num[0] << endl;
	return num[0];
}
//将string类型字符串转换为整型数字,mirror:=0说明字符从左到右排列，否则从右到左排列，str：待转换字符串
double str_to_int(const char mirror,std::string str)
{
	double num = 0;
	int i,t=0;
	for (i = 0; i < str.length(); ++i) {
		t = (mirror > 0) ? (str.length() - 1 - i) : i;
		num += (str[t]-'0')*(pow(10, str.length()-1-i));
	}
	return num;
}

//处理输入的字符
void processInput(char keyValue)
{
#define TEXTX tbwp- divInter * divNum-addInter * addNum-subInter * subNum-mulInter*mulNum-(++numberNum*charInter)
#define TEXTY tbhp
#define ADDOPERX tbwp- divInter * divNum-addInter * (++addNum)-subInter * subNum-mulInter*mulNum-(numberNum*charInter)
#define SUBOPERX tbwp- divInter * divNum-addInter * addNum-subInter * (++subNum)-mulInter*mulNum-(numberNum*charInter)
#define MULOPERX tbwp- divInter * divNum-addInter * addNum-subInter * subNum-mulInter*(++mulNum)-(numberNum*charInter)
#define DIVOPERX tbwp- divInter * (++divNum)-addInter * addNum-subInter * subNum-mulInter*mulNum-(numberNum*charInter)

	//cout << "divNum：" << divNum << ",addNum:" << addNum << ",subNum:" << subNum << ",mulNum:"<< mulNum<<",numberNum:"<< numberNum << endl;
	int strsize = formula.length() - 1;
	strsize = (strsize > 0) ? strsize : 0;
	switch (keyValue) {
	case '1': {	
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '2': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '3': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '4': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '5': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '6': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '7': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '8': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '9': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '0': {
		formula.push_back(keyValue);
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	case '+': {
		if (strsize+1 <= 0)break;
		if (formula[strsize] < 0x30 || formula[strsize]>0x39) {
			switch (formula[strsize]) {
			case '/':divNum--; break;
			case '-':subNum--; break;
			case 'x':mulNum--; break;
			case '+':return;
			}
			formula[strsize] = '+';
		}
		else {
			formula.push_back(keyValue);
		}
		mx = 3; my = 3;
		printTextBox(ADDOPERX, TEXTY, formula);
		break;
	}
	case '-': {
		if (strsize + 1 <= 0)break;
		if (formula[strsize] < 0x30 || formula[strsize]>0x39) {
			switch (formula[strsize]) {
			case '/':divNum--; break;
			case '-':return;
			case 'x':mulNum--; break;
			case '+':addNum--; break;
			}
			formula[strsize] = '-';
		}
		else {
			formula.push_back(keyValue);
		}
		mx = 2; my = 3;
		printTextBox(SUBOPERX, TEXTY, formula);
		break;
	}
	case 42://'*'键值
	case 'X':
	case 'x': {
		if (strsize+1 <= 0)break;
		if (formula[strsize] < 0x30 || formula[strsize]>0x39) {
			switch (formula[strsize]) {
			case '/':divNum--; break;
			case '-':subNum--; break;
			case 'x':return;
			case '+':addNum--; break;
			}
			formula[strsize] = 'x';
		}
		else {
			formula.push_back('x');
		}
		mx = 1; my = 3;
		printTextBox(MULOPERX, TEXTY, formula);
		break;
	}
	case '/': {
		if (strsize + 1 <= 0)break;
		if (formula[strsize] < 0x30 || formula[strsize]>0x39) {
			switch (formula[strsize]) {
			case '/':return;
			case '-':subNum--; break;
			case 'x':mulNum--; break;
			case '+':addNum--; break;
			}
			formula[strsize] = '/';
		}
		else {
			formula.push_back(keyValue);
		}
		mx = 0; my = 3;
		printTextBox(DIVOPERX, TEXTY, formula);
		break;
	}
	case 0x0d:
	case '=': {//处理等号，对算式处理，生成结果
		double result = opernum(formula);
		if (result > pow(10, MAXNUMBER * 2)) {
			result = 0;
		}
		char resultstr[MAXNUMBER*2] = { 0 };
		//将结果打印在窗口上
		_itoa_s((int)result, resultstr, 10);
		resultstr[MAXNUMBER*2 - 1] = '\0';
		//打印
		formula = resultstr;
		numberNum = formula.length()-1;
		divNum = 0;
		addNum = 0;
		subNum = 0;
		mulNum = 0;
		printTextBox(TEXTX, TEXTY, resultstr);
		break;
	}
	case 8:
	case 'c':
	case 'C': {
		formula.clear();
		numberNum = 0;
		divNum = 0;
		addNum = 0;
		subNum = 0;
		mulNum = 0;
		printTextBox(TEXTX, TEXTY, formula);
		break;
	}
	}
}

//令指定位置的按键变色
void keyDown(int x,int y)
{
	if (x > 3 || y > 3)return;
	int i, j;
	mx = x; my = y;
	//glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲区
	glClearColor(WIN_COLOR);
	selectFont(60, ANSI_CHARSET, "");
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			if (i == x && j == y) {
				key(POS(j, i), _keyposw, _keyposh, KEY_DOWN_COLOR, keystr[x][y]);
				continue;
			}
			key(POS(j, i), keyposw, keyposh, KEY_COLOR, keystr[i][j]);
		}
	}
	glFlush();
	//print(mx, my);
	mouseIsDown = 1;//置1，说明按下
}

//恢复所有按键
void keyUp(int a,int b)
{
	glClearColor(WIN_COLOR);
	selectFont(60, ANSI_CHARSET, "");
	//全部重绘，变成原始状态
	if (mx < 4 && my < 4) {
		for (a = 0; a < 4; ++a) {
			for (b = 0; b < 4; ++b) {
				key(POS(b, a), keyposw, keyposh, KEY_COLOR, keystr[a][b]);
			}
		}
		glFlush();
	}
}