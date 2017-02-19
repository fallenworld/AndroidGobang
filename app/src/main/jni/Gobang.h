#ifndef GOBANG_H_
#define GOBANG_H_

#include <bitset>
#include <string>
#include <vector>
#include <tuple>
#include <list>

class Gobang
{
public:
	static const int BLACK = 0;
	static const int WHITE = 1;
	static const int NONE = 8;

	static const int INF = 0xfffffff;

	static const int BOARD_WIDTH = 15;  //棋盘宽度
	static const int BOARD_SIZE = 225;  //棋盘中落子位置的个数

										//矩形
	struct Rect
	{
		int left;
		int top;
		int right;
		int bottom;
	};

	//棋盘
	struct Board
	{
		std::bitset<BOARD_SIZE> bit[2];
	};

	//置位表表项
	struct HashTable
	{
		//flag的取值
		static const int NONE = 0;   //表项尚未填充
		static const int EXACT = 1;  //该值是节点的实际值
		static const int BETA = 2;   //该值是BETA截断时（即实际值>=value）
		static const int ALPHA = 3;  //该值是ALPHA截断时（即实际值<=value）

		//键值
		uint64_t key;
		//值的意义
		int flag;
		//存储的minMax值
		int value;
		//深度
		int depth;
		//最好的移动策略
		int best_move;
	};

	//minMax函数返回的结果
	struct MinMax
	{
		int value;      //极大极小值
		int best_move;    //最佳的移动位置
	};

	//获取类的单例
	static Gobang* getInstance();

	//极大极小值算法，以下为所有用到的优化策略：
	//1.α-β剪枝，
	//2.迭代加深地进行搜索
	//3.使用启发式评估函数
	//5.只在不超过当前有棋子的范围2个点的地方落子
	//6.使用置位表(zobrist键值)
	//7.利用置位表中信息进行着法排序
	//@param board 棋盘
	//@param color 当前要落子的玩家的颜色
	//@param beta  max节点值的上界
	//@param alpha min节点值的下界
	//@param depth 当前搜索深度
	//@param max_depth 最大深度
	//@param border 有棋子区域的边缘
	//@param 上一次的落子位置
	//@return 当前棋局的极大极小值
	int minMax(Board board, int color, int beta, int alpha, int depth, Rect border, int last, std::vector<int>* best_move_list);

	//获取到当前棋局的最佳移动
	int move(const Board& board, int time);

	void test();

private:
	Gobang();
	~Gobang();

	inline char getNumberAscii(int num) { return (char)(num + 48); }

	//棋局的启发式估值函数
	int evaluate(const Board& board, int color, Rect border);

	//计算出棋局的Zobrist Key值
	uint64_t getZobristKey(const Board& board);

	//着法排序
	std::list<std::pair<int, int>> childrenSort(const Board& board, int color, uint64_t zobrist, Rect border);

	//终止测试
	bool terminalTest(const Board& board, int depth, int last, int last_color);

	//向置位表中添加一个记录
	void insertIntoHashTable(uint64_t key, int flag, int value, int depth, int best_move);

	//单例
	static Gobang* instance_;

	//置换表用到的Zobrist数组
	uint64_t zobrist_[BOARD_SIZE][2];

	//24M大小的置位表
	HashTable* hashTable_;
	int table_size_ = 0x100000;

	//不同类型的棋型字符串和其在棋型列表中的下标(棋型字符串中：8是没有子，0是黑子，1是白子)
	std::vector<std::pair<std::string, int>> pattern_list;

	//不同棋型的得分
	std::vector<std::tuple<int, int, int>> pattern_value_list;
};

#endif