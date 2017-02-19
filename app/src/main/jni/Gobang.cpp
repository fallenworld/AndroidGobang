#include <iostream>
#include <algorithm>
#include <random>
#include <cstdio>
#include <list>
#include <ctime>
#include "Gobang.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


void printBoard(const Gobang::Board& board)
{
	using namespace std;
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 15; x++)
		{
			if (board.bit[0].test(y * 15 + x))
			{
				cout << 0;
			}
			else if (board.bit[1].test(y * 15 + x))
			{
				cout << 1;
			}
			else
			{
				cout << 8;
			}
		}
		cout << endl;
	}
}

Gobang* Gobang::instance_ = nullptr;

Gobang* Gobang::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Gobang();
	}
	return instance_;
}

Gobang::Gobang()
{
	using namespace std;
    //初始化置位表
	hashTable_ = new HashTable[table_size_];
	for (int i = 0; i < table_size_; i++)
	{
		hashTable_[i].key = 0;
		hashTable_[i].flag = HashTable::NONE;
		hashTable_[i].value = -1;
		hashTable_[i].depth = -1;
		hashTable_[i].best_move = -1;
	}
	//产生Zobrist随机值
	std::random_device rd;
	std::mt19937_64 gen(rd());
	for (int color = 0; color < 2; color++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			zobrist_[i][color] = gen();
		}
	}
	//单个棋型的字符串:
	//黑色长连
	pattern_list.push_back(make_pair("00000", 0));
	//白色长连
	pattern_list.push_back(make_pair("11111", 7));
	//黑色活四
	pattern_list.push_back(make_pair("800008", 1));
	//白色活四
	pattern_list.push_back(make_pair("811118", 8));
	//黑色冲四
	pattern_list.push_back(make_pair("800001", 2));
    pattern_list.push_back(make_pair("100008", 2));
	pattern_list.push_back(make_pair("8080008", 2));
	pattern_list.push_back(make_pair("8000808", 2));
	pattern_list.push_back(make_pair("8008008", 2));
	//白色冲四
	pattern_list.push_back(make_pair("811110", 9));
	pattern_list.push_back(make_pair("011118", 9));
	pattern_list.push_back(make_pair("8181118", 9));
	pattern_list.push_back(make_pair("8111818", 9));
	pattern_list.push_back(make_pair("8118118", 9));
	//黑色活三
	pattern_list.push_back(make_pair("80008", 3));
	pattern_list.push_back(make_pair("800808", 3));
	pattern_list.push_back(make_pair("808008", 3));
	//白色活三
	pattern_list.push_back(make_pair("81118", 10));
	pattern_list.push_back(make_pair("811818", 10));
	pattern_list.push_back(make_pair("818118", 10));
	//黑色眠三
	pattern_list.push_back(make_pair("880001", 4));
	pattern_list.push_back(make_pair("100088", 4));
	pattern_list.push_back(make_pair("800801", 4));
	pattern_list.push_back(make_pair("108008", 4));
	pattern_list.push_back(make_pair("808001", 4));
	pattern_list.push_back(make_pair("100808", 4));
	pattern_list.push_back(make_pair("08800", 4));
	pattern_list.push_back(make_pair("00880", 4));
	pattern_list.push_back(make_pair("080808", 4));
	pattern_list.push_back(make_pair("1800081", 4));
	//白色眠三
	pattern_list.push_back(make_pair("881110", 11));
	pattern_list.push_back(make_pair("011188", 11));
	pattern_list.push_back(make_pair("811810", 11));
	pattern_list.push_back(make_pair("018118", 11));
	pattern_list.push_back(make_pair("818110", 11));
	pattern_list.push_back(make_pair("011818", 11));
	pattern_list.push_back(make_pair("18811", 11));
	pattern_list.push_back(make_pair("11881", 11));
	pattern_list.push_back(make_pair("181818", 11));
	pattern_list.push_back(make_pair("0811180", 11));
	//黑色活二
	pattern_list.push_back(make_pair("88008", 5));
	pattern_list.push_back(make_pair("80088", 5));
	pattern_list.push_back(make_pair("80808", 5));
	pattern_list.push_back(make_pair("808808", 5));
	//白色活二
	pattern_list.push_back(make_pair("88118", 12));
	pattern_list.push_back(make_pair("81188", 12));
	pattern_list.push_back(make_pair("81818", 12));
	pattern_list.push_back(make_pair("818818", 12));
	//黑色眠二
	pattern_list.push_back(make_pair("888001", 6));
	pattern_list.push_back(make_pair("100888", 6));
	pattern_list.push_back(make_pair("880801", 6));
	pattern_list.push_back(make_pair("108088", 6));
	pattern_list.push_back(make_pair("808801", 6));
	pattern_list.push_back(make_pair("108808", 6));
	pattern_list.push_back(make_pair("08880", 6));
	pattern_list.push_back(make_pair("1808081", 6));
	pattern_list.push_back(make_pair("1800881", 6));
	pattern_list.push_back(make_pair("1880081", 6));
	//白色眠二
	pattern_list.push_back(make_pair("888110", 13));
	pattern_list.push_back(make_pair("011888", 13));
	pattern_list.push_back(make_pair("881810", 13));
	pattern_list.push_back(make_pair("018188", 13));
	pattern_list.push_back(make_pair("818810", 13));
	pattern_list.push_back(make_pair("018818", 13));
	pattern_list.push_back(make_pair("18881", 13));
	pattern_list.push_back(make_pair("0818180", 13));
	pattern_list.push_back(make_pair("0811880", 13));
	pattern_list.push_back(make_pair("0881180", 13));
	//棋型的得分：
	//长连
	//0:黑色长连， 1:黑色活四，2:黑色冲四，3:黑色活三，4.黑色眠三，5：黑色活二，6.黑色眠二
	//7:白色长连， 8:白色活四，9:白色冲四，10:白色活三，11.白色眠三，12：白色活二，13.白色眠二
	pattern_value_list.push_back(make_tuple(-1, 0, 100000));   //黑色长连
	pattern_value_list.push_back(make_tuple(-1, 7, -100000));  //白色长连
	pattern_value_list.push_back(make_tuple(-1, 1, 10000));    //黑色活四
	pattern_value_list.push_back(make_tuple(-1, 8, -10000));   //白色活四
	pattern_value_list.push_back(make_tuple(2, 2, 10000));     //黑色双冲四
	pattern_value_list.push_back(make_tuple(9, 9, -10000));    //白色双冲四
	pattern_value_list.push_back(make_tuple(2, 3, 10000));     //黑色冲四活三
	pattern_value_list.push_back(make_tuple(9, 10, -10000));   //白色冲四活三
	pattern_value_list.push_back(make_tuple(3, 3, 5000));      //黑色双活三
	pattern_value_list.push_back(make_tuple(10, 10, -5000));   //白色双活三
	pattern_value_list.push_back(make_tuple(3, 4, 1000));      //黑色活三眠三
	pattern_value_list.push_back(make_tuple(10, 11, -1000));   //白色活三眠三
	pattern_value_list.push_back(make_tuple(-1, 2, 500));      //黑色冲四
	pattern_value_list.push_back(make_tuple(-1, 9, -500));     //白色冲四
	pattern_value_list.push_back(make_tuple(-1, 3, 200));      //黑色活三
	pattern_value_list.push_back(make_tuple(-1, 10, -200));    //白色活三
	pattern_value_list.push_back(make_tuple(5, 5, 100));       //黑色双活二
	pattern_value_list.push_back(make_tuple(12, 12, -100));    //白色双活二
	pattern_value_list.push_back(make_tuple(-1, 4, 50));       //黑色眠三
	pattern_value_list.push_back(make_tuple(-1, 11, -50));     //白色眠三
	pattern_value_list.push_back(make_tuple(5, 6, 10));        //黑色活二眠二
	pattern_value_list.push_back(make_tuple(12, 13, -10));     //白色活二眠二
	pattern_value_list.push_back(make_tuple(-1, 5, 5));        //黑色活二
	pattern_value_list.push_back(make_tuple(-1, 12, -5));      //白色活二
	pattern_value_list.push_back(make_tuple(-1, 6, 3));        //黑色眠二
pattern_value_list.push_back(make_tuple(-1, 13, -3));      //白色眠二
}

Gobang::~Gobang()
{
	delete hashTable_;
}

uint64_t Gobang::getZobristKey(const Board& board)
{
	uint64_t key = 0;
	for (int color = 0; color < 2; color++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			if (board.bit[color].test(i))
			{
				key ^= zobrist_[i][color];
			}
		}
	}
	return key;
}

std::list<std::pair<int, int>> Gobang::childrenSort(const Board& board, int color, uint64_t zobrist, Rect border)
{
	using namespace std;
	list<pair<int, int>> children;
	int pos;
	int histroy_best = -1;
	//根据Zobrist Key在置位表中找到历史记录的最佳移动
	HashTable* hash_node = hashTable_ + zobrist % table_size_;
	if (hash_node->flag != HashTable::NONE && hash_node->key == zobrist)
	{
		histroy_best = hash_node->best_move;
	}
	//遍历所有可能的落子位置
	for (int y = (border.top - 2 >= 0) ? border.top - 2 : 0; y < ((border.bottom + 2 < BOARD_WIDTH) ? border.bottom + 3 : BOARD_WIDTH); y++)
	{
		for (int x = (border.left - 2 >= 0) ? border.left - 2 : 0; x < ((border.right + 2 <= 14) ? border.right + 3 : BOARD_WIDTH); x++)
		{
			pos = y * BOARD_WIDTH + x;
			//判断该点是否可以落子
			if (board.bit[BLACK].test(pos) || board.bit[WHITE].test(pos)) continue;
			//判断该子节点是不是记录到的历史最佳移动
			if (pos == histroy_best)
			{
				children.insert(children.begin(), make_pair(pos, (color == BLACK) ? INF : -INF));
				continue;
			}
			//从置位表中寻找该节点
			uint64_t new_zobrist = zobrist ^ zobrist_[pos][color];
			hash_node = hashTable_ + new_zobrist % table_size_;
			if (hash_node->flag != HashTable::NONE && hash_node->key == new_zobrist)
			{
				list<pair<int, int>>::iterator iter;
				for (iter = children.begin(); iter != children.end(); iter++)
				{
					if ((color == BLACK && hash_node->value > iter->second)
						|| (color == WHITE && hash_node->value < iter->second))
					{
						children.insert(iter, make_pair(pos, hash_node->value));
						iter = children.begin();
						break;
					}
				}
				if (iter == children.end())
				{
					children.insert(iter, make_pair(pos, hash_node->value));
				}
			}
			else
			{
				children.push_back(make_pair(pos, (color == BLACK) ? -INF : INF));
			}
		}
	}
	return children;
}

bool Gobang::terminalTest(const Board & board, int depth, int last, int last_color)
{
	if (depth == 0)
	{
		return true;
	}
	//测试五子连珠
	int delta_x[4] = {0, 1, 1, 1};
	int delta_y[4] = {1, 1, 0, -1};
	for (int i = 0; i < 4; i++)
	{
		int startX = last % BOARD_WIDTH;
		int startY = last / BOARD_WIDTH;
		int endX = last % BOARD_WIDTH;
		int endY = last / BOARD_WIDTH;
		int pos = startX + startY * BOARD_WIDTH;
		while (0 <= startX && startX < BOARD_WIDTH && 0 <= startY && startY < BOARD_WIDTH
			&& board.bit[last_color].test(pos))
		{
			startX += delta_x[i];
			startY += delta_y[i];
			pos = startX + startY * BOARD_WIDTH;
		}
		while (0 <= endX && endX < BOARD_WIDTH && 0 <= endY && endY < BOARD_WIDTH
			&& board.bit[last_color].test(pos))
		{
			endX -= delta_x[i];
			endY -= delta_y[i];
			pos = endX + endY * BOARD_WIDTH;
		}
		// (startX, startY) 到 (endX, endY)之间全部是当前所下颜色的棋子
		// 因此判断endX - startX的绝对值是否大于5即可判断是否五子连珠
		//int xLength = Math.abs(endX - startX) - 1;
		//int yLength = Math.abs(endY - startY) - 1;
		if (abs(endX - startX) - 1 >= 5 || abs(endY - startY) - 1 >= 5)
		{
			return true;
		}
	}
	return false;
}

int count = 0;

int Gobang::move(const Board& board, int time)
{
	using namespace std;
	Rect border = { -1 ,-1, -1, -1 };
	for (int pos = 0; pos < BOARD_SIZE; pos++)
	{
		if (board.bit[BLACK].test(pos) || board.bit[WHITE].test(pos))
		{
			int x = pos % BOARD_WIDTH;
			int y = pos / BOARD_WIDTH;
			if (border.left == -1 || x < border.left)
			{
				border.left = x;
			}
			if (border.top == -1 || y < border.top)
			{
				border.top = y;
			}
			if (border.right == -1 || x > border.right)
			{
				border.right = x;
			}
			if (border.bottom == -1 || y > border.bottom)
			{
				border.bottom = y;
			}
		}
	}
	vector<int> best_move_list;
	int depth ;
	clock_t start_time = clock();
	clock_t delta_time = 0;
	//迭代加深地进行搜索
	for (depth = 1; delta_time < time / 2; depth++)
	{
		minMax(board, WHITE, INF, -INF, depth, border, 67, &best_move_list);
		delta_time = clock() - start_time;
		/*
		if (depth == 2)
		{
			int x = 1;
		}
		*/
	}
	//返回最佳着法列表中的随机一个着法
	random_device rd;
	mt19937_64 gen(rd());
	uniform_int_distribution<int> dist(0, best_move_list.size() - 1);
	return best_move_list.at(dist(gen));
}

int Gobang::minMax(Board board, int color, int beta, int alpha, int depth, Rect border, int last, std::vector<int>* best_move_list)
{
	count++;
	using namespace std;
	uint64_t zobrist = getZobristKey(board);           //置位表中的Hash键值
	int new_color = (color == BLACK) ? WHITE : BLACK;  //下一个落子的人的颜色
	int best_move = -1;                                //最佳落子位置
	int value = (color == BLACK) ? -INF : INF;         //最终的返回值 
	int child_min_max_value = 0;                       //子节点的极大极小值
	//清空最佳移动位置的列表
	if (best_move_list != nullptr)
	{
		best_move_list->clear();
	}
	//判断是否已经在置位表中
	HashTable* table_node = hashTable_ + zobrist % table_size_;
	if (table_node->flag != HashTable::NONE && table_node->key == zobrist)
	{
		best_move = table_node->best_move;
		if (table_node->depth >= depth)
		{
			if (best_move_list != nullptr)
			{
				best_move_list->push_back(best_move);
			}
			return table_node->value;
		}
	}
	//截断测试
	if (terminalTest(board, depth, last, new_color))
	{
		value = evaluate(board, color, border);
		insertIntoHashTable(zobrist, HashTable::EXACT, value, depth, -1);
		return value;
	}
	//获得着法序列并遍历
	auto children = childrenSort(board, color, zobrist, border);
	for (auto& child : children)
	{
		int pos = child.first;
		int x = pos % BOARD_WIDTH;
		int y = pos / BOARD_WIDTH;
		//更新边缘值和棋盘
		Rect new_border = Rect{ MIN(border.left, x), MIN(border.top, y), MAX(border.right, x), MAX(border.bottom, y) };
		Board new_board = board;
		new_board.bit[color].set(pos);
		//搜索子节点
		child_min_max_value = minMax(new_board, new_color, beta, alpha, depth - 1, new_border, pos, nullptr);
		if (child_min_max_value == value && best_move_list != nullptr)
		{
			best_move_list->push_back(pos);
			continue;
		}
		if (color == BLACK)  //黑棋落子
		{
			if (child_min_max_value > value)
			{
				value = child_min_max_value;
				best_move = pos;
				if (best_move_list != nullptr)
				{
					best_move_list->clear();
					best_move_list->push_back(best_move);
				}
			}
			if (value > beta)
			{
				insertIntoHashTable(zobrist, HashTable::BETA, value, depth, best_move);
				return value;
			}
			if (value > alpha) alpha = value;
		}
		else                //白棋落子
		{
			if (child_min_max_value < value)
			{
				value = child_min_max_value;
				best_move = pos;
				if (best_move_list != nullptr)
				{
					best_move_list->clear();
					best_move_list->push_back(best_move);
				}
			}
			if (value < alpha)
			{
				insertIntoHashTable(zobrist, HashTable::ALPHA, value, depth, best_move);
				return value;
			}
			if (value < beta) beta = value;
		}
	}
	insertIntoHashTable(zobrist, HashTable::EXACT, value, depth, best_move);
	return value;
}

int Gobang::evaluate(const Board& board, int color, Rect border)
{
	using namespace std;
	//棋型列表
	//数组中各个下标处元素代表的意义：
	//0:黑色长连， 1:黑色活四，2:黑色冲四，3:黑色活三，4.黑色眠三，5：黑色活二，6.黑色眠二
	//7:白色长连， 8:白色活四，9:白色冲四，10:白色活三，11.白色眠三，12：白色活二，13.白色眠二
	int pattern_count[14] = { 0 };
	//分别从四个方向统计各个单独棋型的个数
	string line;
	const int start_y_list[] = { 0, 0, 0, 1, 0, BOARD_WIDTH - 2 };
	const int delta_x[] = {0, 1, 1, 1, 1, 1};
	const int delta_y[] = {1, 0, 1, 1, -1, -1};
	const int start_x_delta[] = {1, 0, 1, 0, 1, 0};
	const int start_y_delta[] = {0, 1, 0, 1, 0, -1};
	int start_time = clock();
	for (int i = 0; i < 6; i++)
	{
		for (int start_x = 0, start_y = start_y_list[i];
		     0 <= start_x && start_x < BOARD_WIDTH && 0 <= start_y && start_y < BOARD_WIDTH;
	         start_x += start_x_delta[i], start_y += start_y_delta[i])
		{
			string line;
			for (int x = start_x, y = start_y;
			     0 <= x && x < BOARD_WIDTH && 0 <= y && y < BOARD_WIDTH;
			     x += delta_x[i], y += delta_y[i])
			{
				int pos = y * BOARD_WIDTH + x;
				if (board.bit[BLACK].test(pos))
				{
					line.push_back(getNumberAscii(BLACK));
				}
				else if (board.bit[WHITE].test(pos))
				{
					line.push_back(getNumberAscii(WHITE));
				}
				else
				{
					line.push_back(getNumberAscii(NONE));
				}
			}
		   //搜索每一个棋型在这一条线上的个数
			for (auto& pattern : pattern_list)
			{
			    for (int off = 0; ; off++)
				{
				    off = line.find(pattern.first, off);
					if (off == string::npos)
					{
						break;
					}
					else
					{
					    pattern_count[pattern.second]++;
					}
				}
			}
		}
	}
	//根据统计出的棋型，计算评估值
	int value = 0;
	for (auto& pattern_value : pattern_value_list)
	{
		if (get<0>(pattern_value) == -1) //单个棋型
		{
			value += pattern_count[get<1>(pattern_value)] * get<2>(pattern_value);
		}
		else  //组合棋型
		{
			int count = MIN(pattern_count[get<0>(pattern_value)], pattern_count[get<1>(pattern_value)]);
			value += count * get<2>(pattern_value);
			pattern_count[get<0>(pattern_value)] -= count;
			pattern_count[get<1>(pattern_value)] -= count;
		}
	}
	return value;
}

void Gobang::insertIntoHashTable(uint64_t key, int flag, int value, int depth, int best_move)
{
	HashTable* node = hashTable_ + key % table_size_;
	if (node->flag == HashTable::NONE
		|| node->depth <= depth)
	{
		*node = { key, flag, value, depth, best_move };
	}
}

void Gobang::test()
{
	using namespace std;
	//读取棋盘
	Board board;
	FILE* fin = fopen("D:/board.txt", "rt");
	if (fin == nullptr) cout << "cant open" << endl;
	int i = 0;
	char c = getc(fin);
	while (c != EOF)
	{
		if (c == '0')
		{
			board.bit[BLACK].set(i);
			i++;
		}
		else if (c == '1')
		{
			board.bit[WHITE].set(i);
			i++;
		}
		else if (c == '8')
		{
			i++;
		}
		c = fgetc(fin);
	}

	Rect border = { -1 ,-1, -1, -1 };
	for (int pos = 0; pos < BOARD_SIZE; pos++)
	{
		if (board.bit[BLACK].test(pos) || board.bit[WHITE].test(pos))
		{
			int x = pos % BOARD_WIDTH;
			int y = pos / BOARD_WIDTH;
			if (border.left == -1 || x < border.left)
			{
				border.left = x;
			}
			if (border.top == -1 || y < border.top)
			{
				border.top = y;
			}
			if (border.right == -1 || x > border.right)
			{
				border.right = x;
			}
			if (border.bottom == -1 || y > border.bottom)
			{
				border.bottom = y;
			}
		}
	}
	int zobrist = getZobristKey(board);
	Board new_board;
	//insertIntoHashTable(getZobristKey(board), HashTable::EXACT, 200, 3, 3);
	/*
	//4
	new_board = board;
	new_board.bit[BLACK].set(4);
	insertIntoHashTable(getZobristKey(new_board), HashTable::EXACT, 100, 5, 20);
	//3
	new_board = board;
	new_board.bit[BLACK].set(3);
	insertIntoHashTable(getZobristKey(new_board), HashTable::EXACT, -50, 5, 20);
	//0
	new_board = board;
	new_board.bit[BLACK].set(0);
	insertIntoHashTable(getZobristKey(new_board), HashTable::EXACT, 200, 5, 20);
	//15
	new_board = board;
	new_board.bit[BLACK].set(15);
	insertIntoHashTable(getZobristKey(new_board), HashTable::EXACT, 30, 5, 20);
	//18
	new_board = board;
	new_board.bit[BLACK].set(18);
	insertIntoHashTable(getZobristKey(new_board), HashTable::EXACT, 230, 5, 20);
	*/
	int best = move(board, 2000);
	int x = best % BOARD_WIDTH;
	int y = best / BOARD_WIDTH;
	cout << x << ',' << y << endl;
	//int start_time = clock();
	//for (int i = 0; i < 100; i++) evaluate(board, BLACK, border);
	//int delta_time = clock() - start_time;
}
