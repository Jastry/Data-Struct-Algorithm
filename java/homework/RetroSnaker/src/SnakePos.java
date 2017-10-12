import java.util.LinkedList;
/**
 * 
 * 
 * 
 *         格子类 （或者可以理解成表示蛇的一块身体的类）
 */
public class SnakePos {

    public int col;
    public int row;
    // 一块蛇身体的位置坐标
    // 设置为public方便调用

    /**
     * 行列构造器，表示这一块身体在游戏盘中所处的行列
     * 
     * @param row
     *            所在的行
     * @param col
     *            所在的列
     */
    SnakePos(int row, int col) {
        this.col = col;
        this.row = row;
    }

    /**
     * 留下一个无参的构造器，不是为了调用，而是为了为Food类做方便
     */
    SnakePos() {
        col = 0;
        row = 0;
    }
}


