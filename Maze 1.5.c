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

int maze[2*LimRow+1][2*LimColumn+1]={0};//存储含墙完整迷宫，0路，1墙，2已访问路
char direction[LimRow][LimColumn]={0};//迷宫路径遍历，><v^字符代表方向
char path[LimRow][LimColumn]={0};//迷宫解

int rowOfMaze = 9;//迷宫路行数
int columnOfMaze = 9;//迷宫路列数
int summonMode = 0;//0广度优先，1深度优先
int showMode = 4;//0数字Debug，1#，2■，3纯空格，4线条

void gotoxy(int x, int y)//改变光标位置
{
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int WallAroundIs(int r, int c, int distance, int right, int left, int down, int up)
{
	if(r-distance >= 0 && r+distance <= 2*rowOfMaze
		&& c-distance >= 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && maze[r][c-distance] == left
			&& maze[r+distance][c] == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	//边墙
	if(r-distance >= 0 && r+distance <= 2*rowOfMaze
		&& c-distance >= 0 && c+distance > 2*columnOfMaze)
	{
		if(0 == right && maze[r][c-distance] == left
			&& maze[r+distance][c] == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance >= 0 && r+distance <= 2*rowOfMaze
		&& c-distance < 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && 0 == left
			&& maze[r+distance][c] == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance >= 0 && r+distance > 2*rowOfMaze
		&& c-distance >= 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && maze[r][c-distance] == left
			&& 0 == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance < 0 && r+distance <= 2*rowOfMaze
		&& c-distance >= 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && maze[r][c-distance] == left
			&& maze[r+distance][c] == down && 0 == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	//角墙
	if(r-distance >= 0 && r+distance > 2*rowOfMaze
		&& c-distance >= 0 && c+distance > 2*columnOfMaze)
	{
		if(0 == right && maze[r][c-distance] == left
			&& 0 == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance < 0 && r+distance <= 2*rowOfMaze
		&& c-distance >= 0 && c+distance > 2*columnOfMaze)
	{
		if(0 == right && maze[r][c-distance] == left
			&& maze[r+distance][c] == down && 0 == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance >= 0 && r+distance > 2*rowOfMaze
		&& c-distance < 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && 0 == left
			&& 0 == down && maze[r-distance][c] == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(r-distance < 0 && r+distance <= 2*rowOfMaze
		&& c-distance < 0 && c+distance <= 2*columnOfMaze)
	{
		if(maze[r][c+distance] == right && 0 == left
			&& maze[r+distance][c] == down && 0 == up)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

void ShowMazeLine(int r, int c)
{
	if(maze[r][c] == 0)
	{
		printf("  ");
	}
	//─━│┃┌┍┎┏┐┑┒┓└┕┖┗┘┙┚┛├┝┞┟┠┡┢┣┤┥┦┧┨┩┪┫┬┭┮┯┰┱┲┳┴┵┶┷┸┹┶┻┺┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋
	else if(maze[r][c] == 1)
	{
		//单连-
		/*if(WallAroundIs(r, c, 1, 0, 1, 0, 0))
		{
			printf("- ");
		}
		else if(WallAroundIs(r, c, 1, 1, 0, 0, 0))
		{
			printf(" -");
		}
		//双连━┃┏┛┗┓
		else */if(WallAroundIs(r, c, 1, 0, 0, 1, 1))
		{
			printf("│ ");
		}
		else if(WallAroundIs(r, c, 1, 0, 1, 0, 1))
		{
			printf("┘ ");
		}
		else if(WallAroundIs(r, c, 1, 0, 1, 1, 0))
		{
			printf("┐ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 0, 0, 1))
		{
			printf("└ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 0, 1, 0))
		{
			printf("┌ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 1, 0, 0))
		{
			printf("─ ");
		}
		//三连┫┻┣┳
		else if(WallAroundIs(r, c, 1, 0, 1, 1, 1))
		{
			printf("┤ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 0, 1, 1))
		{
			printf("├ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 1, 0, 1))
		{
			printf("┴ ");
		}
		else if(WallAroundIs(r, c, 1, 1, 1, 1, 0))
		{
			printf("┬ ");
		}
		//四连╋
		else if(WallAroundIs(r, c, 1, 1, 1, 1, 1))
		{
			printf("┼ ");
		}
		else
		{
			printf("●");//单连■●
		}
	}
}

void ShowMaze()//显示迷宫
{
	int r, c;
	for(r=0; r<2*rowOfMaze+1; r++)
	{
		for(c=0; c<2*columnOfMaze+1; c++)
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
				if(maze[r][c] == 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0f);
					printf("  ");
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf0);
				}
				else if(maze[r][c] == 0)
				{
					printf("  ");
				}
			}
			else if(showMode == 4)
			{
				ShowMazeLine(r, c);
			}
		}
		printf("\n");
	}
}

void SummonMaze(int seed)//自创随机遍历法生成迷宫
{
	int r, c;
	int row=rowOfMaze, column=columnOfMaze;//从终点开始，起点为(1,1)
	int randomNumber, isEnd, remainder;
	char pushDirection;
	//初始化
	//seed = 0;
	for(r=0; r<2*rowOfMaze+1; r++)
	{
		for(c=0; c<2*columnOfMaze+1; c++)
		{
			maze[r][c] = 0;
			if(r == 0 || c == 0 || r == 2*rowOfMaze || c == 2*columnOfMaze)
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
	maze[2*rowOfMaze-1][2*columnOfMaze] = 0;
	//生成
	srand(seed);
	maze[2*row-1][2*column-1] = 2;//标记当前点已访问
	remainder = rowOfMaze*columnOfMaze - 1;
	if(summonMode == 1)//深度优先
	{
		row = rowOfMaze;
		column = columnOfMaze;
		while(1)
		{
			while((column == columnOfMaze || maze[2*row-1][2*column+1] == 2)
				&& (column == 1 || maze[2*row-1][2*column-3] == 2)
				&& (row == rowOfMaze || maze[2*row+1][2*column-1] == 2)
				&& (row == 1 || maze[2*row-3][2*column-1] == 2))
			{
				//寻找已访问点
				while(1)
				{
					row = rand()%rowOfMaze+1;
					column = rand()%columnOfMaze+1;
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
				if(column == columnOfMaze) continue;//挖到边界
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
				if(row == rowOfMaze) continue;
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
				row = rand()%rowOfMaze+1;
				column = rand()%columnOfMaze+1;
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
				if(column == columnOfMaze) continue;//挖到边界
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
				if(row == rowOfMaze) continue;
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
			/*isEnd = 1;
			for(row=1; row<=rowOfMaze; row++)
			{
				for(column=1; column<=columnOfMaze; column++)
				{
					if(maze[2*row-1][2*column-1] == 0)
					{
						isEnd = 0;
					}
				}
			}
			if(isEnd == 1) break;*/
			if(remainder == 0) break;
		}
	}
	//删除已访问标记
	for(row=1; row<=rowOfMaze; row++)
	{
		for(column=1; column<=columnOfMaze; column++)
		{
			maze[2*row-1][2*column-1] = 0;
		}
	}
}

void Solve()//自创随机遍历法求解迷宫
{
	int row=rowOfMaze, column=columnOfMaze;//从终点开始
	int randomNumber;
	char moveDirection;//移动方向
	//初始化
	for(row=0; row<rowOfMaze; row++)
	{
		for(column=0; column<columnOfMaze; column++)
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
			row = rand()%rowOfMaze+1;
			column = rand()%columnOfMaze+1;
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
	for(row=1; row<=rowOfMaze; row++)
	{
		for(column=1; column<=columnOfMaze; column++)
		{
			maze[2*row-1][2*column-1] = 0;
		}
	}
	//生成唯一路径
	for(row=1, column=1; row!=rowOfMaze||column!=columnOfMaze; )
	{
		path[row-1][column-1] = direction[row-1][column-1];
		if(direction[row-1][column-1] == '>') column++;
		else if(direction[row-1][column-1] == '<') column--;
		else if(direction[row-1][column-1] == 'v') row++;
		else if(direction[row-1][column-1] == '^') row--;
	}
	path[row-1][column-1] = '>';
	//输出
	for(row=1; row<=rowOfMaze; row++)
	{
		for(column=1; column<=columnOfMaze; column++)
		{
			if(path[row-1][column-1] != 0)
			{
				if(showMode == 1) gotoxy(2*column-1, 2*row-1);
				else gotoxy(4*column-2, 2*row-1);
				//printf("%c", path[row-1][column-1]);
				if(showMode == 3)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xc0);
					printf("  ");
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf0);
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0e);
					if(showMode < 2)
					{
						printf("%c ", path[row-1][column-1]);
					}
					else
					{
						if(path[row-1][column-1] == '>') printf("→");
						else if(path[row-1][column-1] == '<') printf("←");
						else if(path[row-1][column-1] == 'v') printf("↓");
						else if(path[row-1][column-1] == '^') printf("↑");
					}
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
				}
			}
			/*gotoxy(2*column-1, 2*row-1);
			printf("%c", direction[row-1][column-1]);*/
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
		printf("*******************************\n"
			   "**新游戏(1)**设置(2)**退出(3)**\n"
			   "*******************************\n");//宽31
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
				fscanf(file, "Map:%d*%d\n", &rowOfMaze, &columnOfMaze);
				fscanf(file, "summonMode=%d\n", &summonMode);
				fscanf(file, "seed=%d\n", &seed);
				fscanf(file, "pos=(%d,%d)\n", &rp, &cp);
				fclose(file);
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
			while(1)
			{
				if(kbhit())
				{
					key = getch();
					if(key == '`')
					{
						gotoxy(0, 2*rowOfMaze+2);
						printf(":(\nYou Fail!\n");
						remove("maze-lastmap.txt");
						lastMap = 0;
						system("pause");
						break;
					}
					if(key == '!')
					{
						Solve();
						rp = 2*rowOfMaze-1;
						cp = 2*columnOfMaze-1;
					}
					if(key == 'w' || key == 'W')
					{
						if(maze[rp-1][cp] == 0) rp--;
					}
					if(key == 'a' || key == 'A')
					{
						if(rp == 1 && cp == 0)
						{
							gotoxy(0, 2*rowOfMaze+2);
							printf(":)\n彳亍\n");
							remove("maze-lastmap.txt");
							lastMap = 0;
							system("pause");
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
				}
				else
				{
					if(showMode == 1) gotoxy(cp, rp);
					else gotoxy(2*cp, rp);
					file = fopen("maze-lastmap.txt", "w");
					fprintf(file, "Map:%d*%d\n", rowOfMaze, columnOfMaze);
					fprintf(file, "summonMode=%d\n", summonMode);
					fprintf(file, "seed=%d\n", seed);
					fprintf(file, "pos=(%d,%d)\n", rp, cp);
					fclose(file);
					Sleep(100);
					if(rp==2*rowOfMaze-1 && cp==2*columnOfMaze)
					{
						gotoxy(0, 2*rowOfMaze+2);
						printf(":)\nYou Win!\n");
						remove("maze-lastmap.txt");
						lastMap = 0;
						system("pause");
						break;
					}
				}
			}
		}
		else if(choice == 2)
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
			printf("当前迷宫行列数:%d*%d\n>", rowOfMaze, columnOfMaze);
			scanf("%d", &choice);
			if(choice == 0)
			{
				if(showMode == 1)
				{
					rowOfMaze = 5;
					columnOfMaze = 18;
				}
				else
				{
					rowOfMaze = 9;
					columnOfMaze = 9;
				}
			}
			else if(choice == 1)
			{
				if(showMode == 1)
				{
					rowOfMaze = 8;
					columnOfMaze = 30;
				}
				else
				{
					rowOfMaze = 15;
					columnOfMaze = 15;
				}
			}
			else if(choice == 2)
			{
				if(showMode == 1)
				{
					rowOfMaze = 11;
					columnOfMaze = 42;
				}
				else
				{
					rowOfMaze = 21;
					columnOfMaze = 21;
				}
			}
			else if(choice == 3)
			{
				if(showMode == 1)
				{
					rowOfMaze = 14;
					columnOfMaze = 55;
				}
				else
				{
					rowOfMaze = 24;
					columnOfMaze = 44;
				}
			}
			else
			{
				printf("[行数] [列数]>");
				scanf("%d%d", &rowOfMaze, &columnOfMaze);
				if(rowOfMaze < 1) rowOfMaze = 1;
				if(columnOfMaze < 1) columnOfMaze = 1;
				if(rowOfMaze > LimRow) rowOfMaze = LimRow;
				if(columnOfMaze > LimColumn) columnOfMaze = LimColumn;
				if(rowOfMaze == 1 && columnOfMaze == 1) columnOfMaze = 2;
			}
			
		}
		else if(choice == 3)
		{
			break;
		}
		else if(choice == 4)
		{
			printf("[迷宫生成模式：0广度优先/1深度优先]>");
			scanf("%d", &summonMode);
		}
		else if(choice == 5)
		{
			printf("[迷宫显示模式：0数字Debug/1#/2■/3纯空格/4线条]>");
			scanf("%d", &showMode);
		}
		else if(choice == 6)
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
--------------------------------*/
