#include "PuzzDraTests.h"

#include "PuzzDraFirstSearch.h"

#include <iostream>
#include <cstdio>
#include <random>
#include <sstream>


namespace tnkt37
{
	namespace PuzzDraSolver
	{

		void debug(BOARD& board){ cout << toStringBoard(board) << endl; }
		void debug(Route route){ cout << toStringRoute(route) << endl; }


		void inputBoard(BOARD& board)
		{
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					cin >> board[i][j];
				}
			}
		}


		string toStringBoard(BOARD& board)
		{
			std::ostringstream os;
			os << '\n';
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					os << board[i][j] << ' ';
				}
				os << '\n';
			}
			return os.str();
		}


		string toStringRoute(Route route)
		{
			std::ostringstream os;
			int movement = route.top();
			route.pop_back();
			if (movement == -1) return "";
			auto result = toStringRoute(route);
			os << result << (result == "" ? "" : ", ");
			string name[] = { "up", "right", "down", "left" };
			os << name[movement];
			//os << movement;
			return os.str();
		}



		void simplifyRouteTest()
		{
			int row, column;
			cin >> row >> column;
			vector<vector<int> > board(HEIGHT, vector<int>(WIDTH, 0));
			inputBoard(board);

			const int up = 0;
			const int right = 1;
			const int down = 2;
			const int left = 3;

			Route route;

			//vector<int> routes = { up, up, up, up, up, down, up, up, up, up, up, up, up
			//	, up, up, up, up, up, up, up, up, right };

			vector<int> routes = { down, down, down, down, down, up, left, down, right, down, down, up, up, up, up,
				up, up, up, up, up, up, up };

			route.push_back(routes);
			debug(route);
			vector<vector<int>> board2 = board;
			moveByRoute(board, route, row, column);
			debug(board);
			cout << endl;
			Route simplifid;
			simplifyRoutePerfectly(route, row, column, simplifid);
			//_simplifyRouteNonMoving(board, route, row, column, simplifid);
			cout << endl;
			debug(simplifid);
			moveByRoute(board2, simplifid, row, column);
			debug(board2);
		}


		vector<vector<int>> generateRandomBoard()
		{
			vector<vector<int>> board(HEIGHT, vector<int>(WIDTH));
			fillBoardRandom(board);
			return std::move(board);
		}

		std::mt19937 engine(0);
		std::uniform_int_distribution<> attr(1, 6);
		int fillBoardRandom(BOARD& board)
		{
			int removed = 0;
			while (true)//‹ó‚«‚ª‚È‚¢
			{
				bool hasEmpty = false;
				for (auto& line : board)
				{
					for (auto& drop : line)
					{
						if (drop != EMPTY) continue;

						hasEmpty = true;
						drop = attr(engine);
					}
				}
				if (!hasEmpty) break;
				removed += evalCombo(board);
				pushDownDrops(board);
			}
			return removed;
		}


		void firstSearchTest()
		{
			int aheads; int length;
			cin >> aheads >> length;
			vector<vector<int> > originalBoard(HEIGHT, vector<int>(WIDTH, 0));

			while (true)
			{
				originalBoard = generateRandomBoard();
				//inputBoard(originalBoard);
				debug(originalBoard);

				for (int l = 0; l <= aheads; l++)
				{
					vector<vector<int>> board = originalBoard;
					Route ansRoute;
					int evalMAX = -1;
					int size = 1 << 20;
					int row = 0, column = 0;

					for (int i = 0; i < HEIGHT; i++)
					{
						for (int j = 0; j < WIDTH; j++)
						{
							vector<vector<int>> tmpBoard = board;
							int evalNum;

							Route route = firstSearch_4(board, l, length, i, j, evalNum);

							//if ((i == 2 && j == 4) || (i == 2 && j == 1))
							//{
							//	debug(tmpBoard);
							//}
							Route simpliFiedRoute;
							simplifyRoutePerfectly(route, i, j, simpliFiedRoute);

							//if ((i == 2 && j == 4) || (i == 2 && j == 1))
							//{
							//	cout << endl;
							//	cout << "debug:" << endl;
							//	debug(board);
							//	debug(tmpBoard);
							//	cout << i << ", " << j << endl;
							//	debug(route);
							//	cout << "«" << endl;
							//	debug(simpliFiedRoute);
							//	cout << evalNum << endl;
							//	cout << endl;
							//}

							//Route simpliFiedRoute = route;
							if (evalMAX < evalNum)
							{
								ansRoute = simpliFiedRoute;
								evalMAX = evalNum;
								row = i;
								column = j;
								size = ansRoute.size();
							}
							else if (evalMAX == evalNum && size < simpliFiedRoute.size())
							{
								ansRoute = simpliFiedRoute;
								evalMAX = evalNum;
								row = i;
								column = j;
								size = ansRoute.size();
							}
						}
					}

					//debug(ansRoute);
					//cout << row << ", " << column << endl;
					moveByRoute(board, ansRoute, row, column);
					//debug(board);

					int evalNum = evalProcess(board);
					cout << "Aheads:       " << l << endl;
					cout << "Evaluation:   " << evalNum << endl;
					evalNum += fillBoardRandom(board) * 100;//–ˆ‰ñƒ{[ƒhŒ‹‰Ê‚ª•Ï‚í‚é‚Ì‚Å•½“™‚Å‚È‚¢
					cout << "RandomFilled: " << evalNum / 100 * 100 << endl;

				}
				string tmp;
				std::getline(cin, tmp);
			}

		}

	}
}