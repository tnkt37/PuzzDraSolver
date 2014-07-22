#include "PuzzDraRoute.h"


namespace tnkt37
{
	namespace PuzzDraSolver
	{

		vector<Route> _routes[LookAheadMax];
		void calcRoutes_4(int lookAheadCount)
		{
			//route, length
			std::queue<std::tuple<Route, int>> que;
			que.emplace(Route(), 0);

			for (int i = 0; i < LookAheadMax; i++)
			{
				_routes[i].clear();
			}

			//bfsでルートを全列挙
			while (!que.empty())
			{
				auto tup = que.front(); que.pop();
				int length = std::get<1>(tup);
				Route route = std::get<0>(tup);

				_routes[length].push_back(route);

				//debug(route);

				if (length == lookAheadCount)
				{
					continue;
				}

				int pre = route.top();
				for (int i = 0; i < 4; i++)
				{
					if ((i + 2) % 4 == pre) continue;//行って戻るのは禁止

					Route nextRoute = route;
					nextRoute.push_back(i);
					que.emplace(nextRoute, length + 1);

				}
			}
		}

	}
}