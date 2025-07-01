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

int maze[2*LimRow+1][2*LimColumn+1]={0};//存储含墙完整迷宫，0路，1墙，2已访问路
char direction[LimRow][LimColumn]={0};//迷宫路径遍历，><v^字符代表方向
char path[LimRow][LimColumn]={0};//迷宫解

int rowOfPath = 9;//迷宫路行数
int columnOfPath = 9;//迷宫路列数
int rowOfMaze = 2*9+1;
int columnOfMaze = 2*9+1;
int summonMode = 1;//0广度优先，1深度优先
int sideLength = 32;

void DrawSelection(int rm, int cm)
{
	int r, c;
	cleardevice();
	setfont(sideLength*5/2, 0, "Consolas");
	for(r=0; r<2; r++)
	{
		for(c=4; c<9; c++)
		{
			if(r == rm && c == cm) setfillcolor(GRAY);
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

void DrawMaze()
{
	int r, c;
	setcolor(DARKGRAY);
	setfillcolor(DARKGRAY);
	setlinewidth(sideLength < 16 ? 2 : sideLength/16*2);//线宽为2起偶数避免模糊
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			//xyprintf(c*sideLength, r*sideLength, "%d", maze[r][c]);
			if(maze[r][c] == 0);
			else if(maze[r][c] == 1)
			{
				int link = 0;
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
					setfillcolor(LIGHTGRAY);
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/3);
					setfillcolor(DARKGRAY);
				}
				if(link == 12 || link == 9 || link == 6 || link == 3)//双连拐点补小圆
				{
					ege_fillcircle(c*sideLength+sideLength/2, r*sideLength+sideLength/2, sideLength/16);
				}
			}
		}
	}
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
		setfont(sideLength, 0, "Consolas");
		setbkmode(TRANSPARENT);//默认设置为无背景字体
		ege_enable_aa(true);
	}
	else
	{
		if(summonMode == 1) setcaption("Maze Power - DFS");
		else setcaption("Maze Power - BFS");
		while(columnOfMaze*sideLength > screenWidth || (rowOfMaze+2)*sideLength > screenHeight)
		{
			sideLength -= 4;
		}
		if(sideLength < 12) sideLength = 12;
		resizewindow(columnOfMaze*sideLength, rowOfMaze*sideLength);
		setfont(sideLength, 0, "Consolas");
	}
}

void SummonMaze(int seed)//自创随机遍历法生成迷宫
{
	int r, c;
	int row=rowOfPath, column=columnOfPath;//从终点开始，起点为(1,1)
	int randomNumber, isEnd, remainder;
	char pushDirection;
	//初始化
	//seed = 0;
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			maze[r][c] = 0;
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
	int row=rowOfPath, column=columnOfPath;//从终点开始
	int randomNumber;
	char moveDirection;//移动方向
	//初始化
	for(row=0; row<rowOfPath; row++)
	{
		for(column=0; column<columnOfPath; column++)
		{
			direction[row][column] = 0;
			path[row][column] = 0;
		}
	}
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
	//生成唯一路径
	for(row=1, column=1; row!=rowOfPath||column!=columnOfPath; )
	{
		path[row-1][column-1] = direction[row-1][column-1];
		if(direction[row-1][column-1] == '>') column++;
		else if(direction[row-1][column-1] == '<') column--;
		else if(direction[row-1][column-1] == 'v') row++;
		else if(direction[row-1][column-1] == '^') row--;
	}
	path[row-1][column-1] = '>';
	//输出
	for(row=1; row<=rowOfPath; row++)
	{
		for(column=1; column<=columnOfPath; column++)
		{
			if(path[row-1][column-1] != 0)
			{
				//setcolor(YELLOW);
				//xyprintf((2*column-1)*sideLength+sideLength/4, (2*row-1)*sideLength, "%c", path[row-1][column-1]);
				setfillcolor(YELLOW);
				ege_fillcircle((2*column-1)*sideLength+sideLength/2, (2*row-1)*sideLength+sideLength/2, sideLength/2);
			}
		}
	}
}

int main()
{
	int difficulty = 0;
	int r, c, rp, cp;
	mouse_msg mouseMsg;
	key_msg keyMsg;
	// 设置难度
	InitWindow(0);
	r = -1;
	c = -1;
	while(difficulty == 0)
	{
		DrawSelection(r, c);
		while(mousemsg())
		{
			mouseMsg = getmouse();
			r = mouseMsg.y / (3*sideLength);
			c = mouseMsg.x / (3*sideLength);
			if(mouseMsg.is_up())//选择难度
			{
				if(r == 1) summonMode = 1;
				else summonMode = 0;
				if(c >= 4 && c <= 7) difficulty = c-3;
				else difficulty = 5;
			}
			//if(mouseMsg.is_wheel() && keystate(key_control));//调整显示大小
		}
		//while(kbmsg());
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
	else
	{
		char str[64];
		resizewindow(13*32, 10*32);
		inputbox_getline(summonMode == 1 ? "Custom Maze Size Input Box - DFS" : "Custom Maze Size Input Box - BFS",
			"[rowOfPath] [columnOfPath]\n"
			"Pay attention to the Space and press Enter after inputting. The maximum size is 24 * 44.\n"
			"You can give Feedback about this ugly input box to https://github.com/wysaid/xege", str, 64);
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
	SummonMaze(time(0));
	DrawMaze();
	rp = 1;
	cp = 0;
	setfillcolor(RED);
	ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3);
	while(1)
	{
		while(mousemsg())
		{
			mouseMsg = getmouse();
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
				setfillcolor(RED);
				ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3);
			}
		}
		while(kbmsg())
		{
			keyMsg = getkey();
			if(keyMsg.msg == key_msg_down)
			{
				setfillcolor(BLACK);
				ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3+1);
				setfillcolor(GREEN);
				ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/4);
				if(keyMsg.key == 'W')
				{
					if(maze[rp-1][cp] == 0) rp--;
				}
				else if(keyMsg.key == 'A')
				{
					if(rp == 1 && cp == 0);
					else if(maze[rp][cp-1] == 0) cp--;
				}
				else if(keyMsg.key == 'S')
				{
					if(maze[rp+1][cp] == 0) rp++;
				}
				else if(keyMsg.key == 'D')
				{
					if(rp == rowOfMaze-2 && cp == columnOfMaze-1)
					{
						Solve();
					}
					else if(maze[rp][cp+1] == 0) cp++;
				}
				else if(keyMsg.key == 'R')
				{
					SummonMaze(time(0));
					cleardevice();
					DrawMaze();
					rp = 1;
					cp = 0;
				}
				else if(keyMsg.key == '\t')
				{
					Solve();
					rp = rowOfMaze-2;
					cp = columnOfMaze-1;
				}
				setfillcolor(RED);
				ege_fillcircle(cp*sideLength+sideLength/2, rp*sideLength+sideLength/2, sideLength/3);
			}
		}
		delay_ms(50);
	}
	return 0;
}
