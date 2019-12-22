#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//������־
//11.19 22:00 ɾ��ʾ�����룬��ʼд��
//11.20 12:00 ������ƶ�����
//11.20 18:00 д������������������ӹ���	
//11.21 10:00 ���ѣ����Ӻ���
//11.21 21:00 ��ɳ��Ӻ���
//11.22 23:00 ��ʼ����д���ӹ���
//11.23 10:30 �����ӹ���
//11.23 17:00 ���ƴ󲿷ֺ����Ĳ���
//11.24 10:30 ����ͨ�����Ӻ���
//11.25 19:00 �������������
//11.26 12:00 ���һ�����bug
//11.26 13:30 ��ʼд������������
//11.27 14:00 ����������
//11.27 16:30 �ҵ����������ô��ô��bug������
//11.28 19:00 �����е�������������
//11.28 19:36 ��һ�δ�ӮDraughts Medium�Ѷȴ�
//11.30 13:30 ���������СֵѰ�Ҳ��ִ���ҮҮҮ
//12.01 09:00 ����һ��СС��bug
//12.01 18:00 ���Alpha-Beta��֦�㷨��
//12.05 23:00 ǰ�����̶��߷�+�޸�һ��bug
//12.10 ����д�Ĵ������ڿ�����������
//12.11 ������bug�ҳ����ˡ�
//12.11 �оֵ�bug��
//12.12 ����->�߽����

//��������
#define LAYER_FIND 8

//����BOOL�ͱ���
#define BOOL int
#define TRUE 1
#define FALSE 0

//���̻�����Ϣ
#define LENGTH 8
#define EMPTY 0
#define BLACK 1
#define WHITE 2

//������Ϣ
#define king 1
#define solider 0
#define alive 1
#define dead 0

//����
int board_id[LENGTH][LENGTH];//���,���ű�ʾ�Լ��ģ����ű�ʾ�Է���

//ִ����ɫ
int my_color;
int other_color;

//����
int round;

//��ǰ���ӷ�
int flag;

//ǳ������
int DepthSoft = 0;

//ÿһ�����ӵ���Ϣ
struct chess
{
	int x;
	int y;
	int identify;
	int life;
};
struct chess mychess[12], otherchess[12];

//���������ƶ���Ϣ�Ľṹ��
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
//��¼�������
//struct Command eating[12];

//����
void start();//��ʼ������
void printBoard(const int board_ids[][LENGTH]);//��ʾ��ǰ����
void place(const struct Command, int board_ids[][LENGTH], struct chess *mychess, struct chess *otherchess);//�ƶ�����
BOOL isInBound(int x, int y);//�ж��Ƿ�Խ��
void eat(int board_ids[][LENGTH], int x, int y, int number, int layer, struct Command *eating);//�жϵ�num�������Ƿ���Գ���
BOOL ToKing(int x, int y);//�жϸ�����������Ƿ��ܱ�Ϊ����
int move_AI(int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses, int layer, int cutting_line);//��Ҫ�߼�
int estimate(const int board_ids[][LENGTH], const struct chess *mychesses, const struct chess *otherchesses);//������������

int main()
{

	char action[6];//���ڽ���ָ��ı���

	//��ʼ
	scanf("%*s %d", &my_color);
	//�Է�����ɫ
	other_color = (my_color == BLACK) ? WHITE : BLACK;

	//�ظ�ϵͳ
	printf("OK\n");
	fflush(stdout);

	start();//��ʼ������

	//ѭ����ȡָ��
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
			//�ƶ�����
			place(command, board_id, mychess, otherchess);
			//���ӽ������ж��Ƿ���Ϊ����
			flag = -flag;//�����庯�����漰flag
			if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
			{
				otherchess[-board_id[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
			}
			flag = -flag;//�����
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
			//�ж��������
			command.numStep = 0;//��ʼ��Ϊ0
			move_AI(board_id_image, mychess_image, otherchess_image, 0, 999999);
			//��ʱ�ҷ�������Ϣ�Ѿ���������command�ṹ����
			place(command, board_id, mychess, otherchess);
			//Ҫ�����ӷ�ʽ���
			printf("%d ", command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				printf("%d,%d", command.x[i], command.y[i]);
				if (i != command.numStep - 1)
					printf(" ");
				else
					printf("\n");
			}
			//���ӽ������ж����Ƿ���Ϊ����
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

	//���̳�ʼ��
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

//�ƶ����Ӻ���
void place(const struct Command cmd, int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses)
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			//���ĵ�ǰλ�õ�����״̬
			if (board_ids[x_mid][y_mid] > 0)
			{
				//����
				mychesses[board_ids[x_mid][y_mid] - 1].life = dead;
				board_ids[x_mid][y_mid] = 0;
			}
			else
			{
				//����
				otherchesses[-board_ids[x_mid][y_mid] - 1].life = dead;
				board_ids[x_mid][y_mid] = 0;
			}
		}
	}
	//���ĳ�ʼλ�õ�����״̬
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

//�жϸ�����������Ƿ��ܱ�Ϊ����
BOOL ToKing(int x, int y)
{
	//flagΪ����ʾ�ҷ��ж���Ϊ����ʾ�Է������ж�
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

//���Ӻ���
void eat(int board_ids[][LENGTH], int x, int y, int number, int layer, struct Command *eating)
//number��ʾ��ǰ���ڲٿس��ӵ����ӱ��,flagΪ����ʾ�ҷ��ƶ���Ϊ����ʾ�з��ƶ�
{

	//�ֱ�۲����ϡ����ϡ����¡������Ƿ���Գ���
	//����
	if (isInBound(x - 2, y - 2) && board_ids[x - 2][y - 2] == 0 && board_ids[x - 1][y - 1] * flag < 0)
	{
		board_ids[x - 2][y - 2] = number;
		int tick = board_ids[x - 1][y - 1];
		board_ids[x - 1][y - 1] = 0;
		board_ids[x][y] = 0;
		//��¼������Ϣ
		eating[0].x[layer + 1] = x - 2;
		eating[0].y[layer + 1] = y - 2;
		eating[0].numStep++;
		//���Ƶ��ĸ��ṹ��������
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[0];
		}
		//������һ��ѭ��
		eat(board_ids, x - 2, y - 2, number, layer + 1, eatings);
		//�����ˣ�����
		board_ids[x - 2][y - 2] = 0;
		board_ids[x - 1][y - 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[0].numStep)
				eating[0] = eatings[i];
		}
	}
	//����
	if (isInBound(x - 2, y + 2) && board_ids[x - 2][y + 2] == 0 && board_ids[x - 1][y + 1] * flag < 0)
	{
		board_ids[x - 2][y + 2] = number;
		int tick = board_ids[x - 1][y + 1];
		board_ids[x - 1][y + 1] = 0;
		board_ids[x][y] = 0;
		//��¼������Ϣ
		eating[1].x[layer + 1] = x - 2;
		eating[1].y[layer + 1] = y + 2;
		eating[1].numStep++;
		//���Ƶ��ĸ��ṹ��������
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[1];
		}
		//������һ��ѭ��
		eat(board_ids, x - 2, y + 2, number, layer + 1, eatings);
		//�����ˣ�����
		board_ids[x - 2][y + 2] = 0;
		board_ids[x - 1][y + 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[1].numStep)
				eating[1] = eatings[i];
		}
	}
	//����
	if (isInBound(x + 2, y - 2) && board_ids[x + 2][y - 2] == 0 && board_ids[x + 1][y - 1] * flag < 0)
	{
		board_ids[x + 2][y - 2] = number;
		int tick = board_ids[x + 1][y - 1];
		board_ids[x + 1][y - 1] = 0;
		board_ids[x][y] = 0;
		//��¼������Ϣ
		eating[2].x[layer + 1] = x + 2;
		eating[2].y[layer + 1] = y - 2;
		eating[2].numStep++;
		//���Ƶ��ĸ��ṹ��������
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[2];
		}
		//������һ��ѭ��
		eat(board_ids, x + 2, y - 2, number, layer + 1, eatings);
		//�����ˣ�����
		board_ids[x + 2][y - 2] = 0;
		board_ids[x + 1][y - 1] = tick;
		board_ids[x][y] = number;
		for (int i = 0; i < 4; i++)
		{
			if (eatings[i].numStep > eating[2].numStep)
				eating[2] = eatings[i];
		}
	}
	//����
	if (isInBound(x + 2, y + 2) && board_ids[x + 2][y + 2] == 0 && board_ids[x + 1][y + 1] * flag < 0)
	{
		board_ids[x + 2][y + 2] = number;
		int tick = board_ids[x + 1][y + 1];
		board_ids[x + 1][y + 1] = 0;
		board_ids[x][y] = 0;
		//��¼������Ϣ
		eating[3].x[layer + 1] = x + 2;
		eating[3].y[layer + 1] = y + 2;
		eating[3].numStep++;
		//���Ƶ��ĸ��ṹ��������
		struct Command eatings[4];
		for (int i = 0; i < 4; i++)
		{
			eatings[i] = eating[3];
		}
		//������һ��ѭ��
		eat(board_ids, x + 2, y + 2, number, layer + 1, eatings);
		//�����ˣ�����
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

//�������е���ҪAI
int move_AI(int board_ids[][LENGTH], struct chess *mychesses, struct chess *otherchesses, int layer, int cutting_line)
//layer��ʾ��ǰ������������,cutting_line��ʾ��֦�ı߽�����
{
	//����Ѿ������������������㵱ǰ����
	if (layer == LAYER_FIND)
		return estimate(board_ids, mychesses, otherchesses);
	else if (DepthSoft == 1 && layer == 2)
		return estimate(board_ids, mychesses, otherchesses);
	else if (round>=80 && layer == LAYER_FIND-1)
	    return estimate(board_ids, mychesses, otherchesses);

	//��ֵ��Ϣ
	int score_now;
	int score_max = -999999, score_min = 999999;

	//���������Ϣ
	struct Command eating[12][4];
	int eating_accept[12][4] = { 0 };
	int max_eat = 1;

	if (flag > 0)
	{
		for (int i = 0; i < 12; i++)
		{
			//����ѭ��ǰ����ʼ����Ϣ
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
			//����ѭ��ǰ����ʼ����Ϣ
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

	//ȷ���Ƿ���Գ���
	if (max_eat > 1)
	{
		//����ȷ�Ϸ�ֵ���ĳ��ӷ�ʽ
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (eating[i][j].numStep == max_eat)
				{
					eating_accept[i][j] = 1;

					//����һ������
					struct chess mychess_image[12], otherchess_image[12];
					memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
					memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
					int board_ids_image[LENGTH][LENGTH];
					memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

					//�����������巽��
					place(eating[i][j], board_ids_image, mychess_image, otherchess_image);

					//�ж��Ƿ��Ϊ����
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


					//������һ��ѭ��
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

		//������ϣ�ѡ������߷�
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
		//�ڵ����������������շ�ֵ�ɸߵ��͵�˳���������
		if (layer == 0)
		{
			//��¼ÿһ�������ķ�ֵ
			int score_find[12] = { -99999,-99999, -99999, -99999, -99999, -99999,
			-99999, -99999, -99999, -99999, -99999, -99999 };

			//����˳��
			int soft_find[12];

			//����ǳ������ģʽ
			DepthSoft = 1;

			//����12������
			for (int i = 0; i < 12; i++)
			{
				if (mychesses[i].life == alive)
				{
					//����
					if (isInBound(mychesses[i].x - 1, mychesses[i].y - 1) && board_ids[mychesses[i].x - 1][mychesses[i].y - 1] == 0
						&& (my_color == BLACK || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x - 1;
						command.y[1] = mychesses[i].y - 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;

					}
					//����
					if (isInBound(mychesses[i].x - 1, mychesses[i].y + 1) && board_ids[mychesses[i].x - 1][mychesses[i].y + 1] == 0
						&& (my_color == BLACK || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x - 1;
						command.y[1] = mychesses[i].y + 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;
					}
					//����
					if (isInBound(mychesses[i].x + 1, mychesses[i].y - 1) && board_ids[mychesses[i].x + 1][mychesses[i].y - 1] == 0
						&& (my_color == WHITE || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x + 1;
						command.y[1] = mychesses[i].y - 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
						flag = -flag;

						score_now = move_AI(board_ids_image, mychess_image, otherchess_image, layer + 1, score_max);

						flag = -flag;

						if (score_now > score_find[i])
							score_find[i] = score_now;
					}
					//����
					if (isInBound(mychesses[i].x + 1, mychesses[i].y + 1) && board_ids[mychesses[i].x + 1][mychesses[i].y + 1] == 0
						&& (my_color == WHITE || mychesses[i].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[i].x;
						command.y[0] = mychesses[i].y;
						command.x[1] = mychesses[i].x + 1;
						command.y[1] = mychesses[i].y + 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
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

			//�ر�ǳ������ģʽ
			DepthSoft = 0;

			//���շ�ֵ�Ӹߵ��͵�˳���������
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
				//���ֱ������
				score_find[temps] = -999999;

				soft_find[i] = temps;
			}

			//����ǳ���˳���������
			for (int i = 0; i < 12; i++)
			{
				if (mychesses[soft_find[i]].life == alive)
				{
					//����
					if (isInBound(mychesses[soft_find[i]].x - 1, mychesses[soft_find[i]].y - 1) && board_ids[mychesses[soft_find[i]].x - 1][mychesses[soft_find[i]].y - 1] == 0
						&& (my_color == BLACK || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x - 1;
						command.y[1] = mychesses[soft_find[i]].y - 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
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
					//����
					if (isInBound(mychesses[soft_find[i]].x - 1, mychesses[soft_find[i]].y + 1) && board_ids[mychesses[soft_find[i]].x - 1][mychesses[soft_find[i]].y + 1] == 0
						&& (my_color == BLACK || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x - 1;
						command.y[1] = mychesses[soft_find[i]].y + 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
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
					//����
					if (isInBound(mychesses[soft_find[i]].x + 1, mychesses[soft_find[i]].y - 1) && board_ids[mychesses[soft_find[i]].x + 1][mychesses[soft_find[i]].y - 1] == 0
						&& (my_color == WHITE || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x + 1;
						command.y[1] = mychesses[soft_find[i]].y - 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
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
					//����
					if (isInBound(mychesses[soft_find[i]].x + 1, mychesses[soft_find[i]].y + 1) && board_ids[mychesses[soft_find[i]].x + 1][mychesses[soft_find[i]].y + 1] == 0
						&& (my_color == WHITE || mychesses[soft_find[i]].identify == king))
					{
						command.numStep = 2;
						command.x[0] = mychesses[soft_find[i]].x;
						command.y[0] = mychesses[soft_find[i]].y;
						command.x[1] = mychesses[soft_find[i]].x + 1;
						command.y[1] = mychesses[soft_find[i]].y + 1;
						//����һ������
						struct chess mychess_image[12], otherchess_image[12];
						memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
						memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
						int board_ids_image[LENGTH][LENGTH];
						memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

						//����
						place(command, board_ids_image, mychess_image, otherchess_image);

						//�ж��Ƿ��Ϊ����
						if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
						{
							mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
						}

						//������һ��ѭ��
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

			//��������߷�
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
						//����
						if (isInBound(mychesses[i].x - 1, mychesses[i].y - 1) && board_ids[mychesses[i].x - 1][mychesses[i].y - 1] == 0
							&& (my_color == BLACK || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x - 1;
							command.y[1] = mychesses[i].y - 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(mychesses[i].x - 1, mychesses[i].y + 1) && board_ids[mychesses[i].x - 1][mychesses[i].y + 1] == 0
							&& (my_color == BLACK || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x - 1;
							command.y[1] = mychesses[i].y + 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(mychesses[i].x + 1, mychesses[i].y - 1) && board_ids[mychesses[i].x + 1][mychesses[i].y - 1] == 0
							&& (my_color == WHITE || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x + 1;
							command.y[1] = mychesses[i].y - 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(mychesses[i].x + 1, mychesses[i].y + 1) && board_ids[mychesses[i].x + 1][mychesses[i].y + 1] == 0
							&& (my_color == WHITE || mychesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = mychesses[i].x;
							command.y[0] = mychesses[i].y;
							command.x[1] = mychesses[i].x + 1;
							command.y[1] = mychesses[i].y + 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								mychess_image[board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(otherchesses[i].x - 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y - 1] == 0
							&& (other_color == BLACK || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x - 1;
							command.y[1] = otherchesses[i].y - 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(otherchesses[i].x - 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x - 1][otherchesses[i].y + 1] == 0
							&& (other_color == BLACK || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x - 1;
							command.y[1] = otherchesses[i].y + 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(otherchesses[i].x + 1, otherchesses[i].y - 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y - 1] == 0
							&& (other_color == WHITE || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x + 1;
							command.y[1] = otherchesses[i].y - 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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
						//����
						if (isInBound(otherchesses[i].x + 1, otherchesses[i].y + 1) && board_ids[otherchesses[i].x + 1][otherchesses[i].y + 1] == 0
							&& (other_color == WHITE || otherchesses[i].identify == king))
						{
							command.numStep = 2;
							command.x[0] = otherchesses[i].x;
							command.y[0] = otherchesses[i].y;
							command.x[1] = otherchesses[i].x + 1;
							command.y[1] = otherchesses[i].y + 1;
							//����һ������
							struct chess mychess_image[12], otherchess_image[12];
							memcpy(mychess_image, mychesses, sizeof(struct chess) * 12);
							memcpy(otherchess_image, otherchesses, sizeof(struct chess) * 12);
							int board_ids_image[LENGTH][LENGTH];
							memcpy(board_ids_image, board_ids, sizeof(int)*LENGTH*LENGTH);

							//����
							place(command, board_ids_image, mychess_image, otherchess_image);

							//�ж��Ƿ��Ϊ����
							if (ToKing(command.x[command.numStep - 1], command.y[command.numStep - 1]))
							{
								otherchess_image[-board_ids_image[command.x[command.numStep - 1]][command.y[command.numStep - 1]] - 1].identify = king;
							}

							//������һ��ѭ��
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

//������������
int estimate(const int board_ids[][LENGTH], const struct chess *mychesses, const struct chess *otherchesses)
{
	int score = 0;//����

	/*
	��ռ60�֣���ռ50��
	��Է�����Խ������ֵԽ��(����8��)
	���ڱ߽磬��ʾ��ǰ�Ƚϰ�ȫ����ֵ+10
	��������ܹ��ƶ�����ֵ+���ƶ��ķ�����X4
	*/
	for (int i = 0; i < 12; i++)
	{
		if (mychesses[i].life == alive)
		{
			//�ж����
			if (mychesses[i].identify == king)
				score += 60;
			else
			{
				score += 50;
				//�������
				if (my_color == BLACK)
				{
					//�����·�ԽԶ��ֵԽ��
					score += abs(mychesses[i].x - 7);
				}
				else
				{
					score += mychesses[i].x;
				}
			}
			if (mychesses[i].y == 0 || mychesses[i].y == 7)
				score += 5;
			//�ƶ�������
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
			//�ж����
			if (otherchesses[i].identify == king)
				score -= 60;
			else
			{
				score -= 50;
				//�������
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
			//�ƶ�������
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