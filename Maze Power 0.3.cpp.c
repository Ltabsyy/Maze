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
#define LimRow 24
#define LimColumn 44

int maze[2*LimRow+1][2*LimColumn+1]={0};//含墙完整迷宫，0路，1墙，2已访问路
char visitDirection[2*LimRow+1][2*LimColumn+1]={0};//迷宫已访问路径，0未访问，><v^字符代表方向
char mainDirection[2*LimRow+1][2*LimColumn+1]={0};//已访问路径中从起点到当前位置的主路

int rowOfPath = 9;//迷宫路行数
int columnOfPath = 9;//迷宫路列数
int rowOfMaze = 2*9+1;
int columnOfMaze = 2*9+1;
int summonMode = 1;//0广度优先，1深度优先
int sideLength = 32;

void DrawSelection(int rp, int cp)
{
	int r, c;
	cleardevice();
	setfont(sideLength*5/2, 0, "Consolas");
	for(r=0; r<2; r++)
	{
		for(c=4; c<9; c++)
		{
			if(r == rp && c == cp) setfillcolor(GRAY);
			else setfillcolor(LIGHTGRAY);
			ege_fillrect(c*3*sideLength+sideLength/6, r*3*sideLength+sideLength/6, sideLength*8/3, sideLength*8/3);
		}
		setcolor(BLACK);
		xyprintf(4*3*sideLength+sideLength*7/8, r*3*sideLength+sideLength/4, "9");
		xyprintf(5*3*sideLength+sideLength/4+1, r*3*sideLength+sideLength/4, "15");//略右移以居中
		xyprintf(6*3*sideLength+sideLength/4+1, r*3*sideLength+sideLength/4, "21");
		xyprintf(7*3*sideLength+sideLength*7/8, r*3*sideLength+sideLength/4, "M");
		xyprintf(8*3*sideLength+sideLength*7/8, r*3*sideLength+sideLength/4, "C");
	}
	setcolor(LIGHTGRAY);
	xyprintf(sideLength*3/8, 0*sideLength+sideLength/4, "Easy(BFS)");
	xyprintf(sideLength*3/8, 3*sideLength+sideLength/4, "Hard(DFS)");
}

void DrawMaze()//绘制迷宫
{
	int r, c, link;
	setcolor(DARKGRAY);
	setfillcolor(LIGHTGRAY);
	setlinewidth(sideLength < 16 ? 2 : sideLength/16*2);//线宽为2起偶数避免模糊
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			//xyprintf(c*sideLength+sideLength/4, r*sideLength, "%d", maze[r][c]);
			if(maze[r][c] == 1)
			{
				link = 0;
				if(r > 0 && maze[r-1][c] == 1)
				{
					link |= 8;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength);
				}
				if(c+1 < columnOfMaze && maze[r][c+1] == 1)
				{
					link |= 4;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, (c+1)*sideLength, r*sideLength+sideLength/2);
				}
				if(r+1 < rowOfMaze && maze[r+1][c] == 1)
				{
					link |= 2;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, (r+1)*sideLength);
				}
				if(c > 0 && maze[r][c-1] == 1)
				{
					link |= 1;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength, r*sideLength+sideLength/2);
				}
				if(link == 8 || link == 4 || link == 2 || link == 1 || link == 0)//单连大圆
				{
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/3);
				}
			}
		}
	}
}

void DrawPath(int rp, int cp)//绘制路径
{
	int r, c, link;
	//计算主路
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
				//xyprintf(c*sideLength+sideLength/4, r*sideLength, "%c", visitDirection[r][c]);
				link = 0;
				if(r > 0 && visitDirection[r-1][c])//连接主路的支路仍需绘制
				{
					link |= 8;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength);
				}
				if(c+1 < columnOfMaze && visitDirection[r][c+1])
				{
					link |= 4;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, (c+1)*sideLength, r*sideLength+sideLength/2);
				}
				if(r+1 < rowOfMaze && visitDirection[r+1][c])
				{
					link |= 2;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, (r+1)*sideLength);
				}
				if(c > 0 && visitDirection[r][c-1])
				{
					link |= 1;
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength, r*sideLength+sideLength/2);
				}
				if(link == 8 || link == 4 || link == 2 || link == 1 || link == 0)//单连中圆
				{
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/4);
				}
			}
		}
	}
	//绘制支路与主路的连接部分
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(mainDirection[r][c])
			{
				if(r > 0 && visitDirection[r-1][c] && !mainDirection[r-1][c])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength);
				}
				if(c+1 < columnOfMaze && visitDirection[r][c+1] && !mainDirection[r][c+1])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, (c+1)*sideLength, r*sideLength+sideLength/2);
				}
				if(r+1 < rowOfMaze && visitDirection[r+1][c] && !mainDirection[r+1][c])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, (r+1)*sideLength);
				}
				if(c > 0 && visitDirection[r][c-1] && !mainDirection[r][c-1])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength, r*sideLength+sideLength/2);
				}
			}
		}
	}
	setcolor(GREEN);
	setfillcolor(LIME);
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(mainDirection[r][c])
			{
				//xyprintf(c*sideLength+sideLength/4, r*sideLength, "%c", mainDirection[r][c]);
				if(r > 0 && mainDirection[r-1][c])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, r*sideLength);
				}
				if(c+1 < columnOfMaze && mainDirection[r][c+1])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, (c+1)*sideLength, r*sideLength+sideLength/2);
				}
				if(r+1 < rowOfMaze && mainDirection[r+1][c])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength+sideLength/2, (r+1)*sideLength);
				}
				if(c > 0 && mainDirection[r][c-1])
				{
					ege_line(c*sideLength+sideLength/2, r*sideLength+sideLength/2, c*sideLength, r*sideLength+sideLength/2);
				}
			}
		}
	}
	ege_fillcircle(0*sideLength+sideLength/2, 1*sideLength+sideLength/2, sideLength/4);//主路的单连中圆仅在起点
	//当前位置绘制红色大圆
	setfillcolor(RED);
	ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3);
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
		initgraph(9*3*sideLength, 2*3*sideLength, INIT_RENDERMANUAL);
		setbkcolor(BLACK);
		//setfont(sideLength, 0, "Consolas");
		setbkmode(TRANSPARENT);//默认设置为无背景字体
		setlinecap(LINECAP_ROUND);//设置圆形线帽
		ege_enable_aa(true);
	}
	else
	{
		if(summonMode == 1) setcaption("Maze Power - DFS");
		else setcaption("Maze Power - BFS");
		while(columnOfMaze*sideLength > screenWidth || (rowOfMaze+4)*sideLength > screenHeight)
		{
			sideLength -= 4;
		}
		if(sideLength < 12) sideLength = 12;
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
			if(rt < rs)
			{
				r1 = rt;
				r2 = rs;
			}
			else
			{
				r1 = rs;
				r2 = rt;
			}
			if(ct < cs)
			{
				c1 = ct;
				c2 = cs;
			}
			else
			{
				c1 = cs;
				c2 = ct;
			}
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
			if(check == 12)//上右可移
			{
				if((rt == r1 && ct == c1) || (rt == r2 && ct == c2))//左上-右下
				{
					MoveTo(r1, c2, &rs, &cs);//移动到右上
					MoveTo(rt, ct, &rs, &cs);
				}
			}
			else if(check == 9)//上左
			{
				if((rt == r2 && ct == c1) || (rt == r1 && ct == c2))//左下-右上
				{
					MoveTo(r1, c1, &rs, &cs);//移动到左上
					MoveTo(rt, ct, &rs, &cs);
				}
			}
			else if(check == 6)//右下
			{
				if((rt == r2 && ct == c1) || (rt == r1 && ct == c2))//左下-右上
				{
					MoveTo(r2, c2, &rs, &cs);//移动到右下
					MoveTo(rt, ct, &rs, &cs);
				}
			}
			else if(check == 3)//下左
			{
				if((rt == r1 && ct == c1) || (rt == r2 && ct == c2))//左上-右下
				{
					MoveTo(r2, c1, &rs, &cs);//移动到左下
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

void SummonMaze(int seed)//自创随机遍历法生成迷宫
{
	int r, c;
	int row=rowOfPath, column=columnOfPath;//从终点开始，起点为(1,1)
	int randomNumber, remainder;
	char pushDirection;
	//初始化
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			maze[r][c] = 0;
			visitDirection[r][c] = 0;
			mainDirection[r][c] = 0;
			if(r == 0 || c == 0 || r == rowOfMaze-1 || c == columnOfMaze-1)
			{
				maze[r][c] = 1;
			}
			else if(r%2 == 0 || c%2 == 0)//填满，生成时挖
			{
				maze[r][c] = 1;
			}
		}
	}
	maze[1][0] = 0;
	maze[rowOfMaze-2][columnOfMaze-1] = 0;
	//生成
	srand(seed);
	maze[2*row-1][2*column-1] = 2;//标记当前点已访问
	remainder = rowOfPath*columnOfPath - 1;
	if(summonMode == 1)//深度优先
	{
		row = rowOfPath;
		column = columnOfPath;
		while(1)
		{
			while((column == columnOfPath || maze[2*row-1][2*column+1] == 2)
				  && (column == 1 || maze[2*row-1][2*column-3] == 2)
				  && (row == rowOfPath || maze[2*row+1][2*column-1] == 2)
				  && (row == 1 || maze[2*row-3][2*column-1] == 2))
			{
				//寻找已访问点
				while(1)
				{
					row = rand()%rowOfPath+1;
					column = rand()%columnOfPath+1;
					if(maze[2*row-1][2*column-1] == 2) break;//效率不高，不过也够用(doge)
				}
			}
			//决定挖开方向
			randomNumber = rand()%100;
			if(randomNumber < 25)//25%概率
			{
				pushDirection = '>';
			}
			else if(randomNumber < 50)
			{
				pushDirection = '<';
			}
			else if(randomNumber < 75)
			{
				pushDirection = 'v';
			}
			else
			{
				pushDirection = '^';
			}
			//尝试挖开
			if(pushDirection == '>')
			{
				if(column == columnOfPath) continue;//挖到边界
				if(maze[2*row-1][2*column+1] == 2) continue;//挖到已访问
				maze[2*row-1][2*column] = 0;//挖开
				maze[2*row-1][2*column+1] = 2;//标记挖开点已访问
				remainder--;
				column++;
			}
			else if(pushDirection == '<')
			{
				if(column == 1) continue;
				if(maze[2*row-1][2*column-3] == 2) continue;
				maze[2*row-1][2*column-2] = 0;
				maze[2*row-1][2*column-3] = 2;
				remainder--;
				column--;
			}
			else if(pushDirection == 'v')
			{
				if(row == rowOfPath) continue;
				if(maze[2*row+1][2*column-1] == 2) continue;
				maze[2*row][2*column-1] = 0;
				maze[2*row+1][2*column-1] = 2;
				remainder--;
				row++;
			}
			else if(pushDirection == '^')
			{
				if(row == 1) continue;
				if(maze[2*row-3][2*column-1] == 2) continue;
				maze[2*row-2][2*column-1] = 0;
				maze[2*row-3][2*column-1] = 2;
				remainder--;
				row--;
			}
			if(remainder == 0) break;
		}
	}
	else//广度优先
	{
		while(1)
		{
			//寻找已访问点
			while(1)
			{
				row = rand()%rowOfPath+1;
				column = rand()%columnOfPath+1;
				if(maze[2*row-1][2*column-1] == 2) break;//效率不高，不过也够用(doge)
			}
			//决定挖开方向
			randomNumber = rand()%100;
			if(randomNumber < 25)//25%概率
			{
				pushDirection = '>';
			}
			else if(randomNumber < 50)
			{
				pushDirection = '<';
			}
			else if(randomNumber < 75)
			{
				pushDirection = 'v';
			}
			else
			{
				pushDirection = '^';
			}
			//尝试挖开
			if(pushDirection == '>')
			{
				if(column == columnOfPath) continue;//挖到边界
				if(maze[2*row-1][2*column+1] == 2) continue;//挖到已访问
				maze[2*row-1][2*column] = 0;//挖开
				maze[2*row-1][2*column+1] = 2;//标记挖开点已访问
				remainder--;
			}
			else if(pushDirection == '<')
			{
				if(column == 1) continue;
				if(maze[2*row-1][2*column-3] == 2) continue;
				maze[2*row-1][2*column-2] = 0;
				maze[2*row-1][2*column-3] = 2;
				remainder--;
			}
			else if(pushDirection == 'v')
			{
				if(row == rowOfPath) continue;
				if(maze[2*row+1][2*column-1] == 2) continue;
				maze[2*row][2*column-1] = 0;
				maze[2*row+1][2*column-1] = 2;
				remainder--;
			}
			else if(pushDirection == '^')
			{
				if(row == 1) continue;
				if(maze[2*row-3][2*column-1] == 2) continue;
				maze[2*row-2][2*column-1] = 0;
				maze[2*row-3][2*column-1] = 2;
				remainder--;
			}
			//检查是否生成完毕
			if(remainder == 0) break;
		}
	}
	//删除已访问标记
	for(row=1; row<=rowOfPath; row++)
	{
		for(column=1; column<=columnOfPath; column++)
		{
			maze[2*row-1][2*column-1] = 0;
		}
	}
}

void Solve()//自创随机遍历法求解迷宫
{
	char direction[LimRow][LimColumn]={0};//迷宫路径遍历，><v^字符代表方向
	int row=rowOfPath, column=columnOfPath;//从终点开始
	int randomNumber;
	char moveDirection;//移动方向
	//初始化
	direction[row-1][column-1] = '>';
	maze[2*row-1][2*column-1] = 2;//标记当前点已访问
	while(1)
	{
		//寻找已访问点
		while(1)
		{
			row = rand()%rowOfPath+1;
			column = rand()%columnOfPath+1;
			if(maze[2*row-1][2*column-1] == 2) break;
		}
		//决定移动方向
		randomNumber = rand()%100;
		if(randomNumber < 25)//25%概率
		{
			moveDirection = '>';
		}
		else if(randomNumber < 50)
		{
			moveDirection = '<';
		}
		else if(randomNumber < 75)
		{
			moveDirection = 'v';
		}
		else
		{
			moveDirection = '^';
		}
		//尝试移动
		if(moveDirection == '>')
		{
			if(row == rowOfPath && column == columnOfPath) continue;//从终点出界
			if(maze[2*row-1][2*column] == 1) continue;//撞墙
			if(maze[2*row-1][2*column+1] == 2) continue;//移动到已访问
			direction[row-1][column] = '<';//使目标点指向当前点，direction起点(0,0)
			maze[2*row-1][2*column+1] = 2;//标记目标点已访问
		}
		if(moveDirection == '<')
		{
			if(maze[2*row-1][2*column-2] == 1) continue;
			if(maze[2*row-1][2*column-3] == 2) continue;
			direction[row-1][column-2] = '>';
			maze[2*row-1][2*column-3] = 2;
		}
		if(moveDirection == 'v')
		{
			if(maze[2*row][2*column-1] == 1) continue;
			if(maze[2*row+1][2*column-1] == 2) continue;
			direction[row][column-1] = '^';
			maze[2*row+1][2*column-1] = 2;
		}
		if(moveDirection == '^')
		{
			if(maze[2*row-2][2*column-1] == 1) continue;
			if(maze[2*row-3][2*column-1] == 2) continue;
			direction[row-2][column-1] = 'v';
			maze[2*row-3][2*column-1] = 2;
		}
		//已回起点退出
		if(maze[1][1] == 2) break;
	}
	//删除已访问标记
	for(row=1; row<=rowOfPath; row++)
	{
		for(column=1; column<=columnOfPath; column++)
		{
			maze[2*row-1][2*column-1] = 0;
		}
	}
	//输出唯一路径
	MoveTo(0, 0, NULL, NULL);
	MoveTo(1, 1, NULL, NULL);
	for(row=1, column=1; row!=rowOfPath||column!=columnOfPath; )
	{
		if(direction[row-1][column-1] == '>') column++;
		else if(direction[row-1][column-1] == '<') column--;
		else if(direction[row-1][column-1] == 'v') row++;
		else if(direction[row-1][column-1] == '^') row--;
		MoveTo(2*row-1, 2*column-1, NULL, NULL);
	}
	MoveTo(rowOfMaze-2, columnOfMaze-1, NULL, NULL);
}

int main()
{
	int difficulty = 0, seed;
	int rp, cp, rm, cm;
	mouse_msg mouseMsg;
	key_msg keyMsg;
	// 设置难度
	InitWindow(0);
	rp = -1;
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
				if(rp == 1) summonMode = 1;
				else summonMode = 0;
				if(cp >= 4 && cp <= 7) difficulty = cp-3;
				else difficulty = 5;
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
				resizewindow(9*3*sideLength, 2*3*sideLength);
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
				else if(keyMsg.key == 'H') rp = 1;
				else if(keyMsg.key == 'M') cp = 7;
				else if(keyMsg.key == 'C') cp = 8;
				if(rp < 0) rp = 0;
				if(cp < 4) cp = 4;
				if(rp > 1) rp = 1;
				if(cp > 8) cp = 8;
				DrawSelection(rp, cp);
			}
			if(keyMsg.msg == key_msg_up)//抬起时确定选择
			{
				if(keyMsg.key == '\r' || keyMsg.key == ' ' || keyMsg.key == 'M' || keyMsg.key == 'C')
				{
					summonMode = rp;
					difficulty = cp-3;
				}
			}
		}
		delay_ms(50);
	}
	if(difficulty == 1)
	{
		rowOfPath = 9;
		columnOfPath = 9;
	}
	else if(difficulty == 2)
	{
		rowOfPath = 15;
		columnOfPath = 15;
	}
	else if(difficulty == 3)
	{
		rowOfPath = 21;
		columnOfPath = 21;
	}
	else if(difficulty == 4)
	{
		rowOfPath = LimRow;
		columnOfPath = LimColumn;
	}
	else//自定义迷宫规模输入框
	{
		char title[64];
		char text[256];
		char str[64];
		resizewindow(13*32, 10*32);
		sprintf(title, "Custom Maze Size Input Box - %cFS", summonMode == 1 ? 'D' : 'B');
		sprintf(text, "[rowOfPath] [columnOfPath]\n"
			"Pay attention to the Space and press Enter after inputting. The maximum size is %d * %d.\n"
			"You can give Feedback about this ugly input box to https://github.com/wysaid/xege", LimRow, LimColumn);
		inputbox_getline(title, text, str, 64);
		sscanf(str, "%d%d", &rowOfPath, &columnOfPath);
		if(rowOfPath < 1) rowOfPath = 1;
		if(columnOfPath < 1) columnOfPath = 1;
		if(rowOfPath > LimRow) rowOfPath = LimRow;
		if(columnOfPath > LimColumn) columnOfPath = LimColumn;
		if(rowOfPath == 1 && columnOfPath == 1) columnOfPath = 2;
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
			if(mouseMsg.is_up())
			{
				MovePolyline(rm, cm, &rp, &cp);
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
--------------------------------*/
