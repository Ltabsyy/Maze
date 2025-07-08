#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <graphics.h>
/**
 * 迷宫 Maze Power
 * 
 * by Ltabsyy
 * 
 * https://github.com/Ltabsyy/Maze
 **/
#define LimRow 240
#define LimColumn 448

int maze[2*LimRow+1][2*LimColumn+1]={0};//含墙完整迷宫，0路，1墙
char visitDirection[2*LimRow+1][2*LimColumn+1]={0};//迷宫已访问路径，0未访问，><v^字符代表方向
char mainDirection[2*LimRow+1][2*LimColumn+1]={0};//已访问路径中从起点到当前位置的主路

int rowOfPath = 9;//迷宫路行数
int columnOfPath = 9;//迷宫路列数
int rowOfMaze = 2*9+1;
int columnOfMaze = 2*9+1;
int summonMode = 1;//0广度优先，1深广混合，2深度优先
int sideLength = 32;

void DrawSelection(int rp, int cp)
{
	int r, c;
	const char* sizeName[6] = {"XS", "S", "M", "L", "XL", "C"};
	cleardevice();
	setfont(sideLength*5/2, 0, "Consolas");
	setcolor(BLACK);
	for(r=0; r<3; r++)
	{
		for(c=4; c<10; c++)
		{
			setfillcolor((r == rp && c == cp) ? GRAY : LIGHTGRAY);
			ege_fillrect(c*3*sideLength+sideLength/6, r*3*sideLength+sideLength/6, sideLength*8/3, sideLength*8/3);
			if(sizeName[c-4][1]) xyprintf(c*3*sideLength+sideLength*3/8, r*3*sideLength+sideLength/4, sizeName[c-4]);//原1/4，略右移以居中
			else xyprintf(c*3*sideLength+sideLength*7/8, r*3*sideLength+sideLength/4, sizeName[c-4]);
		}
	}
	setcolor(LIGHTGRAY);
	xyprintf(sideLength*3/8, 0*sideLength+sideLength/4, "Easy(BFS)");
	xyprintf(sideLength*3/8, 3*sideLength+sideLength/4, "Normal");
	xyprintf(sideLength*3/8, 6*sideLength+sideLength/4, "Hard(DFS)");
}

void DrawMaze()//绘制迷宫
{
	int r, c, r1, c1, link;
	setcolor(DARKGRAY);
	setfillcolor(LIGHTGRAY);
	setlinewidth(sideLength < 16 ? 2 : sideLength/16*2);//线宽为2起偶数避免模糊
	//逐行绘制横线
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(maze[r][c] == 1)
			{
				for(c1=c; c<columnOfMaze && maze[r][c] == 1; c++);
				c--;
				ege_line(c1*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	//逐列绘制纵线
	for(c=0; c<columnOfMaze; c++)
	{
		for(r=0; r<rowOfMaze; r++)
		{
			if(maze[r][c] == 1)
			{
				for(r1=r; r<rowOfMaze && maze[r][c] == 1; r++);
				r--;
				ege_line(c*sideLength+sideLength/2, r1*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	//逐点绘制单连大圆
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			//xyprintf(c*sideLength+sideLength/4, r*sideLength, "%d", maze[r][c]);
			if(maze[r][c] == 1)
			{
				link = 0;
				if(r > 0 && maze[r-1][c] == 1) link |= 8;
				if(c+1 < columnOfMaze && maze[r][c+1] == 1) link |= 4;
				if(r+1 < rowOfMaze && maze[r+1][c] == 1) link |= 2;
				if(c > 0 && maze[r][c-1] == 1) link |= 1;
				if(link == 8 || link == 4 || link == 2 || link == 1 || link == 0)
				{
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/3);
				}
			}
		}
	}
}

void DrawPath(int rp, int cp)//绘制路径
{
	int r, c, r1, c1, link;
	//绘制支路(已访问路径-主路)
	setcolor(DARKBLUE);
	setfillcolor(BLUE);
	setlinewidth(sideLength < 8 ? 2 : sideLength/8*2);//线宽为2起偶数避免模糊
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(visitDirection[r][c] && !mainDirection[r][c])
			{
				c1 = c;
				if(c > 0 && mainDirection[r][c-1]) c1--;//绘制支路与主路的连接部分
				for(; c<columnOfMaze && visitDirection[r][c] && !mainDirection[r][c]; c++);
				if(!(c<columnOfMaze && mainDirection[r][c])) c--;
				ege_line(c1*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	for(c=0; c<columnOfMaze; c++)
	{
		for(r=0; r<rowOfMaze; r++)
		{
			if(visitDirection[r][c] && !mainDirection[r][c])
			{
				r1 = r;
				if(r > 0 && mainDirection[r-1][c]) r1--;
				for(; r<rowOfMaze && visitDirection[r][c] && !mainDirection[r][c]; r++);
				if(!(r<rowOfMaze && mainDirection[r][c])) r--;
				ege_line(c*sideLength+sideLength/2, r1*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(visitDirection[r][c] && !mainDirection[r][c])
			{
				link = 0;
				if(r > 0 && visitDirection[r-1][c]) link |= 8;
				if(c+1 < columnOfMaze && visitDirection[r][c+1]) link |= 4;
				if(r+1 < rowOfMaze && visitDirection[r+1][c]) link |= 2;
				if(c > 0 && visitDirection[r][c-1]) link |= 1;
				if(link == 8 || link == 4 || link == 2 || link == 1 || link == 0)//单连中圆
				{
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/4);
				}
			}
		}
	}
	//绘制主路
	setcolor(GREEN);//新生的绿意缠绕着深蓝的根(doge)
	setfillcolor(LIME);
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(mainDirection[r][c])
			{
				for(c1=c; c<columnOfMaze && mainDirection[r][c]; c++);
				c--;
				ege_line(c1*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	for(c=0; c<columnOfMaze; c++)
	{
		for(r=0; r<rowOfMaze; r++)
		{
			if(mainDirection[r][c])
			{
				for(r1=r; r<rowOfMaze && mainDirection[r][c]; r++);
				r--;
				ege_line(c*sideLength+sideLength/2, r1*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength+sideLength/2);
			}
		}
	}
	ege_fillcircle(0*sideLength+sideLength/2, 1*sideLength+sideLength/2, sideLength/4);//主路的单连中圆仅在起点
	/*for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(mainDirection[r][c])
			{
				setcolor(LIME);
				xyprintf(c*sideLength+sideLength/4, r*sideLength, "%c", mainDirection[r][c]);
			}
			else if(visitDirection[r][c])
			{
				setcolor(BLUE);
				xyprintf(c*sideLength+sideLength/4, r*sideLength, "%c", visitDirection[r][c]);
			}
		}
	}*/
	//绘制当前位置
	setcolor(DARKRED);
	setfillcolor(RED);
	if(mainDirection[rp][cp] == 'v')//在主路反向绘制小段红线指示来时路
	{
		ege_line(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, cp*sideLength+sideLength/2, rp*sideLength);
	}
	else if(mainDirection[rp][cp] == '<')
	{
		ege_line(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, (cp+1)*sideLength, rp*sideLength+sideLength/2);
	}
	else if(mainDirection[rp][cp] == '^')
	{
		ege_line(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, cp*sideLength+sideLength/2, (rp+1)*sideLength);
	}
	else if(mainDirection[rp][cp] == '>')
	{
		ege_line(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, cp*sideLength, rp*sideLength+sideLength/2);
	}
	ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3);//红色大圆
}

void InitWindow(int mode)
{
	int screenHeight, screenWidth;
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	if(EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) == 0)//无法获取显示屏分辨率
	{
		screenWidth = 1920;
		screenHeight = 1080;
	}
	else
	{
		screenWidth = dm.dmPelsWidth;
		screenHeight = dm.dmPelsHeight;
	}
	if(mode == 0)
	{
		if(screenHeight >= 2160) sideLength = 48;
		else if(screenHeight >= 1440) sideLength = 32;
		else if(screenHeight >= 1080) sideLength = 24;
		else sideLength = 16;
		setcaption("Maze Power");
		SetProcessDPIAware();//避免Windows缩放造成模糊
		initgraph(10*3*sideLength, 3*3*sideLength, INIT_RENDERMANUAL);
		setbkcolor(BLACK);
		//setfont(sideLength, 0, "Consolas");
		setbkmode(TRANSPARENT);//默认设置为无背景字体
		setlinecap(LINECAP_ROUND);//设置圆形线帽
		ege_enable_aa(true);
	}
	else
	{
		if(summonMode == 2) setcaption("Maze Power - DFS");
		else if(summonMode == 1) setcaption("Maze Power - Mix");
		else setcaption("Maze Power - BFS");
		while(columnOfMaze*sideLength > screenWidth || rowOfMaze*sideLength > screenHeight*10/11)
		{
			if(sideLength > 16) sideLength -= 4;
			else sideLength--;
		}
		if(sideLength < 2) sideLength = 2;
		resizewindow(columnOfMaze*sideLength, rowOfMaze*sideLength);
		setfont(sideLength, 0, "Consolas");
	}
}

void MoveTo(int rt, int ct, int* prs, int* pcs)//直线移动并更新路径
{
	static int rs = 1, cs = 0;
	int r = rs, c = cs, check = 1;
	if(rt == 0 && ct == 0)//重置归位
	{
		rs = 1;
		cs = 0;
	}
	else if(rt >= 0 && rt < rowOfMaze && ct >= 0 && ct < columnOfMaze && maze[rt][ct] != 1)
	{
		if(rt == rs && ct < cs)//左移
		{
			for(; c>=ct; c--)
			{
				if(maze[r][c] == 1) check = 0;
			}
			if(check == 1)//不存在阻挡
			{
				for(c=cs; c>=ct; c--)
				{
					visitDirection[r][c] = '<';
				}
				cs = ct;
			}
		}
		else if(rt == rs && ct > cs)//右移
		{
			for(; c<=ct; c++)
			{
				if(maze[r][c] == 1) check = 0;
			}
			if(check == 1)
			{
				for(c=cs; c<=ct; c++)
				{
					visitDirection[r][c] = '>';
				}
				cs = ct;
			}
		}
		else if(ct == cs && rt < rs)//上移
		{
			for(; r>=rt; r--)
			{
				if(maze[r][c] == 1) check = 0;
			}
			if(check == 1)
			{
				for(r=rs; r>=rt; r--)
				{
					visitDirection[r][c] = '^';
				}
				rs = rt;
			}
		}
		else if(ct == cs && rt > rs)//下移
		{
			for(; r<=rt; r++)
			{
				if(maze[r][c] == 1) check = 0;
			}
			if(check == 1)
			{
				for(r=rs; r<=rt; r++)
				{
					visitDirection[r][c] = 'v';
				}
				rs = rt;
			}
		}
	}
	if(prs != NULL) *prs = rs;
	if(pcs != NULL) *pcs = cs;
}

void MovePolyline(int rt, int ct, int* prs, int* pcs)//折线移动并更新路径
{
	int rs, cs;
	MoveTo(-1, -1, &rs, &cs);//获取当前位置
	if(rt >= 0 && rt < rowOfMaze && ct >= 0 && ct < columnOfMaze && maze[rt][ct] != 1)
	{
		if(rt == rs || ct == cs)//不移动或直线移动
		{
			MoveTo(rt, ct, &rs, &cs);
		}
		else
		{
			int r1, c1, r2, c2, r, c, check = 15;
			//确定左上点和右下点
			r1 = (rs < rt) ? rs : rt;
			r2 = (rs > rt) ? rs : rt;
			c1 = (cs < ct) ? cs : ct;
			c2 = (cs > ct) ? cs : ct;
			//计算4条线的可移动性
			for(r=r1, c=c1; c<=c2; c++)
			{
				if(maze[r][c] == 1) check &= ~8;//不可移动则置0，8421为可移动上右下左
			}
			for(c--; r<=r2; r++)//环形遍历
			{
				if(maze[r][c] == 1) check &= ~4;
			}
			for(r--; c>=c1; c--)
			{
				if(maze[r][c] == 1) check &= ~2;
			}
			for(c++; r>=r1; r--)
			{
				if(maze[r][c] == 1) check &= ~1;
			}
			//移动
			if((rt == r1 && ct == c1) || (rt == r2 && ct == c2))//左上-右下
			{
				if((check & 12) == 12)//上右可移，包含上右下、上右左可移
				{
					MoveTo(r1, c2, &rs, &cs);//移动到右上
					MoveTo(rt, ct, &rs, &cs);
				}
				else if((check & 3) == 3)//下左
				{
					MoveTo(r2, c1, &rs, &cs);//移动到左下
					MoveTo(rt, ct, &rs, &cs);
				}
			}
			else// if((rt == r2 && ct == c1) || (rt == r1 && ct == c2))//左下-右上
			{
				if((check & 9) == 9)//上左
				{
					MoveTo(r1, c1, &rs, &cs);//移动到左上
					MoveTo(rt, ct, &rs, &cs);
				}
				else if((check & 6) == 6)//右下
				{
					MoveTo(r2, c2, &rs, &cs);//移动到右下
					MoveTo(rt, ct, &rs, &cs);
				}
			}
		}
	}
	if(prs != NULL) *prs = rs;
	if(pcs != NULL) *pcs = cs;
}

void MoveTowards(char direction, int* prs, int* pcs)//根据方向移动到岔路口并更新路径
{
	int rs, cs, rt, ct;
	int isVisited[2*LimRow+1][2*LimColumn+1]={0};
	MoveTo(-1, -1, &rs, &cs);//获取当前位置
	rt = rs;
	ct = cs;
	isVisited[rt][ct] = 1;//标记已访问
	if(direction == '<' || direction == 'A') ct--;
	else if(direction == '>' || direction == 'D') ct++;
	else if(direction == '^' || direction == 'W') rt--;
	else if(direction == 'v' || direction == 'S') rt++;
	MoveTo(rt, ct, &rs, &cs);//尝试移动
	if(rs == rt && cs == ct)//移动成功
	{
		isVisited[rt][ct] = 1;//来路和当前位置都标已访问
		while(1)
		{
			//计算四方向可移动数
			direction = 0;
			if(rt > 0 && maze[rt-1][ct] != 1 && !isVisited[rt-1][ct]) direction |= 8;
			if(ct+1 < columnOfMaze && maze[rt][ct+1] != 1 && !isVisited[rt][ct+1]) direction |= 4;
			if(rt+1 < rowOfMaze && maze[rt+1][ct] != 1 && !isVisited[rt+1][ct]) direction |= 2;
			if(ct > 0 && maze[rt][ct-1] != 1 && !isVisited[rt][ct-1]) direction |= 1;
			//移动
			if(direction == 8) rt--;
			else if(direction == 4) ct++;
			else if(direction == 2) rt++;
			else if(direction == 1) ct--;
			else break;//遇到岔路口退出
			MoveTo(rt, ct, &rs, &cs);
			isVisited[rt][ct] = 1;
		}
	}
	if(prs != NULL) *prs = rs;
	if(pcs != NULL) *pcs = cs;
}

void MoveBack(int* prs, int* pcs)//沿主路返回上一个岔路口并更新路径
{
	int rs, cs;
	MoveTo(-1, -1, &rs, &cs);//获取当前位置
	//在周围寻找主路并移动
	if(cs > 0 && mainDirection[rs][cs-1]) MoveTowards('<', &rs, &cs);
	else if(cs+1 < columnOfMaze && mainDirection[rs][cs+1]) MoveTowards('>', &rs, &cs);
	else if(rs > 0 && mainDirection[rs-1][cs]) MoveTowards('^', &rs, &cs);
	else if(rs+1 < rowOfMaze && mainDirection[rs+1][cs]) MoveTowards('v', &rs, &cs);
	if(prs != NULL) *prs = rs;
	if(pcs != NULL) *pcs = cs;
}

void MoveBackTo(int rt, int ct, int* prs, int* pcs)//沿主路返回指定位置并更新路径
{
	int rs, cs;
	MoveTo(-1, -1, &rs, &cs);//获取当前位置
	if(rt >= 0 && rt < rowOfMaze && ct >= 0 && ct < columnOfMaze && mainDirection[rt][ct])
	{
		while(rs != rt || cs != ct)
		{
			mainDirection[rs][cs] = 0;
			if(cs > 0 && mainDirection[rs][cs-1]) MoveTo(rs, cs-1, &rs, &cs);
			else if(cs+1 < columnOfMaze && mainDirection[rs][cs+1]) MoveTo(rs, cs+1, &rs, &cs);
			else if(rs > 0 && mainDirection[rs-1][cs]) MoveTo(rs-1, cs, &rs, &cs);
			else if(rs+1 < rowOfMaze && mainDirection[rs+1][cs]) MoveTo(rs+1, cs, &rs, &cs);
		}
	}
	if(prs != NULL) *prs = rs;
	if(pcs != NULL) *pcs = cs;
}

void UpdateMainPath(int rp, int cp)//计算主路
{
	int r, c;
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			mainDirection[r][c] = 0;
		}
	}
	for(r=1, c=0; !(r == rp && c == cp); )
	{
		mainDirection[r][c] = visitDirection[r][c];
		if(visitDirection[r][c] == '<') c--;
		else if(visitDirection[r][c] == '>') c++;
		else if(visitDirection[r][c] == '^') r--;
		else if(visitDirection[r][c] == 'v') r++;
	}
	mainDirection[rp][cp] = visitDirection[rp][cp];
}

void AdjustCoord(int x, int y, int* pr, int* pc)//墙格尝试基于像素坐标转为对应空格坐标（墙格显示为线条）
{
	const static int adjustList[16][4] = {
		{0, 0, 0, 0},//0无连
		{8, 4, 2, 4},//1单连左，向根部二边和指向一边调整
		{8, 8, 1, 4},//2单连下
		{8, 12, 3, 4},//3拐双连┓，向轴向二角和翼部二边调整
		{1, 8, 1, 2},//4单连右
		{8, 8, 2, 2},//5平双连━，向二边调整
		{9, 8, 1, 6},//6拐双连┏
		{8, 8, 3, 6},//7三连┳，向二角和一边调整
		{1, 4, 2, 2},//8单连上
		{9, 4, 2, 6},//9拐双连┛
		{1, 4, 1, 4},//10平双连┃
		{9, 4, 3, 4},//11三连┫
		{1, 12, 3, 2},//12拐双连┗
		{9, 12, 2, 2},//13三连┻
		{1, 12, 1, 6},//14三连┣
		{9, 12, 3, 6}//15四连╋，向四角调整
	};
	int r, c, link = 0, quadrant = 0, adjust = 0;
	r = y / sideLength;
	c = x / sideLength;
	if(maze[r][c] == 1)//非墙格无需转换
	{
		//计算连线形式，8421为上右下左的连接性
		if(r > 0 && maze[r-1][c] == 1) link |= 8;
		if(c+1 < columnOfMaze && maze[r][c+1] == 1) link |= 4;
		if(r+1 < rowOfMaze && maze[r+1][c] == 1) link |= 2;
		if(c > 0 && maze[r][c-1] == 1) link |= 1;
		//计算所处象限，21为偏下和偏右
		//0 1
		//2 3
		if(y % sideLength >= sideLength/2) quadrant |= 2;
		if(x % sideLength >= sideLength/2) quadrant |= 1;
		//计算调整形式，8421为是否向上右下左调整
		//9 8 12
		//1 0 4
		//3 2 6
		adjust = adjustList[link][quadrant];//查表
		//调整坐标
		if(adjust & 8) r--;
		if(adjust & 4) c++;
		if(adjust & 2) r++;
		if(adjust & 1) c--;
	}
	if(pr != NULL) *pr = r;
	if(pc != NULL) *pc = c;
}

void SummonMaze(int seed)//生成迷宫
{
	int r, c, i, rt, ct;
	const int dr[4] = {0, 0, -1, 1};//方向偏移数组
	const int dc[4] = {-1, 1, 0, 0};
	int isVisited[LimRow][LimColumn]={0};//标记(2r+1,2c+1)的路已访问
	//初始化
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			maze[r][c] = 0;
			visitDirection[r][c] = 0;
			mainDirection[r][c] = 0;
			if(r%2 == 0 || c%2 == 0 || r == rowOfMaze-1 || c == columnOfMaze-1)
			{
				maze[r][c] = 1;//墙满，生成时挖
			}
		}
	}
	maze[1][0] = 0;
	maze[rowOfMaze-2][columnOfMaze-1] = 0;
	//生成
	srand(seed);
	isVisited[rowOfPath-1][columnOfPath-1] = 1;
	if(summonMode == 2)//深度优先
	{
		int stackR[LimRow*LimColumn];
		int stackC[LimRow*LimColumn];
		int top = 0;
		stackR[top] = rowOfMaze-2;
		stackC[top] = columnOfMaze-2;
		top++;//终点入栈
		while(top > 0)//栈非空
		{
			r = stackR[top-1];
			c = stackC[top-1];
			i = rand()%4;
			rt = r + dr[i]*2;
			ct = c + dc[i]*2;
			//栈顶尝试移动
			if(rt >= 0 && rt < rowOfMaze && ct >= 0 && ct < columnOfMaze
				&& !isVisited[(rt-1)/2][(ct-1)/2])//未出界且未访问
			{
				maze[r+dr[i]][c+dc[i]] = 0;//挖开
				isVisited[(rt-1)/2][(ct-1)/2] = 1;
				stackR[top] = rt;
				stackC[top] = ct;
				top++;//下次循环将以新点继续尝试
			}
			//栈顶走死则出栈
			while(top > 0)
			{
				r = stackR[top-1];
				c = stackC[top-1];
				if((r == 1 || isVisited[(r-3)/2][(c-1)/2])
					&& (r == rowOfMaze-2 || isVisited[(r+1)/2][(c-1)/2])
					&& (c == 1 || isVisited[(r-1)/2][(c-3)/2])
					&& (c == columnOfMaze-2 || isVisited[(r-1)/2][(c+1)/2]))
				{
					top--;
				}
				else break;
			}
			//栈顶此次未能移动且未走死，下次循环再次尝试
		}
	}
	else//广度优先或深广混合
	{
		int queueR[LimRow*LimColumn];
		int queueC[LimRow*LimColumn];
		int front = 0, rear = 0;
		queueR[rear] = rowOfMaze-2;
		queueC[rear] = columnOfMaze-2;
		rear++;//终点入队
		while(front < rear)//队列非空
		{
			//在队列中随机选择点
			i = front + rand() % (rear-front);
			r = queueR[i];
			c = queueC[i];
			while(1)
			{
				if((r == 1 || isVisited[(r-3)/2][(c-1)/2])
					&& (r == rowOfMaze-2 || isVisited[(r+1)/2][(c-1)/2])
					&& (c == 1 || isVisited[(r-1)/2][(c-3)/2])
					&& (c == columnOfMaze-2 || isVisited[(r-1)/2][(c+1)/2]))
				{
					break;//走死退出
				}
				i = rand()%4;
				rt = r + dr[i]*2;
				ct = c + dc[i]*2;
				//尝试移动
				if(rt >= 0 && rt < rowOfMaze && ct >= 0 && ct < columnOfMaze
					&& !isVisited[(rt-1)/2][(ct-1)/2])//未出界且未访问
				{
					maze[r+dr[i]][c+dc[i]] = 0;//挖开
					isVisited[(rt-1)/2][(ct-1)/2] = 1;
					queueR[rear] = rt;
					queueC[rear] = ct;
					rear++;
					if(summonMode == 1)//混合模式连续移动
					{
						r = rt;
						c = ct;
					}
					else break;//广度优先重新选点
				}
			}
			//队首走死则出队
			while(front < rear)
			{
				r = queueR[front];
				c = queueC[front];
				if((r == 1 || isVisited[(r-3)/2][(c-1)/2])
					&& (r == rowOfMaze-2 || isVisited[(r+1)/2][(c-1)/2])
					&& (c == 1 || isVisited[(r-1)/2][(c-3)/2])
					&& (c == columnOfMaze-2 || isVisited[(r-1)/2][(c+1)/2]))
				{
					front++;
				}
				else break;
			}
		}
	}
}

void Solve()//广度优先遍历法求解迷宫
{
	int r, c, i, rt, ct;
	const int dr[4] = {0, 0, -1, 1};//方向偏移数组
	const int dc[4] = {-1, 1, 0, 0};
	int isVisited[2*LimRow+1][2*LimColumn+1]={0};
	char direction[2*LimRow+1][2*LimColumn+1]={0};//暂存解方向
	int queueR[(2*LimRow+1)*(2*LimColumn+1)];//队列可容纳全部点，无需考虑队满
	int queueC[(2*LimRow+1)*(2*LimColumn+1)];
	int front = 0, rear = 0;
	isVisited[rowOfMaze-2][columnOfMaze-1] = 1;
	direction[rowOfMaze-2][columnOfMaze-1] = '>';
	queueR[rear] = rowOfMaze-2;
	queueC[rear] = columnOfMaze-1;
	rear++;//终点入队
	while(front < rear)//队列非空
	{
		r = queueR[front];
		c = queueC[front];
		front++;
		if(r == 1 && c == 0) break;//到达起点退出
		for(i=0; i<4; i++)
		{
			rt = r + dr[i];
			ct = c + dc[i];
			if(ct == -1 || ct == columnOfMaze) continue;//不能从起点或终点出界
			if(maze[rt][ct] == 0 && !isVisited[rt][ct])
			{
				isVisited[rt][ct] = 1;
				direction[rt][ct] = "><v^"[i];//目标点指向当前点，记录当前方向的反向字符
				queueR[rear] = rt;
				queueC[rear] = ct;
				rear++;
			}
		}
	}
	//输出唯一路径
	MoveTo(0, 0, NULL, NULL);
	for(r=1, c=0; !(r == rowOfMaze-2 && c == columnOfMaze-1); )
	{
		visitDirection[r][c] = direction[r][c];
		if(direction[r][c] == '<') c--;
		else if(direction[r][c] == '>') c++;
		else if(direction[r][c] == '^') r--;
		else if(direction[r][c] == 'v') r++;
		MoveTo(r, c, NULL, NULL);
	}
	MoveTo(rowOfMaze-2, columnOfMaze-1, NULL, NULL);
}

int main()
{
	int difficulty = 0;
	int rp, cp, rm, cm;
	int seed, isMoving = 0;
	mouse_msg mouseMsg;
	key_msg keyMsg;
	// 设置难度
	InitWindow(0);
	rp = 1;
	cp = -1;
	while(difficulty == 0)
	{
		DrawSelection(rp, cp);
		while(mousemsg())
		{
			mouseMsg = getmouse();
			rp = mouseMsg.y / (3*sideLength);
			cp = mouseMsg.x / (3*sideLength);
			if(mouseMsg.is_up())//选择难度
			{
				summonMode = (rp == 0 || rp == 2) ? rp : 1;
				difficulty = (cp >= 4 && cp <= 8) ? cp-3 : 6;
			}
			if(mouseMsg.is_wheel() && keystate(key_control))//调整显示大小
			{
				if(mouseMsg.wheel > 0)
				{
					if(sideLength >= 16) sideLength += 4;
					else sideLength++;
				}
				else
				{
					if(sideLength > 16) sideLength -= 4;
					else if(sideLength > 6) sideLength--;
				}
				resizewindow(10*3*sideLength, 3*3*sideLength);
				DrawSelection(rp, cp);
			}
		}
		while(kbmsg())
		{
			keyMsg = getkey();
			if(keyMsg.msg == key_msg_down)//按下时选择难度，长按时连续触发
			{
				if(keyMsg.key == 'W') rp--;
				else if(keyMsg.key == 'A') cp--;
				else if(keyMsg.key == 'S') rp++;
				else if(keyMsg.key == 'D') cp++;
				else if(keyMsg.key == 'E') rp = 0;
				else if(keyMsg.key == 'N') rp = 1;
				else if(keyMsg.key == 'H') rp = 2;
				else if(keyMsg.key == 'C') cp = 9;
				else if(keyMsg.key == key_left) cp--;
				else if(keyMsg.key == key_up) rp--;
				else if(keyMsg.key == key_right) cp++;
				else if(keyMsg.key == key_down) rp++;
				if(rp < 0) rp = 0;
				if(cp < 4) cp = 4;
				if(rp > 2) rp = 2;
				if(cp > 9) cp = 9;
				DrawSelection(rp, cp);
			}
			if(keyMsg.msg == key_msg_up)//抬起时确定选择
			{
				if(keyMsg.key == '\r' || keyMsg.key == ' ' || keyMsg.key == 'C')
				{
					summonMode = rp;
					difficulty = cp-3;
				}
			}
		}
		delay_ms(50);
	}
	if(difficulty == 1)//超小规模XS
	{
		rowOfPath = 9;
		columnOfPath = 9;
	}
	else if(difficulty == 2)//小规模S
	{
		rowOfPath = 15;
		columnOfPath = 15;
	}
	else if(difficulty == 3)//中规模M
	{
		rowOfPath = 21;
		columnOfPath = 21;
	}
	else if(difficulty == 4)//大规模L
	{
		rowOfPath = 24;
		columnOfPath = 44;
	}
	else if(difficulty == 5)//超大规模XL
	{
		rowOfPath = 40;
		columnOfPath = 72;
	}
	else//自定义迷宫规模输入框
	{
		const char* difficultyName[3] = {"BFS", "Mix", "DFS"};
		char title[64];
		char text[256];
		char str[64];
		resizewindow(13*32, 10*32);
		sprintf(title, "Custom Maze Size Input Box - %s", difficultyName[summonMode]);
		sprintf(text, "[rowOfPath] [columnOfPath]\n"
			"Pay attention to the Space and press Enter after inputting. The maximum size is %d * %d.\n"
			"You can give Feedback about this ugly input box to https://github.com/wysaid/xege", LimRow, LimColumn);
		inputbox_getline(title, text, str, 64);
		sscanf(str, "%d%d", &rowOfPath, &columnOfPath);
		if(rowOfPath < 1) rowOfPath = 1;
		if(columnOfPath < 1) columnOfPath = 1;
		if(rowOfPath > LimRow) rowOfPath = LimRow;
		if(columnOfPath > LimColumn) columnOfPath = LimColumn;
	}
	rowOfMaze = 2*rowOfPath+1;
	columnOfMaze = 2*columnOfPath+1;
	// 游戏
	InitWindow(1);
	seed = time(0);
	SummonMaze(seed);
	rp = 1;
	cp = 0;
	while(1)
	{
		cleardevice();
		DrawMaze();
		DrawPath(rp, cp);
		while(mousemsg())
		{
			mouseMsg = getmouse();
			rm = mouseMsg.y / sideLength;
			cm = mouseMsg.x / sideLength;
			if(maze[rm][cm] == 1) AdjustCoord(mouseMsg.x, mouseMsg.y, &rm, &cm);
			if(mouseMsg.is_down())
			{
				MoveBackTo(rm, cm, &rp, &cp);//主路跳转仅在按下时尝试
				UpdateMainPath(rp, cp);
				isMoving = 1;
			}
			if(mouseMsg.is_up()) isMoving = 0;
			if(isMoving && (rm != rp || cm != cp))
			{
				MovePolyline(rm, cm, &rp, &cp);
				UpdateMainPath(rp, cp);
			}
			if(mouseMsg.is_wheel() && keystate(key_control))//调整显示大小
			{
				if(mouseMsg.wheel > 0)
				{
					if(sideLength >= 16) sideLength += 4;
					else sideLength++;
				}
				else
				{
					if(sideLength > 16) sideLength -= 4;
					else if(sideLength > 2) sideLength--;
				}
				resizewindow(columnOfMaze*sideLength, rowOfMaze*sideLength);
				setfont(sideLength, 0, "Consolas");
				DrawMaze();
				DrawPath(rp, cp);
			}
		}
		while(kbmsg())
		{
			keyMsg = getkey();
			if(keyMsg.msg == key_msg_down)
			{
				if(keyMsg.key == 'W' || keyMsg.key == 'A' || keyMsg.key == 'S' || keyMsg.key == 'D')
				{
					MoveTowards(keyMsg.key, &rp, &cp);
					UpdateMainPath(rp, cp);
				}
				else if(keyMsg.key >= key_left && keyMsg.key <= key_down)//方向键<^>v为0x25到0x28
				{
					MoveTowards("<^>v"[keyMsg.key-key_left], &rp, &cp);
					UpdateMainPath(rp, cp);
				}
				else if(keyMsg.key == 'B')
				{
					MoveBack(&rp, &cp);
					UpdateMainPath(rp, cp);
				}
				else if(keyMsg.key == 'R')//重置
				{
					if(seed != time(0))
					{
						seed = time(0);
						SummonMaze(seed);
						MoveTo(0, 0, &rp, &cp);
					}
				}
				else if(keyMsg.key == '\t')
				{
					Solve();
					rp = rowOfMaze-2;
					cp = columnOfMaze-1;
					UpdateMainPath(rp, cp);
				}
			}
		}
		delay_ms(50);
	}
	return 0;
}

/*--------------------------------
更新日志：
Maze Power 0.2
——新增 设置难度时的Ctrl+滚轮调整显示大小
——新增 键盘选择难度
——新增 通过鼠标点击在迷宫直线移动
——优化 大规模迷宫的默认显示大小
——优化 已访问路径显示连续的线条而不是圆圈
——优化 用圆形线帽代替双连拐点补小圆以简化代码
——优化 按Tab求解以已访问路径的形式显示解
——优化 按R重置不再能高频刷新
Maze Power 0.3
——新增 主路和支路以绿蓝区分
——新增 键盘移动时自动连续移动到岔路口
——新增 通过鼠标点击在迷宫折线移动
——优化 简化迷宫和路径的绘制代码
——优化 根据宏定义生成自定义迷宫规模输入框的提示文字
——修复 最大规模迷宫按Tab求解会产生左下开口
Maze Power 0.4
——新增 当前位置附近显示小段红线指示来时路
——优化 重构迷宫和路径绘制算法
——优化 仅在移动后计算主路
——修复 鼠标折线移动可能失效
Maze Power 0.5
——新增 键盘方向键也可移动
——新增 鼠标按下时连续触发移动
——优化 更精细的默认显示大小
——优化 重构迷宫求解算法
Maze Power 0.6
——新增 键盘按B沿主路返回上一个岔路口
——优化 对于墙格的点击拟合到对应空格的点击
Maze Power 0.7
——新增 重新设计难度体系（旧Hard为新Normal(Mix)，旧可选规模以XS,S,M,L称呼，新Hard难度）
——新增 通过鼠标点击沿主路返回指定位置
——优化 重构迷宫生成算法
——优化 迷宫规模上限由24*44提升到240*448
——优化 迷宫规模下限由1*2下降到1*1
Maze Power 1.0
——新增 XL规模
--------------------------------*/
