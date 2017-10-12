
import java.util.LinkedList;

/**
 * 
 * 
 * 
 *         蛇的实现类
 */
public class Snake {

    private Direction snakeDir;
    // 当前蛇头所向的方向

    private Direction moveDir;
    // moveDir是从Board类中读取到的方向
    // moveDir是在run方法的一个周期中，通过键盘读取的，蛇头想要改变的方向

    // 这段的逻辑是：我们先从Board的键盘监听处读取玩家想要改变的蛇的方向，但是我们不直接把蛇的方向设置成获取的方向
    // 因为如果玩家在run方法的一个周期中多次按下不同的方向键，可能会导致一些BUG，我们先记录“玩家想要改变成”的方向
    // 然后在移动的时候，获取这个想要改变的方向（moveDir）与现在的方向（snakeDir）进行判断后再处理。

    private Food food;
    // 当前蛇在游戏中的食物，会随着蛇吃下一个食物进行刷新

    private LinkedList<SnakePos> snakeBody;
    // 蛇的身体，由一个个SnakePos单元构成
    // 数据结构是链表，因为随机访问次数少，插入删除次数多

    public static final int Row = Configure.ROW;
    public static final int Column = Configure.COL;
    // 从Configure文件中读取的游戏行列

    public Snake() {
        snakeBody = new LinkedList<SnakePos>();
        reset();
        // 初始化蛇
    }

    public Direction getSnakeDir() {
        return snakeDir;
    }

    public void setSnakeDir(Direction snakeDir) {
        this.snakeDir = snakeDir;
    }

    public LinkedList<SnakePos> getSnakeBody() {
        return snakeBody;
    }

    public Food getFood() {
        return food;
    }

    public void setFood(Food food) {
        this.food = food;
    }

    public void setMoveDir(Direction dir) {
        this.moveDir = dir;
    }

    /**
     * 此方法用来初始化蛇，让蛇变成一条竖直3格长度，方向向上的随机位置新蛇
     */
    public void reset() {
        snakeBody.clear();
        // 清空链表
        SnakePos beginPos = null;
        // 创建一格蛇头的引用
        setMoveDir(null);
        // 将键盘监听的方向设置为null
        do {
            beginPos = this.RandomPos();
            // 调用方法随机放置蛇头位置
        } while (beginPos.row + 3 > Row);
        // 如果蛇头向下三行没接触到底边，这个生成是可以被接受的

        snakeBody.add(beginPos);
        snakeBody.add(new SnakePos(beginPos.row + 1, beginPos.col));
        snakeBody.add(new SnakePos(beginPos.row + 2, beginPos.col));
        // 将三格蛇（包括蛇头）添加到SnakeBody链表中
        setSnakeDir(Direction.UP);
        // 设置方向为向上
    }

    /**
     * 创建一个蛇身体（SnakePos类）对象并随机设置行列，将其返回
     * 
     * @return 行列被随机的一个蛇身体对象
     */
    private SnakePos RandomPos() {

        int randomRow = (int) (Math.random() * Row);
        int randomCol = (int) (Math.random() * Column);

        return new SnakePos(randomRow, randomCol);
    }

    /**
     * 控制蛇的移动
     */
    public void snakeMove() {

        int addRow = snakeBody.getFirst().row;
        int addCol = snakeBody.getFirst().col;
        // 想要添加的新蛇头必定是原蛇头相邻某个方向的一块
        // 先将新蛇头的行列设置为原蛇头的行列

        Direction up = Direction.UP;
        Direction down = Direction.DOWN;
        Direction left = Direction.LEFT;
        Direction right = Direction.RIGHT;
        // 创建Direction枚举类的四个引用，为了少写几个字（嘿嘿）

        if ((moveDir != null)
                && !((snakeDir == up && moveDir == down)
                        || (snakeDir == down && moveDir == up)
                        || (snakeDir == left && moveDir == right) || (snakeDir == right && moveDir == left)))
            snakeDir = moveDir;
        // 如果符合条件，就将键盘监听的moveDir方向设置为最新的蛇头方向

        switch (snakeDir) {
        case UP:
            addRow--;
            break;
        case DOWN:
            addRow++;
            break;
        case LEFT:
            addCol--;
            break;
        case RIGHT:
            addCol++;
            break;
        }
        // 根据最新蛇头方向，确定新的蛇头在哪个块生成，修改新蛇头的行列坐标

        SnakePos addPos = new SnakePos(addRow, addCol);
        // 根据这个行列坐标，创建一个蛇身体（SnakePos）对象

        if (!isFood(addPos))
            snakeBody.removeLast();
        // 如果不是食物，则去掉snakeBody链表中最后一个节点
        else
            setFood(new Food().getSnake(snakeBody));
        // 是食物就重新设置一个食物，不用去掉最后一个节点

        if (isCollision(addPos))
            reset();
        // 如果碰撞了，把这条蛇重置
        else
            snakeBody.addFirst(addPos);
        // 没碰撞就将刚才设置的新蛇头放进链表中
        // 注意，即使是食物也会执行这一句话，因为遇到食物不算是碰撞
    }

    /**
     * 判断一个格是不是食物
     * 
     * @param addPos
     *            要判断的格子
     * @return 返回true表示是食物
     */
    private boolean isFood(SnakePos addPos) {
        if (food.row == addPos.row && food.col == addPos.col)
            return true;
        // 如果传入的行列坐标和这个类中的food变量的行列一样就表示是食物
        return false;
    }

    /**
     * 碰撞检测，如果遇到墙壁或者蛇身体就认为碰撞
     * 
     * @param addPos
     *            要判断是否为墙壁（或蛇身体）的格子
     * @return 会发生碰撞返回true
     */
    private boolean isCollision(SnakePos addPos) {
        if (addPos.row < 0 || addPos.row > Row - 1 || addPos.col < 0
                || addPos.col > Column - 1)
            return true;
        // 如果是墙壁返回true
        for (SnakePos sp : snakeBody)
            if ((sp.row == addPos.row) && (sp.col == addPos.col))
                return true;
        // 如果是蛇身体返回true
        return false;
    }

}

