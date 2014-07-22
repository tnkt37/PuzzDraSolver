#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <sstream>
#include <utility>
#include <queue>
#include <tuple>
#include <functional>
#include <random>

#include "PuzzDraOperator.h"
#include "PuzzDraRoute.h"
#include "PuzzDraTests.h"
#include "PuzzDraBeamSearch.h"
#include "PuzzDraFirstSearch.h"

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;

using namespace tnkt37::PuzzDraSolver;

extern "C"
{
	BOARD toBoard(int* boardArr)
	{
		vector<vector<int>> board(HEIGHT, vector<int>(WIDTH));
		for (int i = 0; i < DROP_NUM; i++)
		{
			int r = i / WIDTH, c = i % WIDTH;
			board[r][c] = boardArr[i];
		}
		return std::move(board);
	}

	Route toRoute(int* routeArr)
	{
		Route route;
		for (int i = 0; ; i++)
		{
			if (routeArr[i] == -1)
				break;
			route.push_back(routeArr[i]);
		}
		return route;
	}

	void toRouteArr(const Route& route, int* routeArr)
	{
		for (int i = 0; i < route.size(); i++)
		{
			routeArr[i] = route.getBits(i);
		}

		routeArr[route.size()] = -1;
	}

	void toBoardArr(BOARD& board, int* boardArr)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				boardArr[i * WIDTH + j] = board[i][j];
			}
		}
	}

	__declspec(dllexport) void InitializeSearchData(int maxLookAhead)
	{
		if (maxLookAhead >= LookAheadMax) return;
		calcRoutes_4(maxLookAhead);
	}

	__declspec(dllexport) int BeamSearch(int* boardArr, int aheads, int length, int beamWeight, int startRow, int startColumn, int* outRoute)
	{
		BOARD board = toBoard(boardArr);

		int score;
		auto route = beamSearch(board, aheads, length, beamWeight, startRow, startColumn, score);

		toRouteArr(route, outRoute);

		return score;
	}

	__declspec(dllexport) int FirstSearch(int* boardArr, int aheads, int length, int startRow, int startColumn, int* outRoute)
	{
		BOARD board = toBoard(boardArr);

		int score;
		auto route = firstSearch_4(board, aheads, length, startRow, startColumn, score);

		toRouteArr(route, outRoute);

		return score;
	}

	__declspec(dllexport) int EvalCombo(int* boardArr)
	{
		BOARD board = toBoard(boardArr);
		int comboCount = evalCombo(board);
		toBoardArr(board, boardArr);

		return comboCount;
	}

	__declspec(dllexport) int CountReach(int* boardArr)
	{
		BOARD board = toBoard(boardArr);
		int reachCount = countReach(board);
		toBoardArr(board, boardArr);

		return reachCount;
	}

	__declspec(dllexport) int EvalProcess(int* boardArr)
	{
		BOARD board = toBoard(boardArr);
		int score = evalProcess(board);
		toBoardArr(board, boardArr);

		return score;
	}

	__declspec(dllexport) int PushDownDrops(int* boardArr)
	{
		BOARD board = toBoard(boardArr);
		bool pushed = pushDownDrops(board);
		toBoardArr(board, boardArr);

		//DllImport‚ÌŠÖŒW‚Åint‚Å•Ô‚·
		return pushed;
	}

	__declspec(dllexport) int FillBoardRandom(int* boardArr)
	{
		BOARD board = toBoard(boardArr);
		int score = fillBoardRandom(board);
		toBoardArr(board, boardArr);
		return score;
	}

	__declspec(dllexport) void MoveByRoute(int* boardArr, int* routeArr, int startRow, int startColumn)
	{
		BOARD board = toBoard(boardArr);
		Route route = toRoute(routeArr);
		moveByRoute(board, route, startRow, startColumn);
		toBoardArr(board, boardArr);
	}
}
//
//int main(void)
//{
//	calcRoutes_4(10);
//	while (true)
//	{
//		//outputAnswer();
//		//simplifyRouteTest();
//		//firstSearchTest();
//		beamSearchTest();
//	}
//	return EXIT_SUCCESS;
//}
