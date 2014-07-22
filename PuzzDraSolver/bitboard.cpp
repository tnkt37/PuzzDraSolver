//#include <iostream>
//#include <vector>
//#include <string>
//#include <sstream>
//#include <unordered_set>
//#include <unordered_map>
//#include <array>
//#include <algorithm>
//
//using namespace std;
//
///*入力
//(0, 0) (0, 1) (0, 2) (0, 3) (0, 4) (0, 5)
//(1, 0) (1, 1) (1, 2) (1, 3) (1, 4) (1, 5)
//(2, 0) (2, 1) (2, 2) (2, 3) (2, 4) (2, 5)
//(3, 0) (3, 1) (3, 2) (3, 3) (3, 4) (3, 5)
//(4, 0) (4, 1) (4, 2) (4, 3) (4, 4) (4, 5)
//
//それぞれ属性ごとに 1, 2, 3, 4, 5, 6 の値を取る
//*/
//
//const int H = 5;
//const int W = 6;
//const int SIZE = H * W;
//const int ATTRIBUTE_NUM = 6;
//const int TATE_LINE = 545392672;
//const int YOKO_LINE = (1 << W) - 1;
//const int FILLED_BORD = (1 << SIZE) - 1;
//const int COMBO_NUM_LEAST = 3;
//const pair<int, int> NO_COMBO = pair<int, int>();
//
////pushDown高速化のために使う
////find!=endなら処理する必要がない
////unordered_set<int> pushable;//いらなそう
//
//
////縦のbitから横のbitに変換するためのテーブル
////速くなるか微妙だから余裕があれば使う版と使わない版で比較したほうがいいかも
////unordered_map<int, int> tateToYokoTable;
////void _initializeTateToYokoTable();
//
//
////combo判定用に使う
////firstからsecondまでコンボ成立
//array<pair<int, int>, 1 << W> comboPatterns;
//
//array<int, 1 << W> comboToDelete;
//
//array<int, 1 << W> yokoToTateTable;
//
//void _initializeYokoToTateTable()
//{
//	//全コンボパターンをbit全列挙
//	for (int i = 0; i <= YOKO_LINE; i++)
//	{
//		int tate = 0;
//		for (int k = 0; k < H; k++)
//		{
//			tate = (tate << W) | (((i >> (W - k - 1)) & 1) << W - 1);
//		}
//		yokoToTateTable[i] = tate;
//	}
//}
//
//void _initializeComboPattens()
//{
//	//全コンボパターンをbit全列挙
//	for (int i = 0; i <= YOKO_LINE; i++)
//	{
//		int renzoku = 0;
//		int comboCount = 0;
//		int from = 0;//あとで名前変える
//		int goal = 0;//あとで名前変える
//		for (int k = 0; k < W; k++)
//		{
//			if ((i >> (W - k - 1)) & 1)
//			{
//				renzoku++;
//				goal = k;
//			}
//			else
//			{
//				if (renzoku >= COMBO_NUM_LEAST)
//				{
//					break;
//				}
//				from = k + 1;
//				renzoku = 0;
//			}
//			if (renzoku >= COMBO_NUM_LEAST)
//			{
//				comboCount = renzoku;
//			}
//		}
//
//		if (comboCount)
//		{
//			auto p = pair<int, int>(from, goal);
//			comboPatterns[i] = p;
//
//			int bit = (1 << W) - 1;
//			for (int k = 0; k < W; k++)
//			{
//				if (p.first <= k && k <= p.second)
//				{
//					bit = bit & ~(1 << (W - k - 1));
//				}
//			}
//			comboToDelete[i] = bit;
//		}
//	}
//}
//
//
//void initializeData()
//{
//	_initializeComboPattens();
//	_initializeYokoToTateTable();
//}
//
////縦のbitを横のbitに変換する
//inline int tateToYoko(int tate)
//{
//	int shifted = SIZE - 1;
//	int yoko = 0;
//	for (int i = 0; i < H; i++)
//	{
//		yoko = (yoko << 1) | ((tate >> shifted) & 1);
//		shifted -= W;
//	}
//	return yoko << 1;
//}
//
////入力をビットボードに変換
////(0, 0)が最大bitになる
//int toBitBoard(vector<vector<int> >& board, int attribute)
//{
//	int bits = 0;
//	for (auto line : board)
//	{
//		for (auto ball : line)
//		{
//			bits = (bits << 1) | (ball == attribute);
//		}
//	}
//	return bits;
//}
//
//
////ビットボードを1bitずつ出力
//string toStringBitBoard(int bitBoard)
//{
//	int num = SIZE;
//	ostringstream ss;
//	for (int i = 0; i < H; i++)
//	{
//		for (int j = 0; j < W; j++)
//		{
//			ss << ((bitBoard >> (num - 1)) & 1) << ' ';
//			num--;
//		}
//		ss << '\n';
//	}
//	return ss.str();
//}
//
//
//string toStringBitBoards(vector<int>& bitBoards)
//{
//	ostringstream ss;
//	for (int i = 0; i < H; i++)
//	{
//		for (int j = 0; j < W; j++)
//		{
//			bool output = false;
//			for (int k = 0; k < ATTRIBUTE_NUM; k++)
//			{
//				if ((bitBoards[k] >> W * (H - i - 1) + (W - j - 1)) & 1)
//				{
//					ss << (k + 1) << ' ';
//					output = true;
//					break;
//				}
//			}
//			if (!output) ss << '0' << ' ';
//		}
//		ss << '\n';
//	}
//	return ss.str();
//}
//
//
//string debug(int bitBoard)
//{
//	return toStringBitBoard(bitBoard);
//}
//
//
////全てのビットボードの論理和を取る
//inline int toAllBoard(vector<int>& bitBoards)
//{
//	int allBoard = 0;
//	for (auto board : bitBoards)
//	{
//		allBoard |= board;
//	}
//
//	return allBoard & FILLED_BORD;//30,31bit目が立っているい状態を防ぐ
//}
//
//
////押し下げる
////bitboard : 押し下げられる前のボード
////r : 下からi番目
////c : 左からj番目
//inline int pushDownBitBoard(int bitBoard, int r, int c)
//{
//	int deletedBoard = ~(TATE_LINE >> c) | (TATE_LINE >> c + W * (H - r));//編集するところを削除するbit
//	int nowTate = (bitBoard & (TATE_LINE >> c)) & ~deletedBoard;//今見ている列のbit。押し下げられる
//	int pushedTate = nowTate >> W;//一段下へ
//	return (bitBoard & deletedBoard) | pushedTate;
//}
//
//
////コンボで空いた隙間を埋める : 空きがあったかどうか
//bool pushDownBoards(vector<int>& bitBoards)
//{
//	int allBoard = toAllBoard(bitBoards);
//	if (allBoard == FILLED_BORD)
//	{
//		return false; //隙間が空いていなければ戻る
//	}
//
//	int pushDownedBoard = allBoard;
//	//詰める
//	for (int c = 0; c < W; c++)
//	{
//		int tateAll = (pushDownedBoard << c) & TATE_LINE;//一番左の列に揃えてそれ以外を消す
//		if (tateAll == TATE_LINE || tateAll == 0) continue;//空きがなければ詰めない
//
//		int pushed = 0;
//		for (int r = 0; r < H - 1 - pushed;)//下から見ていく
//		{
//			if (pushDownedBoard >> (r * W + (W - c - 1)) & 1)//空いているかどうか
//			{
//				r++;//上へ
//				continue;
//			}
//			pushDownedBoard = pushDownBitBoard(pushDownedBoard, r, c);//全体を押し下げる
//			for (auto& board : bitBoards)
//			{
//				board = pushDownBitBoard(board, r, c);//一つの属性のビットボードを押し下げる
//			}
//			//途中
//			pushed++;
//		}
//	}
//
//	return allBoard != (pushDownedBoard & FILLED_BORD);//変更されたかどうか
//}
//
//
////attribute1の(row1, column1)とattribute2の(row2, column2)を入れ替える
////必ずそこにボールがなければならない
////rowとcolumnは左上から数えたもの
////0 <= attribute < ATTRIBUTE_NUM
//void swapBall(vector<int>& bitBoards, int attribute1, int row1, int column1, int attribute2, int row2, int column2)
//{
//	if (attribute1 == attribute2)
//	{
//		return;//属性が同じなら入れ替える必要がない
//	}
//
//	int board1 = bitBoards[attribute1];
//	int board2 = bitBoards[attribute2];
//
//	board1 = board1 & (FILLED_BORD & ~(1 << W * (H - row1 - 1) + (W - column1 - 1)));//入れ替え前を0にする
//	board1 = board1 | (1 << W * (H - row2 - 1) + (W - column2 - 1));//入れ替え後を1にする
//	board1 &= FILLED_BORD;
//	board2 = board2 & (FILLED_BORD & ~(1 << W * (H - row2 - 1) + (W - column2 - 1)));//入れ替え前を0にする
//	board2 = board2 | (1 << W * (H - row1 - 1) + (W - column1 - 1));//入れ替え後を1にする
//	board2 &= FILLED_BORD;
//	bitBoards[attribute1] = board1;
//	bitBoards[attribute2] = board2;
//}
//
//
////(row, column)にあるボールの属性を特定する
//int getAttribute(vector<int>& bitBoards, int row, int column)
//{
//	for (int i = 0; i < ATTRIBUTE_NUM; i++)
//	{
//		if ((bitBoards[i] >> W * (H - row - 1) + (W - column - 1)) & 1)
//			return i;
//	}
//	return -1;
//}
//
//
////(row1, column1)と(row2, column2)を入れ替える
////必ずそこにボールがなければならない
////rowとcolumnは左上から数えたもの
////入れ替えられる(入れ替える意味がある)かどうかを返す
//inline bool swapBall(vector<int>& bitBoards, int row1, int column1, int row2, int column2)
//{
//	int attribute1 = getAttribute(bitBoards, row1, column1);
//	int attribute2 = getAttribute(bitBoards, row2, column2);
//	if (attribute1 == attribute2) return false;//属性が同じなら入れ替える意味が無い
//
//	swapBall(bitBoards, attribute1, row1, column2, attribute2, row2, column2);
//	return true;
//}
//
//
////横方向のコンボを見る
////何コンボしたかを返す
//int yokoCombo(int bitBoard, int& deletedBoard)
//{
//	//1列づつ
//	deletedBoard = bitBoard;
//	int comboCount = 0;
//	for (int i = 0, shifted = 0; i < H; i++, shifted += W)
//	{
//		int yoko = (bitBoard >> shifted) & YOKO_LINE;
//
//		auto pattern = comboPatterns[yoko];
//		if (pattern == NO_COMBO) continue;
//
//		comboCount++;
//
//		yoko = yoko & comboToDelete[yoko];
//		deletedBoard = (~(YOKO_LINE << shifted) & deletedBoard) | (yoko << shifted);
//	}
//	return comboCount;
//}
//
//
////縦方向のコンボ
////なんコンボしたかを返す
//int tateCombo(int bitBoard, int& deletedBoard)
//{
//	deletedBoard = bitBoard;
//	int comboCount = 0;
//	for (int i = 0, shifted = W - 1; i < W; i++, shifted--)
//	{
//		int tate = (bitBoard << i) & TATE_LINE;
//		int yoko = tateToYoko(tate);
//
//		auto pattern = comboPatterns[yoko];
//		if (pattern == NO_COMBO) continue;
//
//		comboCount++;
//
//		int yokoDeleted = yoko & comboToDelete[yoko];
//		tate = yokoToTateTable[yokoDeleted];
//		deletedBoard = (~(TATE_LINE >> i) & deletedBoard) | (tate >> i);
//	}
//	return comboCount;
//}
//
////コンボで消す処理
//int comboDelete(vector<int>& bitBoards)
//{
//	int comboCount = 0;
//	for (auto& board : bitBoards)
//	{
//		int yokoDeleted, tateDeleted;
//
//		comboCount += yokoCombo(board, yokoDeleted);
//		comboCount += tateCombo(board, tateDeleted);
//
//		board = yokoDeleted & tateDeleted;
//		//TODO: 横と縦がつながってるパターンでコンボカウントがずれるのを修正
//		//(yokoDeleted & tateDeleted) ^ boardのbitCountがyokoとtateのbitCountにズレがあった場合BFSするか
//		//消された部分をvectorで受け取って二重ループで判定//こっちのほうが速い気がする
//	}
//	return comboCount;
//}
//
////コンボメモ
////pair<int, int> memo[1 << 6]の配列にその横の配置でどこからどこまで連結されているかを事前に計算しておく
////縦のコンボも横のbitに変換してこの配列で見る
//
//
////道順記憶メモ 前の移動の参照を持ちそれをたどる。
////参照カウンタを使って自動的に削除されるように
////前の移動の参照の持ち方はbitの状態を記憶するのは勿体無いので移動させたボールの位置のみにする。向きは位置から判別できる。これもbitに入れれば位置で8bitで済む。
//
//
///*解法メモ
//基本的に時間で制限
//まず普通にbfs
//メモ付きも試す
//ビームサーチ
//ビーム幅を調整
//*/
//
//#pragma region test
//void test_toALLBoard(vector<int>& bitBoards)
//{
//	int allBoard = toAllBoard(bitBoards);
//	cout << toStringBitBoard(allBoard) << endl;
//}
//
//void test_pushDownBitBoard(vector<int>& bitBoards)
//{
//	int allBoard = toAllBoard(bitBoards);
//	cout << toStringBitBoard(allBoard) << endl;
//	int pushedBoard = pushDownBitBoard(allBoard, 0, 0);
//	pushDownBoards(bitBoards);
//	int i = 0;
//	for (auto board : bitBoards)
//	{
//		cout << ++i << endl;
//		cout << toStringBitBoard(board) << endl;
//	}
//}
//
//void test_swapBall(vector<int>& bitBoards)
//{
//	swapBall(bitBoards, 0, 0, 0, 1, 0, 1);
//	cout << toStringBitBoard(bitBoards[0]) << endl;
//	cout << toStringBitBoard(bitBoards[1]) << endl;
//	cout << toStringBitBoards(bitBoards) << endl;
//}
//
//void test_yokoCombo(vector<int>& bitBoards)
//{
//	int deleted;
//	int ret = yokoCombo(bitBoards[0], deleted);
//	cout << ret << endl;
//	cout << toStringBitBoard(deleted) << endl;
//}
//
//void test_tateCombo(vector<int>& bitBoards)
//{
//	int deleted;
//	int ret = tateCombo(bitBoards[0], deleted);
//	cout << ret << endl;
//	cout << toStringBitBoard(deleted) << endl;
//}
//
//void test_comboDeleted(vector<int>& bitBoards)
//{
//	int count = comboDelete(bitBoards);
//	cout << count << endl;
//	cout << toStringBitBoards(bitBoards) << endl;
//}
//
//#pragma endregion
int hoge;