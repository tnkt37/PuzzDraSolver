#include "PuzzDraBeamSearch.h"
#include "PuzzDraTests.h"

#include <string>


namespace tnkt37
{
	namespace PuzzDraSolver
	{
		Route beamSearch(BOARD& board, int aheads, int length, int beamWeight, int startRow, int startColumn, int& outScore)
		{
			std::vector<Route> preRoutes(beamWeight);
			std::priority_queue<std::pair<int, Route>> ordered;
			for (const auto& route : _routes[1])
				ordered.emplace(0, route);

			Route ansRoute;
			int evalMax = -1;

			length = std::min(length, MAX_ROUTE_SIZE);//大きさを制限

			auto at = _routes[aheads][0];
			int loopCount = (length - 1) / _routes[aheads][0].size() + !!((length - 1) % aheads);//lengthまでは必ず探索できるようににループの回数を決める
			for (int k = 0; k < loopCount; k++)
			{
				preRoutes.clear();
				for (int i = 0; i < beamWeight && !ordered.empty(); i++)
				{
					preRoutes.push_back(ordered.top().second);
					ordered.pop();
				}

				for (int i = 1; i <= aheads; i++)
				{
					const int size = _routes[i].size();
					for (const auto& route : preRoutes)
					{
						for (int j = 0; j < size; j++)
						{
							const auto& added = _routes[i][j];

							vector<vector<int>> tmpBoard = board;
							Route next = route;
							Route dummy;

							const int rs = route.size();
							const int as = added.size();
							if (rs + as > length) continue;//サイズが保存できる最大の大きさを超えるのを禁止

							////追加するルートの一回目の移動が意味のないものである場合はそのルートから始まるものを飛ばす
							////なんか精度落ちた。バグってる？
							//const int top = added.top();
							//if (top != -1 && (top + 2) % 4 == route.getBits(0) && size >= 4)
							//{
							//	j += size / 4 - 1;
							//	continue;
							//}

							next.push_back(added);
							if (simplifyRoute(next, startRow, startColumn, dummy) > 0)
								continue;

							int ms = next.size();
							moveByRoute(tmpBoard, next, startRow, startColumn);
							int evalNum = evalProcess(tmpBoard);

							if (evalNum > evalMax)
							{
								evalMax = evalNum;
								ansRoute = next;
							}
							ordered.emplace(evalNum, next);
						}
					}
				}
			}
			outScore = evalMax;
			return ansRoute;
		}

		void tnkt37::PuzzDraSolver::beamSearchTest()
		{
			int aheads, length, beamWeight;
			cin >> aheads >> length >> beamWeight;

			while (true)
			{
				auto board = generateRandomBoard();

				int evalMax = -1;
				Route ansRoute;
				int row = 0, column = 0;
				for (int i = 0; i < HEIGHT; i++)
				{
					for (int j = 0; j < WIDTH; j++)
					{
						auto tmpBoard = board;
						int evalNum;
						Route route = beamSearch(tmpBoard, aheads, length, beamWeight, i, j, evalNum);

						if (evalNum > evalMax)
						{
							evalMax = evalNum;
							ansRoute = route;
							row = i;
							column = j;
						}
					}
				}

				debug(board);
				cout << row << ", " << column << "  size: " << ansRoute.size() << endl;
				debug(ansRoute);

				moveByRoute(board, ansRoute, row, column);
				debug(board);
				evalProcess(board);
				debug(board);
				cout << evalMax << endl;

				string t;
				std::getline(cin, t);
				if (t == "change")
				{
					cin >> aheads >> length >> beamWeight;
				}
			}

		}
	}
}