#ifndef AI_H_
#define AI_H_

void CopyBoard(int newboard[][15], int board[][15]);

void GetRange(int board[][15], int *minRangeX, int *maxRangeX, int *minRangeY, int *maxRangeY);

int ValueTable(int count, int flag1, int flag2);

int CalculateValue(int board[][15], int turn, int x_pos, int y_pos);

int EvaluateHumanMove(int board[][15], int depth, int alpha, int beta, int newmoveX, int newmoveY);

int EvaluateComputerMove(int board[][15], int depth, int alpha, int beta, int newmoveX, int newmoveY);

#endif
