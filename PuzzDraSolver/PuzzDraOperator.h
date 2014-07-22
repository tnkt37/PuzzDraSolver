#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <iostream>


namespace tnkt37
{
	namespace PuzzDraSolver
	{

		using std::vector;
		using std::string;
		using std::cin;
		using std::cout;
		using std::endl;

		const int WIDTH = 6;
		const int HEIGHT = 5;
		const int DROP_NUM = WIDTH * HEIGHT;
		const int ATTRIBUTE_NUM = 6;
		const int LEAST_COMBO_NUM = 3;
		const int EMPTY = 0;

		typedef vector<vector<int>> BOARD;

		//コンボ判定用に使う
		//http://www.prefield.com/algorithm/container/union_find.html
		struct UnionFind {
			std::vector<int> data;
			UnionFind(int size) : data(size, -1) { }
			bool unionSet(int x, int y) {
				x = root(x); y = root(y);
				if (x != y) {
					if (data[y] < data[x]) std::swap(x, y);
					data[x] += data[y]; data[y] = x;
				}
				return x != y;
			}
			bool findSet(int x, int y) {
				return root(x) == root(y);
			}
			int root(int x) {
				return data[x] < 0 ? x : data[x] = root(data[x]);
			}
			int size(int x) {
				return -data[root(x)];
			}
		};


		//範囲チェックのヘルパ関数
		//0 <= row < HEIGHT && 0 <= column < WIDTH
		inline bool outOfRange(int row, int column)
		{
			return row < 0 || row >= HEIGHT || column < 0 || column >= WIDTH;
		}


		inline void swapDrop(BOARD& board, int row1, int column1, int row2, int column2)
		{
			std::swap(board[row1][column1], board[row2][column2]);
		}


		const int _dr_4[] = { -1, 0, 1, 0 };
		const int _dc_4[] = { 0, 1, 0, -1 };

		//4近傍にドロップを移動
		//direction 0を上にして1ずつ90度時計回り
		//移動後のドロップの一を返す(0~2bit目でrow, 3~5bit目でcolumn)
		inline int slideDrop_4(BOARD& board, int row, int column, int direction)
		{
			if (direction < 0 || direction >= 4) return (column << 3) | row;

			int nextRow = row + _dr_4[direction];
			int nextColumn = column + _dc_4[direction];
			if (outOfRange(nextRow, nextColumn)) return (column << 3) | row;

			swapDrop(board, row, column, nextRow, nextColumn);
			return (nextColumn << 3) | nextRow;
		}


		const int _dr_8[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
		const int _dc_8[] = { 0, 1, 1, 1, 0, -1, -1, -1 };

		//8近傍にドロップを移動
		//direction 0を上にして1ずつ45度時計回り
		inline void slideDrop_8(BOARD& board, int row, int column, int direction)
		{
			if (direction < 0 || direction >= 8) return;

			int nextRow = row + _dr_8[direction];
			int nextColumn = column + _dc_8[direction];
			if (outOfRange(nextRow, nextColumn)) return;

			swapDrop(board, row, column, nextRow, nextColumn);
		}



		inline bool pushDownDrops(BOARD& board)
		{
			bool changed = false;
			for (int column = 0; column < WIDTH; column++)//それぞれの行について
			{
				int empties = 0;//空のドロップの数
				for (int row = HEIGHT - 1; row >= 0; row--)//下から
				{
					if (board[row][column] != EMPTY)
					{
						board[row + empties][column] = board[row][column];//下へ

						if (empties != 0)
						{
							board[row][column] = 0;
							changed = true;
						}
					}
					else
					{
						empties++;
					}
				}
			}

			return changed;
		}



		//となりにコンボがあればそれにつなげる
		inline void checkConnect(BOARD& board, UnionFind& comboTree, int pos, int adjacentPos)
		{
			if (adjacentPos < 0 || adjacentPos >= DROP_NUM) return;

			int root = comboTree.root(adjacentPos);

			//コンボに属している
			if (root != adjacentPos)
			{
				//分解
				int row1 = pos / WIDTH;
				int column1 = pos % WIDTH;
				int row2 = root / WIDTH;
				int column2 = root % WIDTH;

				//属性が同じならつなげる
				if (board[row1][column1] == board[row2][column2])
				{
					comboTree.unionSet(adjacentPos, pos);
				}
			}
		}


		//縦のコンボ
		inline void comboRow(BOARD& board, UnionFind& comboTree, int row, int column)
		{
			//コンボにならない
			if (row + 2 >= HEIGHT ||
				board[row][column] != board[row + 1][column] || board[row][column] != board[row + 2][column])
				return;


			int pos = row * WIDTH + column;//平坦化

			checkConnect(board, comboTree, pos, pos - WIDTH);//一段上
			checkConnect(board, comboTree, pos, pos - 1);//左隣
			checkConnect(board, comboTree, pos, pos + 1);//右隣

			comboTree.unionSet(pos, pos + WIDTH);//一個下
			comboTree.unionSet(pos, pos + WIDTH + WIDTH);//二個下
		}


		//横のコンボ
		inline void comboColumn(BOARD& board, UnionFind& comboTree, int row, int column)
		{
			//コンボにならない
			if (column + 2 >= WIDTH ||
				board[row][column] != board[row][column + 1] || board[row][column] != board[row][column + 2])
				return;


			int pos = row * WIDTH + column;//平坦化

			checkConnect(board, comboTree, pos, pos - WIDTH);//一段上
			checkConnect(board, comboTree, pos, pos - 1);//左隣
			checkConnect(board, comboTree, pos, pos + 1);//右隣

			comboTree.unionSet(pos, pos + 1);//一個右
			comboTree.unionSet(pos, pos + 2);//二個右
		}


		//ボードを走査してコンボ関係を保持するUnionFindを構築します
		inline void buildComboTree(BOARD& board, UnionFind& comboTree)
		{
			for (int row = 0; row < HEIGHT; row++)
			{
				for (int column = 0; column < WIDTH; column++)
				{
					if (board[row][column] == EMPTY) continue;

					//縦のコンボ
					comboRow(board, comboTree, row, column);
					//横のコンボ
					comboColumn(board, comboTree, row, column);
				}
			}
		}


		//UnionFindの情報からコンボを削除し、ついでにコンボ数を集計します。
		inline void removeCombo(BOARD& board, UnionFind& comboTree, int& out_comboCount)
		{
			int pos = 0;
			int combo = 0;
			for (int row = 0; row < HEIGHT; row++)
			{
				for (int column = 0; column < WIDTH; column++)
				{
					//コンボに属している
					if (comboTree.root(pos) != pos)
					{
						board[row][column] = 0;
					}
					//コンボの基準点
					else if (comboTree.size(pos) >= 3)
					{
						board[row][column] = 0;
						combo++;
					}

					pos++;
				}
			}
			out_comboCount = combo;
		}


		//カスタムできるevalCombo
		//evaluationFunction内でBOARDやUnionFindを変更してはいけない
		inline int evalComboCustomized(BOARD& board, std::function<int(BOARD&, UnionFind&)> evaluationFunction)
		{
			UnionFind comboTree(DROP_NUM);
			buildComboTree(board, comboTree);

			int evaluatedValue = evaluationFunction(board, comboTree);

			int comboCount;
			removeCombo(board, comboTree, comboCount);
			return evaluatedValue;
		}


		inline int evalCombo(BOARD& board)
		{
			//左上から走査していき、
			//今見ている点を一番左上とするコンボを組み立てる。
			//上か左に隣接しているコンボが同じ属性であればUnion-Find木を使って結合する

			UnionFind comboTree(DROP_NUM);
			buildComboTree(board, comboTree);

			int comboCount;
			removeCombo(board, comboTree, comboCount);

			return comboCount;
		}


		//いわゆる 「待ち」
		//特定の色が来たらコンボが増えるパターンを数える
		//リーチの数
		inline int countReach(BOARD& board)
		{
			int reachCount = 0;
			for (int column = 0; column < WIDTH; column++)
			{
				for (int row = 0; row < HEIGHT - 1; row++)//最後の段の一個上まで
				{
					if (board[row][column] != EMPTY) break;//上から見て空じゃなくなったら個の列は終わり
					if (board[row + 1][column] == EMPTY) continue;//一個下も空きだったら処理しない

					/*リーチのパターンは以下の4つ
					横のコンボの一番右になる
					横のコンボの真ん中になる
					横のコンボの一番左になる
					縦のコンボの一番上になる
					コード重複ヤバイ
					*/
					vector<bool> attributes(ATTRIBUTE_NUM + 1);//その属性でリーチになっているかどうかを保持する
					int counter = 0;
					if (column - 2 >= 0 && board[row][column - 2] != EMPTY &&
						board[row][column - 2] == board[row][column - 1] &&
						!attributes[board[row][column - 2]])//すでにその属性でリーチになっていない
					{
						counter++;
						attributes[board[row][column - 2]] = true;
					}

					if (column - 1 >= 0 && column + 1 < WIDTH && board[row][column - 1] != EMPTY &&
						board[row][column - 1] == board[row][column + 1] &&
						!attributes[board[row][column - 1]])//すでにその属性でリーチになっていない
					{
						counter++;
						attributes[board[row][column - 1]] = true;
					}

					if (column + 2 < WIDTH && board[row][column + 2] != EMPTY &&
						board[row][column + 1] == board[row][column + 2] &&
						!attributes[board[row][column + 2]])//すでにその属性でリーチになっていない
					{
						counter++;
						attributes[board[row][column + 2]] = true;
					}

					if (row + 2 < HEIGHT &&
						board[row + 1][column] == board[row + 2][column] &&
						!attributes[board[row + 1][column]])//すでにその属性でリーチになっていない
					{
						counter++;
						attributes[board[row + 1][column]] = true;

					}

					reachCount += counter;
				}
			}

			return reachCount;
		}


		//評価や盤面の押し下げ等の一連の処理を行う
		//返り値は評価値
		inline int evalProcess(BOARD& board)
		{
			int evalNum = 0;
			while (true)
			{
				int combo = evalCombo(board);
				if (combo == 0) break;
				evalNum += combo * 100;//リーチより価値を高くするための100倍

				bool pushed = pushDownDrops(board);
				if (!pushed) break;
			}
			evalNum += countReach(board);

			return evalNum;
		}



		inline int evalProcessCustomized(BOARD& board, std::function<int(BOARD&, UnionFind&)> comboFunction, std::function<int(BOARD&)> reachFunction)
		{
			int evalNum = 0;
			while (true)
			{
				int combo = evalComboCustomized(board, comboFunction);

				bool pushed = pushDownDrops(board);
				if (!pushed) break;
			}
			evalNum += reachFunction(board);

			return evalNum;
		}
	}
}