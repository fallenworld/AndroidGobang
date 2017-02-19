package whu.zq.gobang;

/**
 * Created by Administrator on 2016/5/30.
 * Jni工具类
 */
public class JniUtils {
    static {
        System.loadLibrary("Gobang");
    }

    /**
     * 获取到AI的下一步落子
     * @param board 当前的棋盘
     * @return AI下一步落子位置
     */
    public static native int AINext(int[] board);
}
