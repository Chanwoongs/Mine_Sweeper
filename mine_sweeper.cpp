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

class Input;
class Map;
class Mine;

class Input
{
	DWORD cNumRead, fdwMode;
	INPUT_RECORD irInBuf[128];
	HANDLE hStdin;
	DWORD fdwSaveOldMode;
	void errorExit(const char*);

	static Input* Instance;

	Input()
	{
		hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (hStdin == INVALID_HANDLE_VALUE)
			errorExit("GetStdHandle");
		if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
			errorExit("GetConsoleMode");
		fdwMode = ENABLE_EXTENDED_FLAGS;
		if (!SetConsoleMode(hStdin, fdwMode))
			errorExit("SetConsoleMode");
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(hStdin, fdwMode))
			errorExit("SetConsoleMode");
	}
	~Input()
	{
		SetConsoleMode(hStdin, fdwSaveOldMode);
	}
public:

	static Input* GetInstance() {
		if (Instance == nullptr) {
			Instance = new Input;
		}
		return Instance;
	}

	void readInputs(Map* map) {
		if (!GetNumberOfConsoleInputEvents(hStdin, &cNumRead)) {
			cNumRead = 0;
			return;
		}
		if (cNumRead == 0) return;

		if (!ReadConsoleInput(
			hStdin,
			irInBuf,
			128,
			&cNumRead))
			errorExit("ReadConsoleInput");
	}

	bool getMouseButtonDown(int button);
	void getMousePosition(int& x, int& y);
};
Input* Input::Instance = nullptr;

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
	bool*	isWall;
	Input*	input;

public:
	Map(int width, int height)
		: width(width), height(height), canvas(new char[(width + 1) * height]), mouse_x(-1), mouse_y(-1), input(Input::GetInstance())
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
		isWall = new bool[size];
		memset(isMine, false, size);
		memset(isOpened, false, size);
		memset(nearMines, -1, sizeof(int) * size);
		memset(isWall, false, size);
	}
	~Map()
	{
		delete[] isWall;
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
			isWall[(width + 1) * (i + 1) - 1] = true;
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
		if (isWall[index] == true) return;
		if (isOpened[index] == false) isOpened[index] = true;
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
		if (nearMines[index] != -1) return;
		nearMines[index] = num_of_near_mines;
		if (nearMines[index] > 0) isWall[index] = true;
	}
	void setNearBlocks(int index) {

		if (nearMines[index] > -1) return;

		checkNearMines(index);

		if (nearMines[index] == 0) {
			setNearBlocks(index - 11);
			setNearBlocks(index - 1);
			setNearBlocks(index + 1);
			setNearBlocks(index + 11);
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
		delete[] mine_pos;
	}
	void settingMine(Map* map) {
		int i = 0;
		int z = 0;
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		
		for (int i = 0; i < num_mines; i++)
		{
			int k = 0;
			int rand_num = rand() % 110;

			for (int j = 0; j < map->getHeight(); j++) {
				if (rand_num == ((map->getWidth() + 1) * (j + 1) - 1)) {
					k = 1;
					i--;
					break;
				}
			}
			if (k == 1) continue;

			for (int j = 0; j < i; j++) {
				if (mine_pos[j] == rand_num) {
					while (mine_pos[j] == rand_num) {
						rand_num = rand() % 110;
					}
				}
			}
			mine_pos[i] = rand_num;
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
	if (input->getMouseButtonDown(1)) {
		input->getMousePosition((this->mouse_x), (this->mouse_y));
	}
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

		if (isMine[clickedMousePosIndex()] == true)
		{
			if(isMine[i] == true)
				canvas[i] = '*';
		}
		if (isOpened[i] == true)
		{
			if (nearMines[i] >= 0) {
				canvas[i] = nearMines[i] + '0';
			}
		}
	}
}

int main()
{
	Input* input = Input::GetInstance();
	Map map(10, 10);
	Mine mine;
	
	mine.settingMine(&map);

	// game loop
	bool isLooping = true;
	while (isLooping) {
		map.clear();	
		input->readInputs(&map);
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
	
	return 0;
}

void Input::errorExit(const char* lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

bool Input::getMouseButtonDown(int button) {
	if (cNumRead == 0) return false;

	for (int i = 0; i < cNumRead; i++)
	{
		if (irInBuf[i].EventType != MOUSE_EVENT) continue;

		if (irInBuf[i].Event.MouseEvent.dwButtonState == 1) {
			return true;
		}
	}
	return false;
}

void Input::getMousePosition(int& x, int& y) {
	for (int i = 0; i < cNumRead; i++)
	{
		if (irInBuf[i].EventType != MOUSE_EVENT) continue;
		if (irInBuf[i].Event.MouseEvent.dwMousePosition.X >= 0 && irInBuf[i].Event.MouseEvent.dwMousePosition.X < 10 
			&& irInBuf[i].Event.MouseEvent.dwMousePosition.Y >= 0 && irInBuf[i].Event.MouseEvent.dwMousePosition.Y < 10) {
			x = irInBuf[i].Event.MouseEvent.dwMousePosition.X;
			y = irInBuf[i].Event.MouseEvent.dwMousePosition.Y;
		}
	}
}