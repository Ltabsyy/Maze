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
 **/
#define LimRow 24
#define LimColumn 94

int maze[2*LimRow+1][2*LimColumn+1]={0};
char direction[LimRow][LimColumn]={0};
char path[LimRow][LimColumn]={0};

int rowOfMaze = 5;
int columnOfMaze = 18;

void gotoxy(int x, int y)
{
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ShowMaze()
{
	int r, c;
	for(r=0; r<2*rowOfMaze+1; r++)
	{
		for(c=0; c<2*columnOfMaze+1; c++)
		{
			if(maze[r][c] == 1)
			{
				printf("#");
			}
			else if(maze[r][c] == 0)
			{
				printf(" ");
			}
			//printf("%d", maze[r][c]);
		}
		printf("\n");
	}
}

void SummonMaze(int seed)
{
	int r, c;
	int row=rowOfMaze, column=columnOfMaze;//从终点开始，起点为(1,1)
	int randomNumber, isEnd;
	char pushDirection;
	//初始化
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
	while(1)
	{
		//寻找已访问点
		while(1)
		{
			row = rand()%rowOfMaze+1;
			column = rand()%columnOfMaze+1;
			if(maze[2*row-1][2*column-1] == 2) break;
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
		}
		if(pushDirection == '<')
		{
			if(column == 1) continue;
			if(maze[2*row-1][2*column-3] == 2) continue;
			maze[2*row-1][2*column-2] = 0;
			maze[2*row-1][2*column-3] = 2;
		}
		if(pushDirection == 'v')
		{
			if(row == rowOfMaze) continue;
			if(maze[2*row+1][2*column-1] == 2) continue;
			maze[2*row][2*column-1] = 0;
			maze[2*row+1][2*column-1] = 2;
		}
		if(pushDirection == '^')
		{
			if(row == 1) continue;
			if(maze[2*row-3][2*column-1] == 2) continue;
			maze[2*row-2][2*column-1] = 0;
			maze[2*row-3][2*column-1] = 2;
		}
		//检查是否生成完毕
		isEnd = 1;
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
		if(isEnd == 1) break;
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

void Solve()
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
				gotoxy(2*column-1, 2*row-1);
				//printf("%c", path[row-1][column-1]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0e);
				printf("%c", path[row-1][column-1]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
			}
			/*gotoxy(2*column-1, 2*row-1);
			printf("%c", direction[row-1][column-1]);*/
		}
	}
}

int main()
{
	int choice, rp, cp;
	char key;
	while(1)
	{
		system("cls");
		printf("*******************************\n"
			   "**新游戏(1)**设置(2)**退出(3)**\n"
			   "*******************************\n");
		printf(">");
		scanf("%d", &choice);
		if(choice == 1)
		{
			rp = 1;
			cp = 0;
			SummonMaze(time(0));
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
					gotoxy(cp, rp);
					Sleep(100);
					if(rp==2*rowOfMaze-1 && cp==2*columnOfMaze)
					{
						gotoxy(0, 2*rowOfMaze+2);
						printf(":)\nYou Win!\n");
						system("pause");
						break;
					}
				}
			}
		}
		else if(choice == 2)
		{
			printf("*******************************\n"
				   "(0)小规模：   5*18\n"
				   "(1)中规模：   8*30\n"
				   "(2)大规模：  11*42\n"
				   "(3)超大规模：14*55\n"
				   "(4)自定义*********\n"
				   "*******************************\n");
			printf("当前迷宫行列数:%d*%d\n>", rowOfMaze, columnOfMaze);
			scanf("%d", &choice);
			if(choice == 0)
			{
				rowOfMaze = 5;
				columnOfMaze = 18;
			}
			else if(choice == 1)
			{
				rowOfMaze = 8;
				columnOfMaze = 30;
			}
			else if(choice == 2)
			{
				rowOfMaze = 11;
				columnOfMaze = 42;
			}
			else if(choice == 3)
			{
				rowOfMaze = 14;
				columnOfMaze = 55;
			}
			else
			{
				printf("[行数] [列数]>");
				scanf("%d%d", &rowOfMaze, &columnOfMaze);
				if(rowOfMaze < 1) rowOfMaze = 1;
				if(columnOfMaze < 1) columnOfMaze = 1;
				if(rowOfMaze > LimRow) rowOfMaze = LimRow;
				if(columnOfMaze > LimColumn) columnOfMaze = LimColumn;
			}
			
		}
		else if(choice == 3)
		{
			break;
		}
		else
		{
			getchar();
		}
	}
	return 0;
}


