#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE  
#endif

#include <iostream>
#include <conio.h> 
#include <cstring> 
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <time.h>
#include "Utils.h"

class Map;
class Mine;

class Map {
private:
	int		width;
	int		height;
	int		size;
	int		mouse_x;
	int		mouse_y;
	int*	nearMines;
	char*	canvas;
	bool*	isMine;
	bool*	isOpened;

public:
	Map(int width, int height)
		: width(width), height(height), canvas(new char[(width + 1) * height]), mouse_x(-1), mouse_y(-1)
	{
		bool faultInput = false;
		if (this->width <= 0) {
			this->width = 10;
			faultInput = true;
		}
		if (this->height <= 0) {
			this->height = 10;
			faultInput = true;
		}
		size = (this->width + 1) * this->height;
		if (faultInput == true) {
			delete canvas;
			canvas = new char[size];
		}
		isMine = new bool[size];
		isOpened = new bool[size];
		nearMines = new int[size];
		memset(isMine, false, size);
		memset(isOpened, false, size);
		memset(nearMines, 0, sizeof(int) * size);
	}
	~Map()
	{
		delete[] nearMines;
		delete[] isOpened;
		delete[] isMine;
		delete[] canvas;
	}
	int getWidth() {
		return width;
	}
	int getHeight() {
		return height;
	}
	void clear()
	{
		memset(canvas, ' ', size);
		
		for (int i = 0; i < height; i++)
		{
			isMine[(width + 1) * (i + 1) - 1] = false;
			isOpened[(width + 1) * (i + 1) - 1] = false;
			nearMines[(width + 1) * (i + 1) - 1] = -1;
		}
	}
	void draw();
	void update(bool& isLooping, Mine* mine);
	void setMine(int mine_pos) {
		isMine[mine_pos] = true;
	}
	void setMouseClickPos(int mouse_x, int mouse_y){
		this->mouse_x = mouse_x;
		this->mouse_y = mouse_y;
	}
	int clickedMousePosIndex() {
		return mouse_x + (mouse_y * 11);
	}
	void checkNearMines(int index) {
		if (index < 0 && index > size - 1) return;
		if (isMine[index] == true) return;
		if (isOpened[index] == true) return;
		isOpened[index] = true;
		int num_of_near_mines = 0;
		for (int i = 0; i < 3; i++)	{
			if (index - 10 - i > 0 && index - 10 - i < size - 1) {
				if (isMine[index - 10 - i] == true && (index - 10 - i) > 0 && (index - 10 - i) < size)
					++num_of_near_mines;
			}
		}
		for (int i = 0; i < 3; i++)	{
			if (index + 1 - i > 0 && index + 1 - i < size - 1) {
				if (isMine[index + 1 - i] == true && (index + 1 - i) > 0 && (index + 1 - i) < size)
					++num_of_near_mines;
			}
		}
		for (int i = 0; i < 3; i++)	{
			if (index + 12 - i > 0 && index + 12 - i < size - 1) {
				if (isMine[index + 12 - i] == true && (index + 12 - i) > 0 && (index + 12 - i) < size)
					++num_of_near_mines;
			}
		}
		nearMines[index] = num_of_near_mines;
	}
	void setNearBlocks(int index) {
		for (int i = 0; i < 3; i++) {
			checkNearMines(index - 10 - i);
		}
		for (int i = 0; i < 3; i++) {
			checkNearMines(index + 1 - i);
		}
		for (int i = 0; i < 3; i++) {
			checkNearMines(index + 12 - i);
		}
	}
	bool checkWin(Mine* mine);
	void render()
	{
		Borland::gotoxy(0, 0); 
		for (int h = 0; h < height; h++)
			canvas[(width + 1) * (h + 1) - 1] = '\n';
		canvas[size - 1] = '\0';
		printf("%s", canvas);
	}
};

class Mine
{
private:
	int		num_mines;
	int*	mine_pos;

public:
	Mine()
	{
		srand((unsigned)time(NULL));
		num_mines = rand() % 10 + 10 + 1;
		mine_pos = new int[num_mines]; 
	}
	~Mine()
	{
		for (int i = 0; i < num_mines; i++)
		{
			mine_pos[i] = NULL;
		}
		delete[] mine_pos;
	}
	void settingMine(Map* map) {
		int i = 0;
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		while (i < num_mines)
		{
			int k = 0;
			int rand_num = rand() % 100;

			for (int j = 0; j < map->getHeight(); j++) {
				if (rand_num == ((map->getWidth() + 1) * (j + 1) - 1)) {
					k = 1;
					break;
				}
			}
			if (k == 1) continue;
			for (int j = 0; j < i; j++){
				if (mine_pos[j] == rand_num) {
					rand_num = rand() % 100;
				}
			}
			mine_pos[i] = rand_num;
			i++;
		}
		for (int j = 0; j < num_mines; j++)
		{
			map->setMine(mine_pos[j]);
		}
		printf("number of mines : %d\n", num_mines);
		for (int i = 0; i < num_mines; i++)
		{
			printf("%d ", mine_pos[i]);
		}
	}
	int getMinePos(int i)
	{
		int pos = mine_pos[i];
		return pos;
	}
	int getMineNum()
	{
		return num_mines;
	}
};

bool Map::checkWin(Mine* mine) {
	int num = 0;
	for (int i = 0; i < size; i++) {
		if (isOpened[i] == false)
		{
			num++;
		}
	}
	if (num == mine->getMineNum() + 10) {
		return true;
	}
	else return false;
}

void Map::update(bool& isLooping, Mine* mine)
{
	if (clickedMousePosIndex() < 0) return;
	for (int i = 0; i < height; i++) {
		if (clickedMousePosIndex() == (width + 1) * (i + 1) - 1)
			return;
	}
	if (isMine[clickedMousePosIndex()] == true) {
		canvas[clickedMousePosIndex()] = '*';
		isLooping = false;
		return;
	}
	if (checkWin(mine)) isLooping = false;
	if (isOpened[clickedMousePosIndex()] == true) return;
	setNearBlocks(clickedMousePosIndex());
}

void Map::draw() {
	for (int i = 0; i < size; i++)
	{
		canvas[i] = '@';
		if (isMine[i] == true)
			canvas[i] = '*';
		/*if (isMine[clickedMousePosIndex()] == true)
		{
			if(isMine[i] == true)
				&canvas[i] = '*';
		}*/
		if (isOpened[i] == true)
		{
			if (nearMines[i] >= 0) {
				canvas[i] = nearMines[i] + '0';
			}
		}
	}
}

static HANDLE hStdin;
static DWORD fdwSaveOldMode;
static void ErrorExit(const char*);
static void MouseEventProc(MOUSE_EVENT_RECORD, Map*);

int main()
{
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	Map map(10, 10);
	Mine mine;
	
	mine.settingMine(&map);

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
		ErrorExit("GetStdHandle");
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		ErrorExit("GetConsoleMode");
	fdwMode = ENABLE_EXTENDED_FLAGS;
	if (!SetConsoleMode(hStdin, fdwMode))
		ErrorExit("SetConsoleMode");
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode))
		ErrorExit("SetConsoleMode");

	// game loop
	bool isLooping = true;
	while (isLooping) {
		map.clear();	
		if (GetNumberOfConsoleInputEvents(hStdin, &cNumRead)) {
			if (cNumRead > 0) {
				if (!ReadConsoleInput(
					hStdin,  
					irInBuf,     
					128,         
					&cNumRead))
					ErrorExit("ReadConsoleInput");
				for (i = 0; i < cNumRead; i++)
				{
					switch (irInBuf[i].EventType)
					{
					case MOUSE_EVENT:
						MouseEventProc(irInBuf[i].Event.MouseEvent, &map);
						break;
					/*default:
						ErrorExit("Unknown event type");
						break;*/
					}
				}
			}
		}
		map.update(isLooping, &mine);
		map.draw();
		map.render();
	}
	if (map.checkWin(&mine)) {
		printf("\n\nYou Win\n\n");
	}
	else {
		printf("\n\nGame Over\n\n");
	}

	SetConsoleMode(hStdin, fdwSaveOldMode);
	
	return 0;
}

void ErrorExit(const char* lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

void MouseEventProc(MOUSE_EVENT_RECORD mer, Map* map)
{
	Borland::gotoxy(0, 12);
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
	switch (mer.dwEventFlags)
	{
	case 0:
		if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			if(mer.dwMousePosition.X >= 0 && mer.dwMousePosition.X < 10 && mer.dwMousePosition.Y >= 0 && mer.dwMousePosition.Y < 10)
				map->setMouseClickPos(mer.dwMousePosition.X, mer.dwMousePosition.Y);
		}
		break;
	default:
		break;
	}
	Borland::gotoxy(0, 0);
}