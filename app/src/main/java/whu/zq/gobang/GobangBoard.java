package whu.zq.gobang;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Pair;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by fallenworld on 2016/5/16.
 * 五子棋的棋盘
 */
class GobangBoard extends FrameLayout{
    // 棋子颜色
    public final static int NONE = -1;  //无棋子
    public final static int BLACK = 0;  //黑棋
    public final static int WHITE = 1;  //白棋

    private Grid mGrid;
    private Focus mFocus;

    // 当前棋盘上的棋子分布，每一个数组元素可能的取值为NONE(-1), BLACK(0), WHITE(1)
    private int[][] board = new int[15][15];

    public GobangBoard(Context context) {
        super(context);
        init(context);
    }

    public GobangBoard(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context) {
        // 初始化棋子分布
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j ++) {
                board[i][j] = NONE;
            }
        }
        // 显示网格线
        mGrid = new Grid(context);
        LayoutParams gridLayoutParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        mGrid.setLayoutParams(gridLayoutParams);
        addView(mGrid);
        getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                if (mFocus == null) {
                    // 创建落子位置选择框
                    final float squareWidth = mGrid.getWidth() / 15.0F;   // 棋盘一格的宽度
                    mFocus = new Focus(getContext());
                    LayoutParams focusLayoutParams = new LayoutParams((int)squareWidth, (int)squareWidth);
                    mFocus.setLayoutParams(focusLayoutParams);
                    mFocus.setPos(7, 7);
                    addView(mFocus);
                }
            }
        });
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int widthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSpecSize = MeasureSpec.getSize(heightMeasureSpec);
        int boardLength = Math.min(widthSpecSize, heightSpecSize);
        setMeasuredDimension(boardLength, boardLength);
    }

    /**
     * 获取到当前棋盘上的棋子分布
     */
    public int[][] getBoard() { return board; }

    /**
     * 在棋盘上落子
     * @return 这一子落下后是否胜利
     */
    public boolean addPiece(int x, int y, int color) {
        if ((color == BLACK || color == WHITE) && board[x][y] == GobangBoard.NONE) {
            ImageView piece = new ImageView(getContext());
            piece.setImageResource((color == BLACK) ? R.drawable.black_piece : R.drawable.white_piece);
            // 棋子图片的宽度
            float squareWidth = mGrid.getWidth() / 15.0F;
            LayoutParams layoutParams = new LayoutParams((int)squareWidth, (int)squareWidth);
            // 设置棋子的位置
            board[x][y] = color;
            layoutParams.setMargins((int)(mGrid.getX() + squareWidth * x), (int)(mGrid.getX() + squareWidth * y), 0, 0);
            piece.setLayoutParams(layoutParams);
            piece.setScaleType(ImageView.ScaleType.FIT_XY);
            addView(piece);
            // 判断是否胜利:
            // 四个方向上前进一格的偏移量
            List<Pair<Integer, Integer>> shifts = new ArrayList<>();
            shifts.add(new Pair<>(0, 1));
            shifts.add(new Pair<>(1, 1));
            shifts.add(new Pair<>(1, 0));
            shifts.add(new Pair<>(1, -1));
            // 分别判断四个方向上是否五子连珠
            for (Pair<Integer, Integer> shift : shifts) {
                int startX = x;
                int startY = y;
                while (0 <= startX && startX < 15 && 0 <= startY && startY < 15 && board[startX][startY] == color ) {
                    startX += shift.first;
                    startY += shift.second;
                }
                int endX = x;
                int endY = y;
                while (0 <= endX && endX < 15 && 0 <= endY && endY < 15 && board[endX][endY] == color) {
                    endX -= shift.first;
                    endY -= shift.second;
                }
                // (startX, startY) 到 (endX, endY)之间全部是当前所下颜色的棋子
                // 因此判断endX - startX的绝对值是否大于5即可判断是否五子连珠
                //int xLength = Math.abs(endX - startX) - 1;
                //int yLength = Math.abs(endY - startY) - 1;
                if (Math.abs(endX - startX) - 1 >= 5 || Math.abs(endY - startY) - 1 >= 5) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * 清空棋盘
     */
    public void clear() {
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j ++) {
                board[i][j] = NONE;
            }
        }
        removeAllViews();
        addView(mGrid);
        mFocus.setPos(7, 7);
        addView(mFocus);
    }

    /**
     * 设置落子选择框的位置
     */
    public void setFocus(int x, int y) {
        mFocus.setPos(x, y);
        GobangBoard.this.removeView(mFocus);
        GobangBoard.this.addView(mFocus);
        mFocus.setVisibility(View.VISIBLE);
    }

    // 获取落子选择框的位置
    public int getFocusPosX() { return mFocus.getFocusPosX(); }
    public int getFocusPosY() { return mFocus.getFocusPosY(); }

    /**
     * 将落子选择框隐藏
     */
    public void hideFocus() { mFocus.hide(); }

    /**
     * 落子选择框当前是否可见
     */
    public boolean isFocusVisible() { return mFocus.getVisibility() == View.VISIBLE; }

    /**
     * 棋盘上的网格
     */
    class Grid extends View {

        public Grid(Context context) { super(context); }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            // 棋盘一格宽度
            float squareWidth = getWidth() / 15.0F;
            // 棋盘最左边的线到View最左边的距离
            float padding = squareWidth / 2.0F;
            Paint paint = new Paint();
            paint.setAntiAlias(true);
            paint.setStrokeWidth(8);
            paint.setStyle(Paint.Style.STROKE);
            paint.setColor(Color.BLACK);
            // 画出最外边的矩形框
            canvas.drawRect(padding, padding,getWidth() - padding, getWidth() - padding, paint);
            // 画出剩余的网格线
            paint.setStrokeWidth(2);
            // 横向的线
            for (int i = 1; i <=13; i++) {
                canvas.drawLine(padding + squareWidth * i, padding, padding +  squareWidth * i,getWidth()- padding, paint);
            }
            // 纵向的线
            for (int i = 1; i <=13; i++) {
                canvas.drawLine(padding, padding + squareWidth * i, getWidth()- padding, padding + squareWidth * i, paint);
            }
            // 绘制中心点
            paint.setStrokeWidth(1);
            paint.setStyle(Paint.Style.FILL);
            canvas.drawCircle(padding + squareWidth * 7, padding + squareWidth * 7, 10, paint);
        }
    }

    /**
     * 选择落子位置的选择框
     */
    class Focus extends View {
        // 当前选择框在棋盘上的坐标，注意坐标范围是0-14
        private int x = -1;
        private int y = -1;

        public Focus(Context context) { super(context); }

        @Override
        protected void onDraw(Canvas canvas) {
            Paint paint = new Paint();
            paint.setAntiAlias(true);
            paint.setStyle(Paint.Style.STROKE);
            paint.setColor(Color.RED);
            paint.setStrokeWidth(Math.max(1, canvas.getWidth() / 4));
            float width = canvas.getWidth();
            float length = width / 3.0F;
            // 画出四个直角框
            canvas.drawLines(new float[] {0, 0, length, 0, 0, 0, 0, length}, paint);
            canvas.drawLines(new float[] {0, width, length, width, 0, width, 0, width - length}, paint);
            canvas.drawLines(new float[] {width, 0, width - length, 0, width, 0, width, length}, paint);
            canvas.drawLines(new float[] {width, width, width - length, width, width, width, width, width - length}, paint);
        }

        /**
         * 设置框框在棋盘上的坐标
         */
        public void setPos(int x, int y) {
            this.x = x;
            this.y = y;
            // 棋盘上一格的宽度
            float squareWidth = mGrid.getWidth() / 15.0F;
            // 要减去的偏移量（这个偏移量大小即为框框宽度的一半）
            float delta = squareWidth / 2.0F;
            // 设置坐标
            LayoutParams layoutParams = (FrameLayout.LayoutParams)getLayoutParams();
            layoutParams.setMargins((int)(mGrid.getX() + squareWidth * x), (int)(mGrid.getY() + squareWidth * y) ,0 ,0);
            setLayoutParams(layoutParams);
        }

        /**
         * 将选择框隐藏起来
         */
        public void hide() {
            x = -1;
            y = -1;
            setVisibility(View.INVISIBLE);
        }

        public int getFocusPosX() { return x; }
        public int getFocusPosY() { return y; }
    }
}