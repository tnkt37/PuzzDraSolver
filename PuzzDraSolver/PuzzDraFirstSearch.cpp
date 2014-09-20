#include "PuzzDraFirstSearch.h"
#include "PuzzDraTests.h"


namespace tnkt37
{
	namespace PuzzDraSolver
	{

		//探索終了判定
		//調整が必要(1だとダメ)
		//そもそもない方がいい説
		inline bool isSearchCompleted(Route& route, int row, int column)
		{
			return false;
			Route simplified;
			int removed = simplifyRoute(route, row, column, simplified);
			return removed >= 4;
		}


		//特殊な貪欲法
		//lookAheadCountターン先まで決めたものを近傍とし、もっと良い道を含んでいたパターンを採用
		//1手づつ拡張していく
		Route firstSearch_4(BOARD& board, int lookAheadCount, int length, int startRow, int startColumn, int& outScore)
		{
			if (outOfRange(startRow, startColumn)) return Route();

			Route addedRoute;
			Route ansRoute;
			int evalAns = -1;
			//if (_routes[lookAheadCount].empty())
			//	calcRoutes_4(lookAheadCount);

			//int evalAllMax = -1;
			//int preSize = -1;
			for (int i = 0; i < length; i++)
			{
				Route nextRoute = addedRoute;
				int evalMax = -1;
				int aheads = lookAheadCount;
				if (i + lookAheadCount >= length)
				{
					aheads = length - i - 1;
				}
				const int size = _routes[aheads].size();
				for (int j = 0; j < size; j++)
				{
					Route& route = _routes[aheads][j];

					vector<vector<int>> tmpBoard = board;
					Route tmpRoute = addedRoute;
					tmpRoute.push_back(route);

					//追加するルートの一回目の移動が意味のないものである場合は飛ばす
					const int top = addedRoute.top();
					if (top != -1 && (top + 2) % 4 == route.getBits(0) && size >= 4)
					{
						j += size / 4 - 1;
						continue;
					}

					//単純化出来る道を探索しないようにしても意味なかった(精度が落ちた)
					//Route simRoute;
					//simplifyRoute(board, tmpRoute, startRow, startColumn, simRoute);
					//if (tmpRoute.size() != simRoute.size())
					//{
					//	continue;
					//}

					//debug(route);
					//debug(tmpRoute);

					moveByRoute(tmpBoard, tmpRoute, startRow, startColumn);

					//debug(tmpBoard);

					int evalNum = evalProcess(tmpBoard);

					//cout << evalNum << endl;

					if (evalNum > evalMax)
					{
						nextRoute = tmpRoute;
						evalMax = evalNum;
					}
				}

				//山登りにすると減った
				//if (evalAllMax > evalMax) break;
				//evalAllMax = evalMax;

				//やっていいのか微妙
				//if (nextRoute.size() == preSize)
				//{
				//	break;
				//}
				//preSize = nextRoute.size();

				//一番いいのを取る
				if (evalMax > evalAns)
				{
					ansRoute = nextRoute;
					evalAns = evalMax;
				}
				//else if(evalMax < evalAns) break;
				//山降りる

				int routeBits = nextRoute.getBits(nextRoute.size() - aheads);

				//lengthまでの探索が終わっている
				if (i + lookAheadCount >= length)
				{
					//addedRoute = nextRoute;
					break;
				}

				addedRoute.push_back(routeBits);

				//cout << i << endl;
				//debug(nextRoute);
				//debug(addedRoute);
			}
			outScore = evalAns;
			return ansRoute;
		}


		void outputAnswer_firstSearch()
		{
			int aheads; int length;
			cin >> aheads >> length;
			vector<vector<int> > board(HEIGHT, vector<int>(WIDTH, 0));


			inputBoard(board);
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
					Route route = firstSearch_4(board, aheads, length, i, j, evalNum);

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
					//	cout << "↓" << endl;
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

			debug(ansRoute);
			cout << row << ", " << column << endl;
			moveByRoute(board, ansRoute, row, column);
			debug(board);

			int evalNum = evalProcess(board);

			cout << evalNum << endl;
			debug(board);

		}

	}
}
