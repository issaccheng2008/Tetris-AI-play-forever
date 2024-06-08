#include <windows.h>
#include <iostream>
#include <string>
#include <iostream>
#include <math.h>
#include <bitset>
#include<algorithm>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <set>
#include <bitset>
#include <random>
#include <ctime>
#include <cstdlib>
#include <omp.h>
#include <cstring>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;
//need to change to fit specific need
const int w = 41;//the pixel width of each square
int bx = 298, by = -242;//the lower_left corner of the board
int px = 774, py = -1010;//the upper_left corner of the place where it display the next piece
int top_y = -1050;//the y value of the top of the board

//because Jstris becomes unreliable when the speed is this fast, it may take a few tries
//for my program to finish the 1000 lines run.
//(even if the platform has 99.9% accuracy per piece put, I would only have a 8% chance of finishing it.



const int S = 10, MOD = 4;//the width is 10. MOD is the highest legal height minus one.
const int MAX = 1048575;//the numbers of possible arrangements. calculated by pow(MOD, S) - 1
bool check_sheet[MAX+1], check[10];//storing if a state is legal or not.
int below[25][10];//the lower_side of each block at each oriantation
int above[25][10];//the upper_side of each block at each oriantation
int low[25];//storing the lowest position of each block at each oriantation
int rotation[25];//storing the number of times to press "up" to get to each oriantation
int point[10][2] = { {1,2},{3,6},{7,10},{11,11},{12,13},{14,17},{18,19},{0} };//marking where each blocks's information is stored
int pot[10] = { 0,2,1,3,6,5,4,7 };//storing the mirror image of each block
int tar, miror = 0, rm = 0;//which block I want to know the answer,if the current game is flipped,if the game was flipped at time of action
int curgame[15], lh = 7;//storing current game state, the hold at the beginning of current game
int nxt_piece;//next piece
bool repair, is_not_direct,rsw;//if the AI is filling up a 'hole',if the curpiece is put directly(not creat hole or fill hole),if the current game's 'flipness' is different from before
struct ret { int ori, x, y; }anss;//record the answer found
struct save_holes { int x, y, depth; bool ch; };//record the current hole's position, depth, and if there is a hole
struct ssor { int p, con; };//a struct made for sorting the current game by height, put the current block at the lowest place first.
bool cmp(ssor a, ssor b) { return a.con < b.con; }
bool dfs(int cur, int hold, save_holes hole);//finding answer
bool is_last_run;//check if it is the last block in the 7-block cycle
void put_piece(int i, int cur, bool* C, int hold, save_holes hole, bool follow) {//current piect,number of pieces put,if had found a answer,what the current piece in hold is, information about the hole,if is recreating the current piece
	if (!is_not_direct || follow == 0 || cur != tar - 1) {
		ssor* sor = new ssor[15]();//sorting the game by height
		for (int k = 1; k <= S + 2; k++)
			sor[k] = { k,curgame[k] };
		sort(sor + 1, sor + S + 3, cmp);

		if (follow && cur == tar - 1)//if I need to recreat the last game
			sor[1].p = anss.x + low[anss.ori];

		for (int t = point[i][0]; t <= point[i][1] && !*C; t++) {//go through all oriantation of current piece
			if (follow && cur == tar - 1)
				t = anss.ori;
			for (int jj = 1, cc = 1, j = 1; jj <= S + low[t] && !*C; jj++, cc = 1) {//go through all the positions to put the current piece
				j = sor[jj].p - low[t];
				if (j<1 || j>S)//if is outside the boundary
					continue;

				for (int k = 0; k <= 3; k++)//check if the current piece fit
					if (below[t][k] != 100 && curgame[j] + below[t][k] != curgame[j + k]) {
						cc = 0;
						break;
					}
				if (!cc)
					continue;

				for (int k = 0; k <= 3; k++)
					curgame[j + k] += above[t][k];
				*C = dfs(cur + 1, hold, hole);
				for (int k = 0; k <= 3; k++)
					curgame[j + k] -= above[t][k];
				if (tar == cur && *C)//if works
					anss = { t,j,curgame[j] }, rm = miror;
				if (is_last_run&&*C)//if works
					anss = { t,j,curgame[j] }, rm = miror;
			}
			if (follow && cur == tar - 1)
				break;
		}
		delete[] sor;
	}
	if (*C)//if can be put directly, then do not need to consider creating a hole
		return;
	for (int sw = 0; sw <= 1; sw++) {//go through the two oriantations
		if (hole.ch && sw == 0 && !(*C)) {//filling a hole
			for (int t = point[i][0], cc = 1,cc2=0; t <= point[i][1] && !*C; t++, cc = 1,cc2=0) {//oriantation
				if (above[t][0] != hole.depth)//check if can fit a hole
					continue;
				for (int k = 0; k <= 3; k++) {
					if (below[t][k] == 100)
						break;
					if ((k != 0 && hole.y + below[t][k] != curgame[hole.x + k]) ||
						below[t][k] + hole.y < curgame[hole.x + k + 1]) {
						cc = 0;
						break;
					}
					if (below[t][k] + hole.y == curgame[hole.x + k + 1])
						cc2 = 1;
				}
				if (cc == 0||!cc2)
					continue;

				for (int k = 1; k <= 3; k++)
					curgame[hole.x + k] += above[t][k];
				repair = 0,is_not_direct=0;
				*C = dfs(cur + 1, hold, { 0,0,0,0 });
				for (int k = 1; k <= 3; k++)
					curgame[hole.x + k] -= above[t][k];
				if (tar == cur && *C && !follow)
					anss = { t,hole.x,hole.y }, rm = miror, repair = 1,is_not_direct=1;
				if (is_last_run && *C)
					anss = { t,hole.x,hole.y }, rm = miror, repair = 1,is_not_direct=1;
			}
		}
		else
			if (cur <= 6 && !hole.ch && !(*C) && (i == 6 || i == 4)&&(!follow || sw == rsw || cur != tar - 1)) {
				for (int t = point[i][0]; t <= point[i][1] && !*C; t++) {//oriantation
					for (int j = 1, cc = 0; j <= S && !*C; j++, cc = 0) {//all places
						if (follow && tar - 1 == cur)//check if works
							t = anss.ori, j = anss.x;
						int lst;
						for (int k = 1; k <= 3; k++) {
							if (below[t][k] == 100)
								break;
							if (curgame[j] + below[t][k] < curgame[j + k]) {
								cc = 0; break;
							}
							if ((curgame[j] + below[t][k]) > curgame[j + k])
								cc++, lst = k;
						}
						if (cc != 1)
							continue;
						if (curgame[j + lst + 1] == curgame[j] + below[t][lst + 1])
							continue;
						if (curgame[j + lst] < curgame[j + lst + 1])
							continue;
						hole = { j + lst,curgame[j + lst],curgame[j] + below[t][lst] - curgame[j + lst],1 };
						if (hole.depth > 4) { hole.ch = 0; continue; }

						curgame[j + lst] = curgame[j] + below[t][lst] + above[t][lst];
						for (int k = 0; k < lst; k++)
							curgame[j + k] += above[t][k];
						is_not_direct = 0;
						*C = dfs(cur + 1, hold, hole);
						hole.ch = 0;
						curgame[j + lst] -= above[t][lst] + hole.depth;
						for (int k = 0; k < lst; k++)
							curgame[j + k] -= above[t][k];
						if (tar == cur && *C)
							anss = { t,j,curgame[j] }, rm = miror,is_not_direct=1,rsw=sw;
						if (is_last_run && *C)
							anss = { t,j,curgame[j] }, rm = miror,is_not_direct=1,rsw=sw;
						if (follow && tar - 1 == cur)
							break;
					}
					if (follow && tar - 1 == cur)
						break;
				}
			}
		miror ^= 1;//flipping the game
		for (int k = 1; k <= S / 2; k++)
			swap(curgame[k], curgame[S - k + 1]);
		bool* ncheck = new bool[10]();
		for (int k = 0; k < 7; k++)
			ncheck[k] = check[pot[k]];
		for (int k = 0; k < 7; k++)
			check[k] = ncheck[k];
		delete[] ncheck;
		i = pot[i];
		hold = pot[hold];
		hole.x = S - hole.x + 1;
	}
} 




int curcolor[3];
int board_color[3] = { 0,0,0 };
int piece_color[3] = { 0,0,0 };
bool check_end = 1;//since I set the operating screen function runs on a different thread, I use this to check if the last operation has finished
int start[7][4] = {//record the starting position of each piece at each oriantation in Jstris
	{4, 6, 3, 5},
	{4, 5, 4, 4},
	{4, 5, 4, 4},
	{5, 5, 5, 5},
	{4, 5, 4, 4},
	{4, 5, 4, 4},
	{4, 5, 4, 4}
};
const int ww = 2, yy = 4;
int holdd = 7;//record the piece in hold at the start(the pieces are from 0-6,7 means nothing)
void getcolor(int x, int y,int* curcolorr) {//get the pixel color at (x,y), and store it in curcolorr
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, 1, 1);
	SelectObject(hdcMem, hbmScreen);
	BitBlt(hdcMem, 0, 0, 1, 1, hdcScreen, x, y, SRCCOPY);
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 1;
	bmi.bmiHeader.biHeight = -1;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	struct {
		BITMAPINFOHEADER bmiHeader;
		RGBTRIPLE color;
	} dib = { bmi.bmiHeader };
	if (GetDIBits(hdcMem, hbmScreen, 0, 1, &dib.color, (BITMAPINFO*)&dib, DIB_RGB_COLORS)) {
		curcolorr[0] = (int)dib.color.rgbtRed;
		curcolorr[1] = (int)dib.color.rgbtGreen;
		curcolorr[2] = (int)dib.color.rgbtBlue;
	}
	DeleteObject(hbmScreen);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
}

void press_key(WORD key, bool ch) {//press key. ch checks if the key is 'down'
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key;
	input.ki.wScan = 0;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	if (ch)
		Sleep(10);//for some reason, when I press the down key for the same time as the others, the program crashes
	SendInput(1, &input, sizeof(INPUT));
}
int arr[ww][yy];
int color[10][3] = {//store color for each piece, uses color to recognize what piece it is.
	{15, 155, 215},
	{33, 65, 198},
	{227, 91, 2},
	{227, 159, 2},
	{89, 177, 1},
	{175, 41, 138},
	{215, 15, 55}
};
int optt[25] = {0,1,2,7,10,9,8,3,6,5,4,11,0,0,14,17,16,15};//store the miror piece of each piece at each oriantation
int in_put(int pplace) {//get which piece it is
	int* curcolorr = new int[3]();
	getcolor(px + w, py+3*pplace*w,curcolorr);
	bool ch = 1;
	for (int k = 0; k < 3; k++)
		if (curcolorr[k] != piece_color[k]) {
			ch = 0; break;
		}
	if (ch) {
		getcolor(px + w, py + w+ 3 * pplace * w,curcolorr);
		for (int k = 0; k < 3; k++)
			if (curcolorr[k] != piece_color[k]) {
				ch = 0; break;
			}
		if (ch)
			in_put(pplace);
	}
	for (int i = 0; i < 7; i++) {
		ch = 1;
		for (int k = 0; k < 3; k++)
			if (curcolorr[k] != color[i][k]) {
				ch = 0; break;
			}
		if (ch)
			return i;
	}
}
int rmm, curr, mini;//since operating the keyboard and finding the next answer run on two different threads, I dublicated them to avoid memory leakage
ret ansss;
bool repairr;

int thick[7][4] = {//storing how thick each piece is at each oriantation
	{3,0},
	{2,1,2,1},
	{2,1,2,1},
	{1},
	{2,1,2,1},
	{2,1,2,1},
	{2,1,2,1}
};
void pr(bool hold) {//operating screen
	if (hold)
		printf("hold\n");
	if (hold) {
		press_key('C', 0);//hold
		if(!rmm)
			swap(curr, holdd);
		else {
			int tholdd = holdd;
			holdd = pot[curr];
			curr = pot[tholdd];
		}
	}
	if (curr == 7) {
 		check_end = 1;
		return;
	}
	printf("%d\n", curr);
	int tarx, tary, oriantation;
	 if (rmm) {//if current game is mirrored
		oriantation = optt[ansss.ori], curr = pot[curr];
		tarx = S - ansss.x + 1-thick[curr][rotation[oriantation]];
	 }
	else
		tarx = ansss.x, oriantation = ansss.ori;
	int stro = rotation[oriantation];
	tary = ansss.y - mini;
	printf("%d %d\n", tarx, tary);
	for (int i = 1; i <= stro; i++)//moving to right oriantation
		press_key(VK_UP, 0);
	int cplace = start[curr][stro];
	while (cplace < tarx)//move right
		press_key(VK_RIGHT, 0), cplace++;
	while (cplace > tarx)//move left
		press_key(VK_LEFT, 0), cplace--;
	bool chd = 0;
	if (repairr) {//if is repairing hole
		if (rmm)
			tarx--, press_key(VK_LEFT, 0);
		else
			tarx++, press_key(VK_RIGHT, 0);
		chd = 0;
		while (1) {
			press_key(VK_DOWN, 1);//soft land
			getcolor(tarx * w + bx - w, by - w * tary,curcolor);
			for (int k = 0; k < 3; k++)
				if (curcolor[k] != board_color[k]) {
					chd = 1; break;
				}
			if (chd)
				break;
		}
		if (rmm)
			press_key(VK_RIGHT, 0);
		else
			press_key(VK_LEFT, 0);
	}
	press_key(VK_SPACE, 0);//hard land
	Sleep(1);//increase for precision
	check_end = 1;
	return;
}
int store_piece[10], curp = 5;//since the platform shows 5 pieces at a time, I sped up the process by reading 5 pieces at a time
void prepare_for_operate_screen() {
	int tt = 1e9;
	for (int i = 1; i <= S; i++)
		tt = min(tt, curgame[i]);
	for (int i = 1; i <= S; i++)
		printf("%d ",curgame[i]-tt);
	printf("\n");
	while (!check_end)
		;
	if (curp == 5) {
		Sleep(40);
#pragma omp parallel for
		for (int i=0;i<5;i++)
			store_piece[i]=in_put(i);
		curp = 0;
	}
	curr = nxt_piece;
	nxt_piece = store_piece[curp++];
	if (rm)
		nxt_piece = pot[nxt_piece];
	ansss = anss, rmm = rm, repairr = repair, mini = tt;
	check_end = 0;
}




int recreate = 0;
bool dfs(int cur, int hold, save_holes hole) {//searching for legal move
	bool C = 1;
	if (cur >= 8) {//if have already put all pieces
		if (hold <= 6) {//if there is a piece in 'hold' position
			if (cur == tar)
				is_last_run = 1;
			C = 0, put_piece(hold, cur - 1, &C, 7, hole, 0);//press C, and put the first piece in the next cycle in the 'hold' position
			is_last_run = 0;
			if (cur == tar && C) {
				lh = rm?pot[nxt_piece]:nxt_piece;
				prepare_for_operate_screen();
				//pr(1);
				thread screenThread(pr, 1);//'1' means press hold
				screenThread.join();
				is_not_direct = repair = 0;
				return 1;
			}
		}
		else {
			if (hole.ch)//check if the state that the 7-piece cycle landed on was legal
				return 0;
			int mini = 1e9, mx = 0;
			for (int i = 1; i <= S; i++)
				mini = min(mini, curgame[i]), mx = max(mx, curgame[i]);
			if (mx - mini > MOD - 1)
				return 0;
			int total = 0;
			for (int i = 1; i <= S; i++)
				total = total * MOD + curgame[i] - mini;
			if (check_sheet[total] == 0)
				return 0;
			if (cur == tar)
				recreate = total;
			return 1;
		}
	}
	if (hole.ch && hole.y < curgame[hole.x + 1])//do a precheck to speed things up
		return 0;
	int mx = 0, tot = 0;
	for (int i = 1; i <= S; i++)
		mx = max(mx, curgame[i]);
	if (mx > 3) {
		mx -= 3;
		for (int i = 1; i <= S; i++)
			tot += max(0, mx - curgame[i]);
		if (hole.ch && hole.y <= mx)
			tot += min(mx, hole.y + hole.depth) - hole.y;
		if (tot > (7 + (hold < 7) - cur + 1) * 4)
			return 0;
	}

	int curpiece;
	if (cur == tar) {
		curpiece = nxt_piece;
		C = 0;
		check[curpiece] = 1;
		put_piece(curpiece, cur, &C, hold, hole, 0);//search first
		if (C) {//if putting directly is ok
			prepare_for_operate_screen();
			//pr(0);
			thread screenThread(pr, 0);
			tar++, C = 0, put_piece(curpiece, cur, &C, hold, hole, 1);//recreat the current game, and move on
			check[curpiece] = 0;
			screenThread.join();
			return 1;
		}

		if (hold <= 6)//if there is already a piece in 'hold'
			put_piece(hold, cur, &C, curpiece, hole, 0);
		else {
			prepare_for_operate_screen();
			//pr(1);
			thread screenThread(pr, 1);
			tar++, C=dfs(cur + 1, curpiece, hole);
			check[curpiece] = 0;
			screenThread.join();
			return 1;
		}
		if (tar == cur) {
			prepare_for_operate_screen();
			//pr(1);
			thread screenThread(pr, 1);
			tar++, C = 0;
			put_piece(hold, cur, &C, curpiece, hole, 1);
			check[curpiece] = 0;
			screenThread.join();
			return 1;
		}
	}
	else
		for (int i = 0; i < 7 && C; i++)
			if (!check[i]) {
				C = 0;
				check[i] = 1;
				put_piece(i, cur, &C, hold, hole, 0);
				if (hold <= 6)
					put_piece(hold, cur, &C, i, hole, 0);
				else
					C = dfs(cur + 1, i, hole);
				check[i] = 0;
			}
	return C;
}
int main() {
	optt[12] = 18, optt[18] = 12;
	optt[13] = 19, optt[19] = 13;
	ifstream file("tetrominoes.txt");//load in all the information about the tetrominoes
	for (int i = 1; i <= 19; i++) {
		for (int j = 1; j <= 3; j++)
			file >> below[i][j];
		for (int j = 0; j < 4; j++)
			file >> above[i][j];
		file >> low[i] >> rotation[i];
	}
	int ct;
	for (int i = 0; i <= MAX; i++)
		check_sheet[i] = 1;
	ifstream file2("input.txt");//load in all the illegal states
	while (file2 >> ct)
		check_sheet[ct] = 0;//and set them to zero
	curgame[0] = curgame[S + 1] = curgame[S + 2] = 1000;
	int rlh;
	cin >> nxt_piece;
	Sleep(500);//time for reaction
	while (1) {
		if (rm) {
			rm = 0,miror=0;
			for (int i = 1; i <= S; i++)
				curgame[i] = recreate % MOD, recreate /= MOD;
			lh = pot[lh];
			nxt_piece = pot[nxt_piece];
		}
		else
			for (int i = S; i >= 1; i--)
				curgame[i] = recreate % MOD, recreate /= MOD;
		tar = lh < 7 ? 2 : 1;
		check[lh] = 1;
		rlh = lh;
		cout << dfs(tar, lh, { 0,0,0,0 });
		check[rlh] = 0;
		printf("%d change_round\n",tar);
	}
	return 0;
}