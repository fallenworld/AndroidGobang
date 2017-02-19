#include "GobangJni.h"
#include "Gobang.h"


JNIEXPORT jint JNICALL Java_whu_zq_gobang_JniUtils_AINext
        (JNIEnv * env, jclass java_class, jintArray java_array)
{
    //获取到棋盘
	Gobang::Board board;
    jint array[Gobang::BOARD_SIZE];
    env->GetIntArrayRegion(java_array, 0, 15 * 15, array);
    for (int i = 0; i < Gobang::BOARD_SIZE; i++)
    {
        if (array[i] == Gobang::BLACK)
        {
            board.bit[Gobang::BLACK].set(i);
        }
        else if (array[i] == Gobang::WHITE)
        {
            board.bit[Gobang::WHITE].set(i);
        }
    }
	Gobang* gobang = Gobang::getInstance();
	return gobang->move(board, 9000);
}
