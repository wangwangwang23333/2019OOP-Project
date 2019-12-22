#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//更新日志
//11.19 22:00 删除示例代码，开始写啦
//11.20 12:00 完成了移动函数
//11.20 18:00 写了最基础最垃圾的走子功能	
//11.21 10:00 好难，吃子好难
//11.21 21:00 完成吃子函数
//11.22 23:00 开始重新写走子功能
//11.23 10:30 检查吃子功能
//11.23 17:00 完善大部分函数的参数
//11.24 10:30 测试通过走子函数
//11.25 19:00 部分完成子升王
//11.26 12:00 解决一个深坑bug
//11.26 13:30 开始写局面评估函数
//11.27 14:00 完成深度搜索
//11.27 16:30 我的深度搜索怎么这么多bug嘤嘤嘤
//11.28 19:00 终于有点脑子了嘤嘤嘤
//11.28 19:36 第一次打赢Draughts Medium难度打卡
//11.30 13:30 简化了最大最小值寻找部分代码耶耶耶
//12.01 09:00 改了一个小小的bug
//12.01 18:00 完成Alpha-Beta剪枝算法啦
//12.05 23:00 前两步固定走法+修改一个bug
//12.10 重新写的代码终于可以正常跑了
//12.11 升王棋bug找出来了√
//12.11 残局的bug√
//12.12 调参->边界参数

//搜索层数
#define LAYER_FIND 8

//定义BOOL型变量
#define BOOL int
#define TRUE 1
#define FALSE 0

//棋盘基本信息
#define LENGTH 8
#define EMPTY 0
#define BLACK 1
#define WHITE 2

//棋子信息
#define king 1
#define solider 0
#define alive 1
#define dead 0

//棋盘
int board_id[LENGTH][LENGTH];//编号,正号表示自己的，负号表示对方的

//执子颜色
int my_color;
int other_color;

//轮数
int round;

//当前走子方
int flag;

//浅层搜索
int DepthSoft = 0;

//每一个棋子的信息
struct chess
{
	int x;
	int y;
	int identify;
	int life;
};
struct chess mychess[12], otherchess[12];

//接收棋子移动信息的结构体
#define MAX_STEP 15
struct Command
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
};
struct Command command = {
		.x = {0},
		.y = {0},
		.numStep = 0 };
//记录吃子情况
//struct Command eating[12];

//函数
void start();//初始化棋盘
void printBoard(const int board_ids[][LENGTH]);//显示当前局面
void place(const struct Command, int board_ids[][LENGTH], struct chess *mychess, struct chess *otherchess);//移动棋子
BOOL isInBound(int x, int y);//判断是否越界
void eat(int board_ids[][LENGTH], int x, int y, int number, int layer, struct Command *eating);//判断第num个棋子是否可以吃子
BOOL ToKing(int x, int y);//判断该坐标的棋子是否能变为王棋
int move_AI(int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses, int layer, int cutting_line);//主要逻辑
int estimate(const int board_ids[][LENGTH], const struct chess *mychesses, const struct chess *otherchesses);//局面评估函数

int main()
{

	char action[6];//用于接收指令的变量

	//开始
	scanf("%*s %d", &my_color);
	//对方的颜色
	other_color = (my_color == BLACK) ? WHITE : BLACK;

	//回复系统
	printf("OK\n");
	fflush(stdout);

	start();//初始化棋盘

	//循环读取指令
	while (scanf("%s", action) == 1)
	{
		if (!strcmp(action, "END"))
		{
			break;
		}
		else if (!strcmp(action, "PLACE"))
		{
			scanf("%d", &command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				scanf("%d,%d", &command.x[i], &command.y[i]);
			}
			//移动棋子
			place(command, board_id, mychess, otherchess);
			//走子结束，判断是否升为王棋
			flag = -flag;//升王棋函数中涉及flag
			if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
			{
				otherchess[-board_id[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
			}
			flag = -flag;//变回来
			//printBoard(board_id);
			round++;
		}
		else if (!strcmp(action, "TURN"))
		{
			int board_id_image[LENGTH][LENGTH];
			memcpy(board_id_image, board_id, sizeof(int)*LENGTH*LENGTH);
			struct chess mychess_image[12];
			struct chess otherchess_image[12];
			memcpy(mychess_image, mychess, sizeof(struct chess) * 12);
			memcpy(otherchess_image, otherchess, sizeof(struct chess) * 12);
			//判断如何走棋
			command.numStep = 0;//初始化为0
			move_AI(board_id_image, mychess_image, otherchess_image, 0, 999999);
			//此时我方走子信息已经被储存在command结构体中
			place(command, board_id, mychess, otherchess);
			//要把走子方式输出
			printf("%d ", command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				printf("%d,%d", command.x[i], command.y[i]);
				if (i != command.numStep - 1)
					printf(" ");
				else
					printf("\n");
			}
			//走子结束，判断其是否升为王棋
			if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
			{
				mychess[board_id[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
			}
			fflush(stdout);
			round++;
		}
		else if (!strcmp(action, "PRINT"))
		{
			printBoard(board_id);
		}
		else if (!strcmp(action, "FIND"))
		{
			printf("Please input the number of the chess you want to find:\n(1~12 for your chess && -1~-12 for the enemy's chess)\n");
			int temp;
			scanf("%d", &temp);
			if (temp > 0 && temp < 13)
			{
				temp--;
				printf("%d chess is your chess", temp + 1);
				if (mychess[temp].life == dead)
					printf(",which has been dead\n");
				else
				{
					printf(",the identity now is ");
					if (mychess[temp].identify == solider)
					{
						printf("[solider]");
					}
					else
					{
						printf("[king]");
					}
					printf(",whose place is (%d,%d)\n", mychess[temp].x, mychess[temp].y);
				}
			}
			else if (temp < 0 && temp > -13)
			{
				temp = -temp;
				temp--;
				printf("%d chess is the enemy's chess", -temp - 1);
				if (otherchess[temp].life == dead)
					printf(",which has been dead\n");
				else
				{
					printf(",the identity now is");
					if (otherchess[temp].identify == solider)
					{
						printf("[solider]");
					}
					else
					{
						printf("[king]");
					}
					printf(",whose place is (%d,%d)\n", otherchess[temp].x, otherchess[temp].y);
				}
			}


		}
		else if (!strcmp(action, "BOARD"))
		{
			int a, b;
			scanf("%d %d", &a, &b);
			printf("%d,%d:%d\n", a, b, board_id[a][b]);
		}
	}
	scanf("%*d");
	return 0;

}

void printBoard(const int board_ids[][LENGTH])
{
	printf("========\n");
	printf(" 01234567\n");
	for (int i = 0; i < LENGTH; i++)
	{
		printf("%d", i);
		for (int j = 0; j < LENGTH; j++)
		{
			if (board_ids[i][j] == 0)
			{
				printf(".");
			}
			else if ((board_ids[i][j] > 0 && my_color == BLACK) || (board_ids[i][j] < 0 && my_color == WHITE))
			{
				printf("O");
			}
			else
			{
				printf("X");
			}
		}
		printf("\n");
	}
	printf("========\n");
}

void start()
{
	flag = 1;

	//棋盘初始化
	int temp = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			if (my_color == WHITE)
			{
				mychess[temp].x = i;
				mychess[temp].y = j + (i + 1) % 2;
				mychess[temp].identify = solider;
				mychess[temp++].life = alive;
				board_id[i][j + (i + 1) % 2] = temp;
			}
			else
			{
				otherchess[temp].x = i;
				otherchess[temp].y = j + (i + 1) % 2;
				otherchess[temp].identify = solider;
				otherchess[temp++].life = alive;
				board_id[i][j + (i + 1) % 2] = -temp;
			}
		}
	}
	temp = 0;
	for (int i = 5; i < 8; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			if (my_color == BLACK)
			{
				mychess[temp].x = i;
				mychess[temp].y = j + (i + 1) % 2;
				mychess[temp].identify = solider;
				mychess[temp++].life = alive;
				board_id[i][j + (i + 1) % 2] = temp;
			}
			else
			{
				otherchess[temp].x = i;
				otherchess[temp].y = j + (i + 1) % 2;
				otherchess[temp].identify = solider;
				otherchess[temp++].life = alive;
				board_id[i][j + (i + 1) % 2] = -temp;
			}
		}
	}
}

//移动棋子函数
void place(const struct Command cmd, int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses)
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			//更改当前位置的棋子状态
			if (board_ids[x_mid][y_mid] > 0)
			{
				//死亡
				mychesses[board_ids[x_mid][y_mid] - 1].life = dead;
				board_ids[x_mid][y_mid] = 0;
			}
			else
			{
				//死亡
				otherchesses[-board_ids[x_mid][y_mid] - 1].life = dead;
				board_ids[x_mid][y_mid] = 0;
			}
		}
	}
	//更改初始位置的棋子状态
	board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] = board_ids[cmd.x[0]][cmd.y[0]];
	board_ids[cmd.x[0]][cmd.y[0]] = 0;
	if (board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] > 0)
	{
		mychesses[board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] - 1].x = cmd.x[cmd.numStep - 1];
		mychesses[board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] - 1].y = cmd.y[cmd.numStep - 1];

	}
	else
	{
		otherchesses[-board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] - 1].x = cmd.x[cmd.numStep - 1];
		otherchesses[-board_ids[cmd.x[cmd.numStep - 1]][cmd.y[cmd.numStep - 1]] - 1].y = cmd.y[cmd.numStep - 1];
	}
}

BOOL isInBound(int x, int y)
{
	return x >= 0 && x < LENGTH && y >= 0 && y < LENGTH;
}

//判断该坐标的棋子是否能变为王棋
BOOL ToKing(int x, int y)
{
	//flag为正表示我方行动，为负表示对方正在行动
	if (my_color == BLACK)
	{
		if ((flag > 0 && x == 0) || (flag < 0 && x == LENGTH - 1))
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if ((flag > 0 && x == LENGTH - 1) || (flag < 0 && x == 0))
			return TRUE;
		else
			return FALSE;
	}
}

//吃子函数
void eat(int board_ids[][LENGTH], int x, int y, int number, int layer, struct Command *eating)
//number表示当前正在操控吃子的棋子编号,flag为正表示我方移动、为负表示敌方移动
{

	//分别观察左上、右上、左下、右下是否可以吃子
	//左上
	if (isInBound(x - 2, y - 2) && board_ids[x - 2][y - 2] == 0 && board_ids[x - 1][y - 1] * flag < 0)
	{
		board_ids[x - 2][y - 2] = number;
		int tick = board_ids[x - 1][y - 1];
		board_ids[x - 1][y - 1] = 0;
		board_ids[x][y] = 0;
		//记录吃子信息
		eating[0].x[layer + 1] = x - 2;
		eating[0].y[layer + 1] = y - 2;
		eating[0].numStep++;
		//复制到四个结构体数据中
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[0];
		}
		//进入下一层循环
		eat(board_ids, x - 2, y - 2, number, layer + 1, eatings);
		//走完了，撤回
		board_ids[x - 2][y - 2] = 0;
		board_ids[x - 1][y - 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[0].numStep)
				eating[0] = eatings[i];
		}
	}
	//右上
	if (isInBound(x - 2, y + 2) && board_ids[x - 2][y + 2] == 0 && board_ids[x - 1][y + 1] * flag < 0)
	{
		board_ids[x - 2][y + 2] = number;
		int tick = board_ids[x - 1][y + 1];
		board_ids[x - 1][y + 1] = 0;
		board_ids[x][y] = 0;
		//记录吃子信息
		eating[1].x[layer + 1] = x - 2;
		eating[1].y[layer + 1] = y + 2;
		eating[1].numStep++;
		//复制到四个结构体数据中
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[1];
		}
		//进入下一层循环
		eat(board_ids, x - 2, y + 2, number, layer + 1, eatings);
		//走完了，撤回
		board_ids[x - 2][y + 2] = 0;
		board_ids[x - 1][y + 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[1].numStep)
				eating[1] = eatings[i];
		}
	}
	//左下
	if (isInBound(x + 2, y - 2) && board_ids[x + 2][y - 2] == 0 && board_ids[x + 1][y - 1] * flag < 0)
	{
		board_ids[x + 2][y - 2] = number;
		int tick = board_ids[x + 1][y - 1];
		board_ids[x + 1][y - 1] = 0;
		board_ids[x][y] = 0;
		//记录吃子信息
		eating[2].x[layer + 1] = x + 2;
		eating[2].y[layer + 1] = y - 2;
		eating[2].numStep++;
		//复制到四个结构体数据中
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[2];
		}
		//进入下一层循环
		eat(board_ids, x + 2, y - 2, number, layer + 1, eatings);
		//走完了，撤回
		board_ids[x + 2][y - 2] = 0;
		board_ids[x + 1][y - 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[2].numStep)
				eating[2] = eatings[i];
		}
	}
	//右下
	if (isInBound(x + 2, y + 2) && board_ids[x + 2][y + 2] == 0 && board_ids[x + 1][y + 1] * flag < 0)
	{
		board_ids[x + 2][y + 2] = number;
		int tick = board_ids[x + 1][y + 1];
		board_ids[x + 1][y + 1] = 0;
		board_ids[x][y] = 0;
		//记录吃子信息
		eating[3].x[layer + 1] = x + 2;
		eating[3].y[layer + 1] = y + 2;
		eating[3].numStep++;
		//复制到四个结构体数据中
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[3];
		}
		//进入下一层循环
		eat(board_ids, x + 2, y + 2, number, layer + 1, eatings);
		//走完了，撤回
		board_ids[x + 2][y + 2] = 0;
		board_ids[x + 1][y + 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[3].numStep)
				eating[3] = eatings[i];
		}
	}
}

//棋盘运行的主要AI
int move_AI(int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses, int layer, int cutting_line)
//layer表示当前搜索所处层数,cutting_line表示剪枝的边界条件
{
	//如果已经到达搜索层数，计算当前局面
	if (layer == LAYER_FIND)
		return estimate(board_ids, mychesses, otherchesses);
	else if (DepthSoft == 1 && layer == 2)
		return estimate(board_ids, mychesses, otherchesses);
	else if (round>=80 && layer == LAYER_FIND-1)
	    return estimate(board_ids, mychesses, otherchesses);

	//分值信息
	int score_now;
	int score_max = -999999, score_min = 999999;

	//储存吃子信息
	struct Command eating[12][4];
	int eating_accept[12][4] = { 0 };
	int max_eat = 1;

	if (flag > 0)
	{
		for (int i = 0; i < 12; i++)
		{
			//进入循环前，初始化信息
			for (int j = 0; j < 4; j++)
			{
				eating[i][j].numStep = 1;
				eating[i][j].x[0] = mychesses[i].x;
				eating[i][j].y[0] = mychesses[i].y;
			}
			if (mychesses[i].life == alive)
			{
				eat(board_ids, mychesses[i].x, mychesses[i].y, i + 1, 0, eating[i]);
				for (int j = 0; j < 4; j++)
				{
					if (eating[i][j].numStep > max_eat)
						max_eat = eating[i][j].numStep;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 12; i++)
		{
			//进入循环前，初始化信息
			for (int j = 0; j < 4; j++)
			{
				eating[i][j].numStep = 1;
				eating[i][j].x[0] = otherchesses[i].x;
				eating[i][j].y[0] = otherchesses[i].y;
			}
			if (otherchesses[i].life == alive)
			{

				eat(board_ids, otherchesses[i].x, otherchesses[i].y, -i - 1, 0, eating[i]);
				for (int j = 0; j < 4; j++)
				{
					if (eating[i][j].numStep > max_eat)
					{
						max_eat = eating[i][j].numStep;
					}
				}
			}
		}
	}

	int tick_i = -1, tick_j = 0;

	//确认是否可以吃子
	if (max_eat > 1)
	{
		//遍历确认分值最大的吃子方式
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (eating[i][j].numStep == max_eat)
				{
					eating_accept[i][j] = 1;

					//复制一个传入
					struct chess mychess_image[12], otherchess_image[12];
					memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
					memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
					int board_ids_image[LENGTH][LENGTH];
					memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

					//尝试这种下棋方法
					place(eating[i][j], board_ids_image, mychess_image, otherchess_image);

					//判断是否成为王棋
					if (ToKing(eating[i][j].x[eating[i][j].numStep - 1], eating[i][j].y[eating[i][j].numStep - 1]))
					{
						if (flag > 0)
						{
							mychess_image[board_ids_image[eating[i][j].x[eating[i][j].numStep - 1]][eating[i][j].y[eating[i][j].numStep - 1]] - 1].identify = king;
						}
						else
						{
							otherchess_image[-board_ids_image[eating[i][j].x[eating[i][j].numStep - 1]][eating[i][j].y[eating[i][j].numStep - 1]] - 1].identify = king;
						}
					}


					//进入下一层循环
					flag = -flag;

					if (flag > 0)
					{
						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, 999999);
						if (score_now < score_min)
						{
							score_min = score_now;
							tick_i = i;
							tick_j = j;
						}
					}
					else
					{
						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, -999999);
						if (score_now > score_max)
						{
							score_max = score_now;
							tick_i = i;
							tick_j = j;
						}
					}

					flag = -flag;
				}
			}
		}

		//遍历完毕，选择最佳走法
		if (layer != 0)
		{
			if (flag > 0)
				return score_max;
			else
				return score_min;
		}
		else
		{
			command = eating[tick_i][tick_j];
			return 0;
		}
	}
	else
	{
		//在第零层进行搜索，按照分值由高到低的顺序进行搜索
		if (layer == 0)
		{
			//记录每一种搜索的分值
			int score_find[12] = { -99999,-99999, -99999, -99999, -99999, -99999,
			-99999, -99999, -99999, -99999, -99999, -99999 };

			//搜索顺序
			int soft_find[12];

			//进入浅层搜索模式
			DepthSoft = 1;

			//遍历12种搜索
			for (int i = 0; i < 12; i++)
			{
				if (mychesses[i].life == alive)
				{
					//左上
					if (isInBound(mychesses[i].x - 1, mychesses[i].y - 1) && board_ids[mychesses[i].x - 1][mychesses[i].y - 1] == 0
						&& (my_color == BLACK || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x - 1;
						command.y[1] = mychesses[i].y - 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;

					}
					//右上
					if (isInBound(mychesses[i].x - 1, mychesses[i].y + 1) && board_ids[mychesses[i].x - 1][mychesses[i].y + 1] == 0
						&& (my_color == BLACK || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x - 1;
						command.y[1] = mychesses[i].y + 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;
					}
					//左下
					if (isInBound(mychesses[i].x + 1, mychesses[i].y - 1) && board_ids[mychesses[i].x + 1][mychesses[i].y - 1] == 0
						&& (my_color == WHITE || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x + 1;
						command.y[1] = mychesses[i].y - 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;
					}
					//右下
					if (isInBound(mychesses[i].x + 1, mychesses[i].y + 1) && board_ids[mychesses[i].x + 1][mychesses[i].y + 1] == 0
						&& (my_color == WHITE || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x + 1;
						command.y[1] = mychesses[i].y + 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > cutting_line)
						{
							return 999999;
						}

						if (score_now > score_find[i])
							score_find[i] = score_now;
					}
				}
			}

			//关闭浅层搜索模式
			DepthSoft = 0;

			//按照分值从高到低的顺序进行搜索
			for (int i = 0; i < 12; i++)
			{
				int max = -999999;
				int temps = 0;
				for (int j = 0; j < 12; j++)
				{
					if (score_find[j] > max)
					{
						max = score_find[j];
						temps = j;
					}
				}
				//本轮遍历完成
				score_find[temps] = -999999;

				soft_find[i] = temps;
			}

			//按照浅层的顺序进行搜索
			for (int i = 0; i < 12; i++)
			{
				if (mychesses[soft_find[i]].life == alive)
				{
					//左上
					if (isInBound(mychesses[soft_find[i]].x - 1, mychesses[soft_find[i]].y - 1) && board_ids[mychesses[soft_find[i]].x - 1][mychesses[soft_find[i]].y - 1] == 0
						&& (my_color == BLACK || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x - 1;
						command.y[1] = mychesses[soft_find[i]].y - 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > cutting_line)
						{
							return 999999;
						}

						if (score_now > score_max || tick_i == -1)
						{
							score_max = score_now;
							tick_i = soft_find[i];
							tick_j = 0;
						}
					}
					//右上
					if (isInBound(mychesses[soft_find[i]].x - 1, mychesses[soft_find[i]].y + 1) && board_ids[mychesses[soft_find[i]].x - 1][mychesses[soft_find[i]].y + 1] == 0
						&& (my_color == BLACK || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x - 1;
						command.y[1] = mychesses[soft_find[i]].y + 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > cutting_line)
						{
							return 999999;
						}

						if (score_now > score_max || tick_i == -1)
						{
							score_max = score_now;
							tick_i = soft_find[i];
							tick_j = 1;
						}
					}
					//左下
					if (isInBound(mychesses[soft_find[i]].x + 1, mychesses[soft_find[i]].y - 1) && board_ids[mychesses[soft_find[i]].x + 1][mychesses[soft_find[i]].y - 1] == 0
						&& (my_color == WHITE || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x + 1;
						command.y[1] = mychesses[soft_find[i]].y - 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > cutting_line)
						{
							return 999999;
						}

						if (score_now > score_max || tick_i == -1)
						{
							score_max = score_now;
							tick_i = soft_find[i];
							tick_j = 2;
						}
					}
					//右下
					if (isInBound(mychesses[soft_find[i]].x + 1, mychesses[soft_find[i]].y + 1) && board_ids[mychesses[soft_find[i]].x + 1][mychesses[soft_find[i]].y + 1] == 0
						&& (my_color == WHITE || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x + 1;
						command.y[1] = mychesses[soft_find[i]].y + 1;
						//复制一个传入
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//放置
						place(command, board_ids_image, mychess_image, otherchess_image);

						//判断是否成为王棋
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//进入下一层循环
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > cutting_line)
						{
							return 999999;
						}

						if (score_now > score_max || tick_i == -1)
						{
							score_max = score_now;
							tick_i = soft_find[i];
							tick_j = 3;
						}
					}
				}
			}

			//返回最佳走法
			command.numStep = 2;
			command.x[0] = mychesses[tick_i].x;
			command.y[0] = mychesses[tick_i].y;
			switch (tick_j)
			{
			case 0:
				command.x[1] = mychesses[tick_i].x - 1;
				command.y[1] = mychesses[tick_i].y - 1;
				break;
			case 1:
				command.x[1] = mychesses[tick_i].x - 1;
				command.y[1] = mychesses[tick_i].y + 1;
				break;
			case 2:
				command.x[1] = mychesses[tick_i].x + 1;
				command.y[1] = mychesses[tick_i].y - 1;
				break;
			case 3:
				command.x[1] = mychesses[tick_i].x + 1;
				command.y[1] = mychesses[tick_i].y + 1;
				break;
			}
			return 0;
		}
		else
		{
			if (flag > 0)
			{
				for (int i = 0; i < 12; i++)
				{
					if (mychesses[i].life == alive)
					{
						//左上
						if (isInBound(mychesses[i].x - 1, mychesses[i].y - 1) && board_ids[mychesses[i].x - 1][mychesses[i].y - 1] == 0
							&& (my_color == BLACK || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x - 1;
							command.y[1] = mychesses[i].y - 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

							flag = -flag;

							if (score_now > cutting_line)
							{
								return 999999;
							}

							if (score_now > score_max || tick_i == -1)
							{
								score_max = score_now;
								tick_i = i;
								tick_j = 0;
							}
						}
						//右上
						if (isInBound(mychesses[i].x - 1, mychesses[i].y + 1) && board_ids[mychesses[i].x - 1][mychesses[i].y + 1] == 0
							&& (my_color == BLACK || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x - 1;
							command.y[1] = mychesses[i].y + 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

							flag = -flag;

							if (score_now > cutting_line)
							{
								return 999999;
							}

							if (score_now > score_max || tick_i == -1)
							{
								score_max = score_now;
								tick_i = i;
								tick_j = 1;
							}
						}
						//左下
						if (isInBound(mychesses[i].x + 1, mychesses[i].y - 1) && board_ids[mychesses[i].x + 1][mychesses[i].y - 1] == 0
							&& (my_color == WHITE || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x + 1;
							command.y[1] = mychesses[i].y - 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

							flag = -flag;

							if (score_now > cutting_line)
							{
								return 999999;
							}

							if (score_now > score_max || tick_i == -1)
							{
								score_max = score_now;
								tick_i = i;
								tick_j = 2;
							}
						}
						//右下
						if (isInBound(mychesses[i].x + 1, mychesses[i].y + 1) && board_ids[mychesses[i].x + 1][mychesses[i].y + 1] == 0
							&& (my_color == WHITE || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x + 1;
							command.y[1] = mychesses[i].y + 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

							flag = -flag;

							if (score_now > cutting_line)
							{
								return 999999;
							}

							if (score_now > score_max || tick_i == -1)
							{
								score_max = score_now;
								tick_i = i;
								tick_j = 3;
							}
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < 12; i++)
				{
					if (otherchesses[i].life == alive)
					{
						//左上
						if (isInBound(otherchesses[i].x - 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y - 1] == 0
							&& (other_color == BLACK || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x - 1;
							command.y[1] = otherchesses[i].y - 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_min);

							flag = -flag;

							if (score_now < cutting_line)
							{
								return -999999;
							}

							if (score_now < score_min)
							{
								score_min = score_now;
								tick_i = i;
								tick_j = 0;
							}
						}
						//右上
						if (isInBound(otherchesses[i].x - 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y + 1] == 0
							&& (other_color == BLACK || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x - 1;
							command.y[1] = otherchesses[i].y + 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_min);

							flag = -flag;

							if (score_now < cutting_line)
							{
								return -999999;
							}

							if (score_now < score_min)
							{
								score_min = score_now;
								tick_i = i;
								tick_j = 1;
							}
						}
						//左下
						if (isInBound(otherchesses[i].x + 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y - 1] == 0
							&& (other_color == WHITE || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x + 1;
							command.y[1] = otherchesses[i].y - 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_min);

							flag = -flag;

							if (score_now < cutting_line)
							{
								return -999999;
							}

							if (score_now < score_min)
							{
								score_min = score_now;
								tick_i = i;
								tick_j = 2;
							}
						}
						//右下
						if (isInBound(otherchesses[i].x + 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y + 1] == 0
							&& (other_color == WHITE || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x + 1;
							command.y[1] = otherchesses[i].y + 1;
							//复制一个传入
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//放置
							place(command, board_ids_image, mychess_image, otherchess_image);

							//判断是否成为王棋
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//进入下一层循环
							flag = -flag;

							score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_min);

							flag = -flag;

							if (score_now < cutting_line)
							{
								return -999999;
							}

							if (score_now < score_min)
							{
								score_min = score_now;
								tick_i = i;
								tick_j = 3;
							}
						}
					}
				}
			}


			if (flag > 0)
				return score_max;
			else
				return score_min;

		}

	}
}

//局面评估函数
int estimate(const int board_ids[][LENGTH], const struct chess *mychesses, const struct chess *otherchesses)
{
	int score = 0;//分数

	/*
	王占60分，兵占50分
	离对方底线越近，分值越高(上限8分)
	处在边界，表示当前比较安全，分值+10
	如果该子能够移动，分值+可移动的方向数X4
	*/
	for (int i = 0; i < 12; i++)
	{
		if (mychesses[i].life == alive)
		{
			//判断身份
			if (mychesses[i].identify == king)
				score += 60;
			else
			{
				score += 50;
				//计算距离
				if (my_color == BLACK)
				{
					//离最下方越远分值越高
					score += abs(mychesses[i].x - 7);
				}
				else
				{
					score += mychesses[i].x;
				}
			}
			if (mychesses[i].y == 0 || mychesses[i].y == 7)
				score += 5;
			//移动方向数
			if (isInBound(mychesses[i].x - 1, mychesses[i].y - 1) && board_ids[mychesses[i].x - 1][mychesses[i].y - 1] == 0
				&& (my_color == BLACK || mychesses[i].identify == king))
				score += 5;
			if (isInBound(mychesses[i].x - 1, mychesses[i].y + 1) && board_ids[mychesses[i].x - 1][mychesses[i].y + 1] == 0
				&& (my_color == BLACK || mychesses[i].identify == king))
				score += 5;
			if (isInBound(mychesses[i].x + 1, mychesses[i].y - 1) && board_ids[mychesses[i].x + 1][mychesses[i].y - 1] == 0
				&& (my_color == WHITE || mychesses[i].identify == king))
				score += 5;
			if (isInBound(mychesses[i].x + 1, mychesses[i].y + 1) && board_ids[mychesses[i].x + 1][mychesses[i].y + 1] == 0
				&& (my_color == WHITE || mychesses[i].identify == king))
				score += 5;

		}
		if (otherchesses[i].life == alive)
		{
			//判断身份
			if (otherchesses[i].identify == king)
				score -= 60;
			else
			{
				score -= 50;
				//计算距离
				if (my_color == BLACK)
				{
					score -= otherchesses[i].x;
				}
				else
				{
					score -= abs(otherchesses[i].x - 7);
				}
			}
			if (otherchesses[i].y == 0 || otherchesses[i].y == 7)
				score -= 5;
			//移动方向数
			if (isInBound(otherchesses[i].x - 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y - 1] == 0
				&& (other_color == BLACK || otherchesses[i].identify == king))
				score -= 5;
			if (isInBound(otherchesses[i].x - 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y + 1] == 0
				&& (other_color == BLACK || otherchesses[i].identify == king))
				score -= 5;
			if (isInBound(otherchesses[i].x + 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y - 1] == 0
				&& (other_color == WHITE || otherchesses[i].identify == king))
				score -= 5;
			if (isInBound(otherchesses[i].x + 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y + 1] == 0
				&& (other_color == WHITE || otherchesses[i].identify == king))
				score -= 5;
		}
	}

	return score;
}