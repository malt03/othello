#include <iostream>
#include <sys/time.h>
using namespace std;

#define CUL_DEPTH 5
#define MAP_SIZE 8
#define B 0
#define W 1
#define N -1
#define DRAW -2

#define FLIPABLE_C 5
#define SETTLED_C 20
#define POINT_C 1

void firstInit(), gameInit(int&,int**,int&), draw(int**,int), turnChange(int&), culBestPos(int**,int,int&,int&,int), flipOneVec(int,int,int**,int,int), copyMap(int**,int**), AI(int**,int,int);
bool checkFlipOneVec(int,int,int**,int,int), drawStone(int), checkFlip(int,int,int**,int), flip(int,int,int**,int);
int flipableCount(int**,int), culMinPos(int**,int,int&,int&), whichWin(int**), culEvaluation(int**,int,int,int), countSettledStone(int**,int), stonePoint(int**,int), culBestPosRe(int**,int,int,int,int);


int vec_x[8] = {-1,-1,0,1,1,1,0,-1};
int vec_y[8] = {0,1,1,1,0,-1,-1,-1};

int point_table[MAP_SIZE][MAP_SIZE] = {
  { 60, 10,  0, -2, -2,  0,-10, 60},
  {-10,-90, -3, -3, -3, -3,-90,-10},
  {  0, -3,  0, -1, -1,  0, -3,  0},
  { -2, -3, -1, -1, -1, -1, -3, -2},
  { -2, -3, -1, -1, -1, -1, -3, -2},
  {  0, -3,  0, -1, -1,  0, -3,  0},
  {-10,-90, -3, -3, -3, -3,-90,-10},
  { 60, 10,  0, -2, -2,  0,-10, 60},
};

int map0[MAP_SIZE][MAP_SIZE];
int isAI = true;
timeval time_val;

int main(){
  gettimeofday(&time_val, NULL);
  cout << "start:" << time_val.tv_sec << "." << time_val.tv_usec/1000 << endl;
  firstInit();
  int input, turn, input_x, input_y, move;
  int** map = new int*[MAP_SIZE];
  for(int i=0;i<MAP_SIZE;i++) map[i] = new int[MAP_SIZE];

  while(true){
	gameInit(turn, map, move);

	while(true){
	  cout << culEvaluation(map, turn, move, turn) << endl;
	  draw(map, turn);
	  do{
		do{
		  input = 54;
		  //cin >> input;
		}while(input%10>MAP_SIZE || input/10>MAP_SIZE);
	  }while(!checkFlip(input%10,input/10,map,!turn));
	  input_x = input % 10;
	  input_y = input / 10;

	  flip(input_x,input_y,map,turn);
	  map[input%10][input/10] = turn;

	  move ++;

	  if(flipableCount(map,turn) != 0){
		turnChange(turn);
		
		if(isAI){
		  cout << culEvaluation(map, turn, move, turn) << endl;

		  do{
			draw(map, turn);
			AI(map, turn, move);
		  }while(flipableCount(map,turn)==0);
		  return 0;
		  turnChange(turn);
		  move ++;
		}
	  }
	}
  }
}

void firstInit(){
  for(int i=0;i<MAP_SIZE;i++)
	for(int j=0;j<MAP_SIZE;j++)
	  map0[i][j] = N;
  map0[3][3] = W;
  map0[3][4] = B;
  map0[4][3] = B;
  map0[4][4] = W;
}

void gameInit(int& turn, int** map, int& move){
  for(int i=0;i<MAP_SIZE;i++)
	for(int j=0;j<MAP_SIZE;j++)
	  map[i][j] = map0[i][j];
  turn = B;
  move = 0;
}

void turnChange(int& turn){
  turn = !turn;
}

void draw(int** map, int turn){
  cout << "  ";
  for(int i=0;i<MAP_SIZE;i++)
	cout << " " << i;
  cout << endl;
  for(int i=0;i<MAP_SIZE;i++){
	cout << i << " ";
	for(int j=0;j<MAP_SIZE;j++){
	  if(!drawStone(map[i][j])){
		if(checkFlip(i,j,map,!turn)) cout << " !";
		else						 cout << "  ";
	  }
	}
	cout << endl;
  }

  cout << endl;
}

bool drawStone(int stone){
  switch(stone){
  case W: cout << " ●"; return true;
  case B: cout << " ○"; return true;
  default: return false;
  }
}

bool checkFlip(int x0, int y0, int** map, int enemy){
  for(int i=0;i<8;i++){
	if(checkFlipOneVec(x0,y0,map,enemy,i)) return true;
  }
  return false;
}

bool checkFlipOneVec(int x0, int y0, int** map, int enemy, int vec){
  if(map[x0][y0] != N) return false;

  int x=x0, y=y0;
  bool flag = false;
  while(true){
	x += vec_x[vec];
	y += vec_y[vec];

	if(x<0 || y<0|| x>=MAP_SIZE || y>=MAP_SIZE) return false;
	if(map[x][y] == N) return false;
	if(map[x][y] == enemy){
	  flag = true;
	  continue;
	}

	if(flag) return true;
	return false;
  }
}

bool flip(int x0, int y0, int** map, int turn){
  bool is_flip = false;
  for(int i=0;i<8;i++){
	if(checkFlipOneVec(x0,y0,map,!turn,i)){
	  flipOneVec(x0,y0,map,turn,i);
	  is_flip = true;
	}
  }
  return is_flip;
}

void flipOneVec(int x0, int y0, int** map, int turn, int vec){
  int x = x0 + vec_x[vec];
  int y = y0 + vec_y[vec];

  do{
	map[x][y] = turn;
	x += vec_x[vec];
	y += vec_y[vec];
  }while(map[x][y]==!turn);
}

int flipableCount(int** map, int enemy){
  int n = 0;
  for(int i=0;i<MAP_SIZE;i++)
	for(int j=0;j<MAP_SIZE;j++)
	  if(checkFlip(i,j,map,enemy)) n++;

  return n;
}

void culBestPos(int** map, int turn, int& ans_x, int& ans_y, int move){
  int point_map[MAP_SIZE][MAP_SIZE] = {
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
	{-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000},
  };
  bool flag = true;
  int *** cul_map = new int**[MAP_SIZE*MAP_SIZE];
  for(int i=0;i<MAP_SIZE*MAP_SIZE;i++){
	cul_map[i] = new int*[MAP_SIZE];
	for(int k=0;k<MAP_SIZE;k++)cul_map[i][k] = new int[MAP_SIZE];
  }
  
#pragma omp parallel for
  for(int n=0;n<MAP_SIZE*MAP_SIZE;n++){
	int i = n / MAP_SIZE;
	int j = n % MAP_SIZE;
	copyMap(map, cul_map[n]);
	  
	if(flip(i,j,cul_map[n],turn)){
	  /*time*/
	  gettimeofday(&time_val, NULL);
	  cout << n << "-start :" << time_val.tv_sec << "." << time_val.tv_usec/1000 << endl;
	  /*end*/
	  
	  cul_map[n][i][j] = turn;
	  point_map[i][j] = culBestPosRe(cul_map[n], !turn, 0, turn, move);

	  /*time*/
	  gettimeofday(&time_val, NULL);
	  cout << n << "-finish:" << time_val.tv_sec << "." << time_val.tv_usec/1000 << endl;
	  /*end*/
	}
  }

  int max_point = -1000;
  for(int i=0;i<MAP_SIZE;i++){
	for(int j=0;j<MAP_SIZE;j++){
	  if(point_map[i][j] > max_point){
		max_point = point_map[i][j];
		ans_x = i;
		ans_y = j;
	  }
	}
  }
}

int culBestPosRe(int** map, int turn, int depth, int culc_part, int move){
  if(CUL_DEPTH==depth || depth+move>=60)
	return culEvaluation(map, move+depth, turn,  culc_part);
  else{
	int** cul_map = new int*[MAP_SIZE];
	for(int i=0;i<MAP_SIZE;i++)cul_map[i] = new int[MAP_SIZE];
	copyMap(map, cul_map);

	int point;
	if(turn == culc_part)	point = -1000;
	else					point = 1000;

	for(int i=0;i<MAP_SIZE;i++){
	  for(int j=0;j<MAP_SIZE;j++){
		if(flip(i,j,cul_map,turn)){
		  cul_map[i][j] = turn;
		  int point_t;
		  if(flipableCount(cul_map,turn) == 0)
			point_t = culBestPosRe(cul_map, turn, depth+1, culc_part, move);
		  else
			point_t = culBestPosRe(cul_map, !turn, depth+1, culc_part, move);

		  if(turn==culc_part && point_t>point)		point = point_t;
		  else if(turn!=culc_part && point_t<point)	point = point_t;
		}
		copyMap(map, cul_map);
	  }
	}
	return point;
  }
}

int culEvaluation(int** map, int turn, int move, int culc_part){
  if(move >= 60){
	if(whichWin(map) == turn)	return 1000;
	else						return -1000;
  }

  int ans_x, ans_y;
  int flipable = flipableCount(map, !turn);
  flipable -= culMinPos(map, turn, ans_x, ans_y);
  
  int settled_stone = countSettledStone(map, turn);

  int stone_point = stonePoint(map, turn);

  return flipable*FLIPABLE_C+settled_stone*SETTLED_C+stone_point*POINT_C;
}

int countSettledStone(int** map, int turn){
  int settled_stone = 0;
  int start_x[4] = {0,0,7,7};
  int start_y[4] = {0,7,0,7};
  int dir_x[8] = {1,0,1,0,-1,0,-1,0};
  int dir_y[8] = {0,1,0,-1,0,1,0,-1};

  for(int i=0;i<4;i++){
	int stone = map[start_x[i]][start_y[i]];
	if(stone != N){
	  for(int j=0;j<2;j++){
		for(int k=1;k<MAP_SIZE-2;k++){
		  if(map[start_x[i]+dir_x[i*2+j]*k][start_y[i]+dir_y[i*2+j]*k] == stone){
			if(stone == turn)	settled_stone ++;
			else				settled_stone --;
		  }
		}
	  }
	  if(map[start_x[i]+dir_x[i*2]][start_y[i]+dir_y[i*2+1]] == stone){
		if(stone == turn)	settled_stone += 90/SETTLED_C + 1;
		else				settled_stone -= 90/SETTLED_C + 1;
	  }
	}
  }

  return settled_stone;
}

int stonePoint(int** map, int turn){
  int point = 0;
  for(int i=0;i<MAP_SIZE;i++){
	for(int j=0;j<MAP_SIZE;j++){
	  if(map[i][j] == turn)		point += point_table[i][j];
	  else if(map[i][j] != N)	point -= point_table[i][j];
	}
  }

  return point;
}

int culMinPos(int** map, int turn, int& ans_x, int& ans_y){
  int** cul_map = new int*[MAP_SIZE];
  for(int i=0;i<MAP_SIZE;i++)cul_map[i] = new int[MAP_SIZE];
  copyMap(map, cul_map);

  int min_flipable = 99;
  for(int i=0;i<MAP_SIZE;i++){
	for(int j=0;j<MAP_SIZE;j++){
	  if(flip(i,j,cul_map,turn)){
		cul_map[i][j] = turn;
		int flipable = flipableCount(cul_map, turn);
		if(flipable < min_flipable){
		  min_flipable = flipable;
		  ans_x = i;
		  ans_y = j;
		}
		copyMap(map, cul_map);
	  }
	}
  }
  return min_flipable;
}

void copyMap(int** map0, int** map1){
  for(int i=0;i<MAP_SIZE;i++)
	for(int j=0;j<MAP_SIZE;j++)
	  map1[i][j] = map0[i][j];
}

void AI(int** map, int turn, int move){
  int x, y;
  culBestPos(map, turn, x, y, move);
  flip(x,y,map,turn);
  map[x][y] = turn;
}

int whichWin(int** map){
  int white=0, black=0;
  for(int i=0;i<MAP_SIZE;i++)
	for(int j=0;j<MAP_SIZE;j++){
	  switch(map[i][j]){
	  case B: black++; break;
	  case W: white++; break;
	  default: break;
	  }
	}
  if(black > white)			return W;
  else if(white > black)	return B;
  else						return DRAW;
}
