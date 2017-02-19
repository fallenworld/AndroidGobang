package whu.zq.gobang;

import android.app.Dialog;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.Button;

public class GameActivity extends AppCompatActivity {
    GobangBoard mBoard;
    Button mOKButton;
    Button mUpButton;
    Button mDownButton;
    Button mLeftButton;
    Button mRightButton;

    // 玩家的棋子颜色
    int playerColor = GobangBoard.BLACK;
    // 当前回合下棋人的颜色
    int turn = GobangBoard.BLACK;
    // 棋盘触摸事件监听器
    View.OnTouchListener boardOnTouchListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game);
        findViews();
        init();
    }

    private void findViews() {
        mBoard = (GobangBoard)findViewById(R.id.board);
        mOKButton = (Button)findViewById(R.id.ok_btn);
        mUpButton = (Button)findViewById(R.id.up_btn);
        mDownButton = (Button)findViewById(R.id.down_btn);
        mLeftButton = (Button)findViewById(R.id.left_btn);
        mRightButton = (Button)findViewById(R.id.right_btn);
    }

    private void init() {
        mOKButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (turn == playerColor && mBoard.isFocusVisible()
                        && mBoard.getBoard()[mBoard.getFocusPosX()][mBoard.getFocusPosY()] == GobangBoard.NONE) {
                    boolean victory = mBoard.addPiece(mBoard.getFocusPosX(), mBoard.getFocusPosY(), GobangBoard.BLACK);
                    if (victory) {
                        new AlertDialog.Builder(GameActivity.this)
                                .setMessage("恭喜你取得胜利！")
                                .setPositiveButton("确定", null)
                                .create().show();
                        mBoard.clear();
                        turn = GobangBoard.BLACK;
                        return;
                    }
                    turn = (turn == GobangBoard.BLACK) ? GobangBoard.WHITE : GobangBoard.BLACK;
                    onTurnChanged();
                }
            }
        });
        mUpButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (turn == playerColor && mBoard.isFocusVisible() && mBoard.getFocusPosY() > 0)  {
                    mBoard.setFocus(mBoard.getFocusPosX(), mBoard.getFocusPosY() - 1);
                }
            }
        });
        mDownButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (turn == playerColor && mBoard.isFocusVisible() && mBoard.getFocusPosY() < 14) {
                    mBoard.setFocus(mBoard.getFocusPosX(), mBoard.getFocusPosY() + 1);
                }
            }
        });
        mLeftButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (turn == playerColor && mBoard.isFocusVisible() && mBoard.getFocusPosX() > 0) {
                    mBoard.setFocus(mBoard.getFocusPosX() - 1, mBoard.getFocusPosY());
                }
            }
        });
        mRightButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (turn == playerColor && mBoard.isFocusVisible() && mBoard.getFocusPosX() < 14) {
                    mBoard.setFocus(mBoard.getFocusPosX() + 1, mBoard.getFocusPosY());
                }
            }
        });
        mBoard.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                if (boardOnTouchListener == null) {
                    boardOnTouchListener = new View.OnTouchListener() {
                        @Override
                        public boolean onTouch(View v, MotionEvent event) {
                            if (turn == playerColor) {
                                float squareWidth = mBoard.getWidth() / 15.0F;
                                // 判断触摸点的网格坐标
                                int x = (int)(event.getX() / squareWidth + 0.5);
                                int y = (int)(event.getY() / squareWidth + 0.5);
                                if (0 <= x && x < 15 && 0 <= y && y < 15) {
                                    mBoard.setFocus(x, y);
                                }
                                return true;
                            }
                            return false;
                        }
                    };
                }
                // 给棋盘上的网格线添加触摸监听：触摸时显示落子Focus
                mBoard.setOnTouchListener(boardOnTouchListener);
            }
        });
    }

    /**
     * 换另一方下棋时的回调
     */
    public void onTurnChanged() {
        // 若此时轮到电脑AI下棋
        if (turn != playerColor) {
            AITurn();
        }
    }

    /**
     * 电脑AI下棋
     */
    private void AITurn() {
        int AIColor = (playerColor == GobangBoard.BLACK) ? GobangBoard.WHITE : GobangBoard.BLACK;
        int[] boardParam = new int[15 * 15];
        //把二维数组内容复制到一维数组
        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 15; x++)
            {
                boardParam[y * 15 + x] = mBoard.getBoard()[x][y];
            }
        }
        int nextStep = JniUtils.AINext(boardParam);
        int nextX = nextStep % 15;
        int nextY = nextStep / 15;
        mBoard.setFocus(nextX, nextY);
        boolean gameOver = mBoard.addPiece(nextX, nextY, AIColor);
        if (gameOver) {
            new AlertDialog.Builder(GameActivity.this)
                    .setMessage("很遗憾，你输了")
                    .setPositiveButton("确定", null)
                    .create().show();
            mBoard.clear();
            turn = GobangBoard.BLACK;
            return;
        }
        turn = (turn == GobangBoard.BLACK) ? GobangBoard.WHITE : GobangBoard.BLACK;
        onTurnChanged();
    }

    static class Point {
        public int x;
        public int y;
    }
}
