//#define _CRT_SECURE_NO_WARNINGS//vs专属(doge)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>//时间戳作种子
#include <conio.h>
#include <windows.h>
/**
 * 迷宫 Maze
 * 
 * by Ltabsyy
 * 
 * ###########
 *  > > > > v#
 * # # ##### #
 * # # #    >
 * ###########
 * 
 * https://github.com/Ltabsyy/Maze
 **/
#define LimRow 24
#define LimColumn 89//#最大89，其他最大44

int maze[2*LimRow+1][2*LimColumn+1]={0};//含墙完整迷宫，0路，1墙

int rowOfPath = 9;//迷宫路行数
int columnOfPath = 9;//迷宫路列数
int rowOfMaze = 2*9+1;
int columnOfMaze = 2*9+1;
int summonMode = 1;//0广度优先，1深广混合，2深度优先
int showMode = 4;//0数字Debug，1#，2■，3纯空格，4线条

const char* linkString[16] = {
	"●",  "●",  "●",  "┐ ",
	"●",  "─ ", "┌ ", "┬ ",
	"●",  "┘ ", "│ ", "┤ ",
	"└ ", "┴ ", "├ ", "┼ "
};//线条显示模式的单格字符串

void gotoxy(int x, int y)//改变光标位置
{
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void showCursor(int visible)//显示或隐藏光标
{
	CONSOLE_CURSOR_INFO cursor_info = {20, visible};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
void ColorStr(const char* content, int color)//输出彩色字符
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	fputs(content, stdout);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), showMode == 3 ? 0xf0 : 0x07);
}

void ShowMaze()//显示迷宫
{
	int r, c, link;
	for(r=0; r<rowOfMaze; r++)
	{
		for(c=0; c<columnOfMaze; c++)
		{
			if(showMode == 0)
			{
				printf("%d ", maze[r][c]);
			}
			else if(showMode == 1)
			{
				if(maze[r][c] == 1) printf("#");
				else if(maze[r][c] == 0) printf(" ");
			}
			else if(showMode == 2)//■□
			{
				if(maze[r][c] == 1) printf("■");
				else if(maze[r][c] == 0) printf("  ");
			}
			else if(showMode == 3)
			{
				if(maze[r][c] == 1) ColorStr("  ", 0x0f);
				else if(maze[r][c] == 0) printf("  ");
			}
			else if(showMode == 4)
			{
				if(maze[r][c] == 0) printf("  ");
				else if(maze[r][c] == 1)
				{
					//计算连线形式，8421为上右下左的连接性
					link = 0;
					if(r > 0 && maze[r-1][c] == 1) link |= 8;
					if(c+1 < columnOfMaze && maze[r][c+1] == 1) link |= 4;
					if(r+1 < rowOfMaze && maze[r+1][c] == 1) link |= 2;
					if(c > 0 && maze[r][c-1] == 1) link |= 1;
					printf(linkString[link]);
				}
			}
		}
		printf("\n");
	}
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
	//生成唯一路径
	char path[LimRow][LimColumn]={0};//迷宫解
	for(r=0, c=0; !(r == rowOfPath-1 && c == columnOfPath-1); )
	{
		path[r][c] = direction[2*r+1][2*c+1];
		if(path[r][c] == '<') c--;
		else if(path[r][c] == '>') c++;
		else if(path[r][c] == '^') r--;
		else if(path[r][c] == 'v') r++;
	}
	//输出
	for(r=0; r<rowOfPath; r++)
	{
		for(c=0; c<columnOfPath; c++)
		{
			if(path[r][c] != 0)
			{
				if(showMode == 1) gotoxy(2*c+1, 2*r+1);
				else gotoxy(4*c+2, 2*r+1);
				//printf("%c", path[r][c]);
				if(showMode == 3)
				{
					ColorStr("  ", 0xc0);
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0e);
					if(showMode < 2)
					{
						printf("%c ", path[r][c]);
					}
					else
					{
						if(path[r][c] == '>') printf("→");
						else if(path[r][c] == '<') printf("←");
						else if(path[r][c] == 'v') printf("↓");
						else if(path[r][c] == '^') printf("↑");
					}
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
				}
			}
			//gotoxy(2*c+1, 2*r+1);
			//printf("%c", direction[2*r+1][2*c+1]);
		}
	}
}

int main()
{
	FILE* file;
	int choice, seed, rp, cp, lastMap = 0;
	int t0, t1;
	char key;
	if(file = fopen("maze-lastmap.txt", "r"))
	{
		lastMap = 1;
		fclose(file);
	}
	while(1)
	{
		if(showMode == 3) system("color f0");
		else system("color 07");
		system("cls");
		printf("*******************************\n"//宽31
			   "(1)新游戏\n"
			   "(2)设置\n"
			   "(3)退出\n"
			   "*******************************\n");
		if(lastMap == 1)
		{
			choice = 1;
		}
		else
		{
			printf(">");
			scanf("%d", &choice);
		}
		if(choice == 1)
		{
			if(lastMap == 1)
			{
				//printf("你还未解决上一次的迷宫，请继续(doge)\n");
				file = fopen("maze-lastmap.txt", "r");
				fscanf(file, "Map:%d*%d\n", &rowOfPath, &columnOfPath);
				fscanf(file, "summonMode=%d\n", &summonMode);
				fscanf(file, "seed=%d\n", &seed);
				fscanf(file, "pos=(%d,%d)\n", &rp, &cp);
				fclose(file);
				rowOfMaze = 2*rowOfPath+1;
				columnOfMaze = 2*columnOfPath+1;
				//system("pause");
			}
			else
			{
				seed = time(0);
				rp = 1;
				cp = 0;
			}
			SummonMaze(seed);
			system("cls");
			ShowMaze();
			printf("[wasd:移动/`:放弃/!:显示答案]");
			if(showMode == 1) gotoxy(cp, rp);
			else gotoxy(2*cp, rp);
			if(showMode > 1)
			{
				ColorStr("●\b\b", showMode == 3 ? 0xfc : 0x04);
				showCursor(0);
			}
			while(1)
			{
				if(kbhit())
				{
					key = getch();
					if(key == '`')
					{
						gotoxy(0, rowOfMaze+1);
						printf(":(\nYou Fail!\n");
						break;
					}
					if(key == '!')
					{
						//if(showMode > 1) printf("  ");//会导致左线泛蓝
						Solve();
						rp = rowOfMaze-2;
						cp = columnOfMaze-2;
					}
					if(key == 'w' || key == 'W')
					{
						if(maze[rp-1][cp] == 0) rp--;
					}
					if(key == 'a' || key == 'A')
					{
						if(rp == 1 && cp == 0)
						{
							gotoxy(0, rowOfMaze+1);
							printf(":)\n彳亍\n");
							break;
						}
						if(maze[rp][cp-1] == 0) cp--;
					}
					if(key == 's' || key == 'S')
					{
						if(maze[rp+1][cp] == 0) rp++;
					}
					if(key == 'd' || key == 'D')
					{
						if(maze[rp][cp+1] == 0) cp++;
					}
					if(showMode > 1) printf("  ");
					if(showMode == 1) gotoxy(cp, rp);
					else gotoxy(2*cp, rp);
					if(showMode > 1) ColorStr("●\b\b", showMode == 3 ? 0xfc : 0x04);
					file = fopen("maze-lastmap.txt", "w");
					fprintf(file, "Map:%d*%d\n", rowOfPath, columnOfPath);
					fprintf(file, "summonMode=%d\n", summonMode);
					fprintf(file, "seed=%d\n", seed);
					fprintf(file, "pos=(%d,%d)\n", rp, cp);
					fclose(file);
					if(rp==rowOfMaze-2 && cp==columnOfMaze-1)
					{
						if(showMode > 1) printf("  ");
						gotoxy(0, rowOfMaze+1);
						printf(":)\nYou Win!\n");
						break;
					}
				}
				Sleep(100);
			}
			remove("maze-lastmap.txt");
			lastMap = 0;
			showCursor(1);
			system("pause");
		}
		else if(choice == 2)
		{
			printf("*******************************\n");
			printf("(1)设置迷宫行列数 当前:%d*%d\n", rowOfPath, columnOfPath);
			printf("(2)设置生成模式 当前:");
			if(summonMode == 2) printf("深度优先\n");
			else if(summonMode == 1) printf("深广混合\n");
			else printf("广度优先\n");
			printf("(3)退出\n");
			printf("*******************************\n");
			printf(">");
			scanf("%d", &choice);
			if(choice == 1)
			{
				printf("*******************************\n");
				if(showMode == 1)
				{
					printf("(0)小规模：   5*18\n"
						   "(1)中规模：   8*30\n"
						   "(2)大规模：  11*42\n"
						   "(3)超大规模：14*55\n");
					//最大规模：24*89
				}
				else
				{
					printf("(0)小规模：   9*9\n"
						   "(1)中规模：  15*15\n"
						   "(2)大规模：  21*21\n"
						   "(3)最大规模：24*44\n");
				}
				printf("(4)自定义*********\n"
					   "*******************************\n");
				printf("当前迷宫行列数:%d*%d\n>", rowOfPath, columnOfPath);
				scanf("%d", &choice);
				if(choice == 0)
				{
					if(showMode == 1)
					{
						rowOfPath = 5;
						columnOfPath = 18;
					}
					else
					{
						rowOfPath = 9;
						columnOfPath = 9;
					}
				}
				else if(choice == 1)
				{
					if(showMode == 1)
					{
						rowOfPath = 8;
						columnOfPath = 30;
					}
					else
					{
						rowOfPath = 15;
						columnOfPath = 15;
					}
				}
				else if(choice == 2)
				{
					if(showMode == 1)
					{
						rowOfPath = 11;
						columnOfPath = 42;
					}
					else
					{
						rowOfPath = 21;
						columnOfPath = 21;
					}
				}
				else if(choice == 3)
				{
					if(showMode == 1)
					{
						rowOfPath = 14;
						columnOfPath = 55;
					}
					else
					{
						rowOfPath = 24;
						columnOfPath = 44;
					}
				}
				else
				{
					printf("[行数] [列数]>");
					scanf("%d%d", &rowOfPath, &columnOfPath);
					if(rowOfPath < 1) rowOfPath = 1;
					if(columnOfPath < 1) columnOfPath = 1;
					if(rowOfPath > LimRow) rowOfPath = LimRow;
					if(columnOfPath > LimColumn) columnOfPath = LimColumn;
				}
				rowOfMaze = 2*rowOfPath+1;
				columnOfMaze = 2*columnOfPath+1;
			}
			else if(choice == 2)
			{
				printf("*******************************\n");
				printf("(0)广度优先：简单\n");
				printf("(1)深广混合：普通\n");
				printf("(2)深度优先：困难\n");
				printf("*******************************\n");
				printf(">");
				scanf("%d", &summonMode);
			}
		}
		else if(choice == 3)
		{
			break;
		}
		else if(choice == 4)
		{
			printf("[迷宫显示模式：0数字Debug/1#/2■/3纯空格/4线条]>");
			scanf("%d", &showMode);
		}
		else if(choice == 5)//生成测试
		{
			t0 = time(0);
			for(seed=0; seed<10000; seed++)
			{
				SummonMaze(seed);
				if(seed % 16 == 0) printf("\rMaze:%d*%d-%d", rowOfMaze, columnOfMaze, seed);
				//ShowMaze();//注释前18秒，注释后0秒
			}
			t1 = time(0);
			printf("\n用时：%d\n", t1-t0);
			system("pause");
		}
		/*else if(choice == 6)//生成求解测试
		{
			t0 = time(0);
			for(seed=0; seed<10000; seed++)
			{
				SummonMaze(seed);
				Solve();
				if(seed % 16 == 0) printf("\rMaze:%d*%d-%d", rowOfMaze, columnOfMaze, seed);
				//ShowMaze();
			}
			t1 = time(0);
			printf("\n用时：%d\n", t1-t0);
			system("pause");
		}*/
		else
		{
			getchar();
		}
	}
	return 0;
}

/*--------------------------------
更新日志：
Maze 1.1
——新增 继续上一次游戏
——新增 现在可以从入口出去(doge)
——优化 现在地图不能为1*1
Maze 1.2
——优化 地图采用■表示
——优化 设置迷宫规模预设为正方形
Maze 1.3
——新增 主页可按4切换迷宫显示模式
Maze 1.4
——优化 地图采用线条表示
——优化 现在上一次游戏直接在程序启动时打开
——优化 答案采用箭头表示
——优化 缩小最大列数以适配16:10屏
Maze 1.5
——新增 深度优先迷宫生成
——优化 迷宫生成效率
Maze 1.6
——优化 线条显示代码
——优化 主页和设置
Maze 1.7
——新增 当前位置显示红色●
——优化 仅在移动时更新游戏状态
Maze 1.8
——优化 重构线条显示代码
——优化 生成和求解算法跟随Maze Power 1.0升级
--------------------------------*/
