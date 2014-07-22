#pragma once

#include "PuzzDraOperator.h"
#include <algorithm>
#include <queue>
#include <tuple>

namespace tnkt37
{
	namespace PuzzDraSolver
	{


		//ルートを保存するための構造体
		//スタック形式で保存
		//最大42手まで保存可
		//8方向への移動が3bitに格納される(0~7)
		//空の状態では初め1にしておき、これが64bit目に到達したら埋まったと判定する(bit1 >> 63)
		//ルートの数はMSBでわかる
		//全部インライン展開
		//すでに宣言してある変数に代入しようとするとコンパイルエラーになるなんで

		const int MAX_ROUTE_SIZE = 42;

		class Route
		{
		public:
			unsigned long long bit1, bit2;
			Route()
			{
				bit1 = 1LL;
				bit2 = 1LL;
			}

			Route(const Route& route)
			{
				bit1 = route.bit1;
				bit2 = route.bit2;
			}

			//ルートを追加
			//0 <= nextRoute < 8
			void push_back(int nextRoute)
			{
				if (bit1 >> 63)
				{
					bit2 = (bit2 << 3) | (nextRoute & ((1 << 3) - 1));
				}
				else
				{
					bit1 = (bit1 << 3) | (nextRoute & ((1 << 3) - 1));
				}
			}

			//一気にルートを追加
			//lengthは追加する道の長さ
			void push_back(int nextRoute, int length)
			{
				int msb;
				MSB64bit(bit1, msb);
				if (length <= 0) return;
				if (msb == 63)
				{
					bit2 = (bit2 << 3 * length) | nextRoute;
				}
				else if (msb + length < 64)
				{
					bit1 = (bit1 << 3 * length) | nextRoute;
				}
				else//途中で途切れる
				{
					int firstLength = length - (63 - msb) / 3;
					bit1 = (bit1 << (3 * firstLength)) | (nextRoute & ((1 << ((firstLength * 3) - 1))));
					bit2 = (bit2 << (3 * (length - firstLength))) | (nextRoute >> (firstLength * 3));
				}
			}

			void push_back(const Route& route)
			{
				int argMsb1, argMsb2;
				MSB64bit(route.bit1, argMsb1);
				MSB64bit(route.bit2, argMsb2);

				push_back(route.bit1 & ~(1 << argMsb1), argMsb1 / 3);
				push_back(route.bit2 & ~(1 << argMsb2), argMsb2 / 3);
			}

			void push_back(const vector<int>& routes)
			{
				for (const auto& route : routes)
				{
					push_back(route);
				}
			}

			//一番最後の道を取得します
			int top() const
			{
				if (bit1 == 1LL) return -1;
				if (bit2 != 1LL) return bit2 & ((1 << 3) - 1);
				return bit1 & ((1 << 3) - 1);
			}

			//index番目の進んだ方向を取得します
			//index : はじめから数えた道数
			int getBits(int index) const
			{
				if (index >= MAX_ROUTE_SIZE) return -1;

				if (index < 21)//一つ目のbitに収まるなら
				{
					int msb;
					MSB64bit(bit1, msb);
					if (msb < index * 3) return -1;

					return (bit1 >> (long long)((msb / 3 - index - 1) * 3) & ((1 << 3) - 1));//bitを取得して返す
				}
				else
				{
					int msb;
					MSB64bit(bit2, msb);
					index -= 21;
					if (msb < index * 3) return -1;

					return (bit2 >> (long long)((msb / 3 - index - 1) * 3) & ((1 << 3) - 1));//bitを取得して返す
				}
			}

			//一番最初の道を削除します
			void shift()
			{
				if (bit1 == 1LL) return;

				if (bit2 == 1LL)
				{
					bit1 = bit1 >> 3;
				}
				else
				{
					bit1 = (bit1 >> 3) | (bit2 & ((1 << 3) - 1));
					bit2 = (bit2 >> 3);
				}
			}

			//一番最後の道を削除します
			void pop_back()
			{
				if (bit1 == 1LL) return;

				if (bit2 != 1LL)
				{
					bit2 = bit2 >> 3;
				}
				else
				{
					bit1 = bit1 >> 3;
				}
				bit1 = std::max(1uLL, bit1);
				bit2 = std::max(1uLL, bit2);

			}

			//一番最後の道からlength個の道を削除します
			void pop_back(int length)
			{
				if (bit2 == 1uLL)//全部bit1
				{
					bit1 = bit1 >> length * 3;
				}

				int msb;
				MSB64bit(bit2, msb);

				int secondLength = length - msb / 3;
				if (secondLength <= 0)//全部bit2の分
				{
					bit2 = (bit2 >> length * 3);
				}
				else//途中で別れる
				{
					bit2 = 1LL;
					bit1 = (bit1 >> secondLength * 3);
				}

				bit1 = std::max(1uLL, bit1);//0にならないように
				bit2 = std::max(1uLL, bit2);//0にならないように
			}

			int size() const
			{
				int bit1Size, bit2Size;
				MSB64bit(bit1, bit1Size);
				MSB64bit(bit2, bit2Size);
				return (bit1Size + bit2Size) / 3;
			}

			void clear()
			{
				bit1 = 1uLL;
				bit2 = 1uLL;
			}

			Route& operator=(const Route& route)
			{
				bit1 = route.bit1;
				bit2 = route.bit2;
				return *this;
			}

			bool operator<(const Route& route) const
			{
				return size() < route.size();
			}

		private:
			//http://marupeke296.com/TIPS_No17_Bit.html
			int count64bit(long long v) const {
				long long count = (v & 0x5555555555555555) + ((v >> 1) & 0x5555555555555555);
				count = (count & 0x3333333333333333) + ((count >> 2) & 0x3333333333333333);
				count = (count & 0x0f0f0f0f0f0f0f0f) + ((count >> 4) & 0x0f0f0f0f0f0f0f0f);
				count = (count & 0x00ff00ff00ff00ff) + ((count >> 8) & 0x00ff00ff00ff00ff);
				count = (count & 0x0000ffff0000ffff) + ((count >> 16) & 0x0000ffff0000ffff);
				return (int)((count & 0x00000000ffffffff) + ((count >> 32) & 0x00000000ffffffff));
			}
			//0 <= return < 64
			void MSB64bit(long long v, int &out) const {
				if (v == 0) return;
				v |= (v >> 1);
				v |= (v >> 2);
				v |= (v >> 4);
				v |= (v >> 8);
				v |= (v >> 16);
				v |= (v >> 32);
				out = count64bit(v) - 1;
			}
		};


		const int LookAheadMax = 11;
		extern vector<Route> _routes[LookAheadMax];
		extern void calcRoutes_4(int lookAheadCount);


		inline int moveByRoute(BOARD& board, Route route, int row, int column)
		{
			int movement = route.top();
			route.pop_back();
			if (movement == -1) return -1;
			int next = moveByRoute(board, route, row, column);
			if (next != -1)
			{
				row = next & ((1 << 3) - 1);
				column = (next >> 3) & ((1 << 3) - 1);
			}

			next = slideDrop_4(board, row, column, movement);
			return next;
		}


		//行って帰ってくるパターンを消す
		inline void _simplifyRouteSideStep(Route route, Route& ansRoute)
		{
			int movement = route.top();
			route.pop_back();

			if (movement == -1) return;

			int preMovement = route.top();

			if ((movement + 2) % 4 == route.top())//次の移動の90度反対(意味のない移動)
			{
				route.pop_back();//次の移動も飛ばす

				movement = route.top();
				route.pop_back();
				if (movement == -1) return;
			}
			_simplifyRouteSideStep(route, ansRoute);
			ansRoute.push_back(movement);
			return;
		}

		//壁にぶつかる移動を消す
		inline int _simplifyRouteNonMoving(Route route, int row, int column, Route& ansRoute)
		{
			int movement = route.top();
			route.pop_back();

			if (movement == -1) return -1;

			int preMovement = route.top();

			int next = _simplifyRouteNonMoving(route, row, column, ansRoute);
			if (next != -1)
			{
				row = next & ((1 << 3) - 1);
				column = (next >> 3) & ((1 << 3) - 1);
			}

			int nextRow = row + _dr_4[movement];
			int nextColumn = column + _dc_4[movement];
			if (outOfRange(nextRow, nextColumn))
				return (column << 3) | row;

			ansRoute.push_back(movement);
			return (nextColumn << 3) | nextRow;

		}


		//意味のない移動を削除します。1度では全ての意味のない移動が消せない可能性もあります。
		//
		inline int simplifyRoute(Route route, int row, int column, Route& outRoute)
		{
			Route tmpRoute, ansRoute;
			_simplifyRouteNonMoving(route, row, column, tmpRoute);
			_simplifyRouteSideStep(tmpRoute, ansRoute);
			int removedCount = route.size() - ansRoute.size();
			outRoute = ansRoute;

			return removedCount;
		}


		//意味のない移動を完全に削除
		inline int simplifyRoutePerfectly(Route& route, int row, int column, Route& outRoute)
		{
			Route tmpRoute;
			Route nextRoute;
			int removed = simplifyRoute(route, row, column, nextRoute);
			while (true)
			{
				tmpRoute = nextRoute;
				nextRoute.clear();
				_simplifyRouteSideStep(tmpRoute, nextRoute);
				int rem = tmpRoute.size() - nextRoute.size();
				if (rem == 0) break;
				removed += rem;
			}
			outRoute = tmpRoute;
			return removed;
		}

	}
}