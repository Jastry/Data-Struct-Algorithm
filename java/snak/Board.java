import java.awt.EventQueue;
import java.awt.KeyEventPostProcessor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;


public class Board {

    public static void main(String[] args) {
        // TODO Auto-generated method stub

        // 开启一个线程，所有的Swing组件必须由事件分派线程进行配置，线程将鼠标点击和按键控制转移到用户接口组件。
        EventQueue.invokeLater(new Runnable() {
            // 匿名内部类，是一个Runnable接口的实例，实现了run方法
            public void run() {

                JFrame frame = new BoardFrame();
                // 创建下面自己定义的SimpleFrame类对象，以便于调用构造器方法

                frame.setTitle("Retro Snake");
                // 设置标题

                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                // 选择当用户关闭框架的时候进行的操作 ，在有些时候需要将窗口隐藏，不能直接退出需要用到这个方法

                frame.setVisible(true);
                // 将窗口可见化，这样以便用户在第一次看见窗口之前我们能够向其中添加内容
            }
        });
    }

}

class BoardFrame extends JFrame {

    private Snake snk;
    // 在我们绘图的工作区域创建一个蛇对象引用

    public static final int INTERVAL = Configure.INTERVAL;

    // 需要用到的睡眠间隔，决定了蛇的移动速度
    // 从Configure文件中读取的游戏时间间隔

    public BoardFrame() {

        snk = new Snake();

        snk.setFood(new Food().getSnake(snk.getSnakeBody()));
        // 创建一个食物对象，调用getSnake方法判断该食物生成点不在蛇的身体上
        // getSnake的返回类型是Food，可以这样直接调用

        final KeyboardFocusManager manager = KeyboardFocusManager
                .getCurrentKeyboardFocusManager();
        // 创建一个键盘监听相关类
        // 因为我们要在下面开启线程，线程中只能获得final修饰的局部变量，所以这个变量是不可变的

        new Thread(new Runnable() {
            // 开启线程来不断重绘蛇
            // 之所以采用多线程，是为了让代码更加灵活，如果要改编成双人贪吃蛇更方便

            public void run() {

                while (true) {
                    BoardComponent bc = new BoardComponent();
                    bc.setSnake(snk);
                    add(bc);
                    // 创建JComponent的实例，将上面创建的蛇对象传入

                    MyKeyEventPostProcessor mke = new MyKeyEventPostProcessor();
                    mke.setSnk(snk);
                    manager.addKeyEventPostProcessor(mke);
                    // 创建监听键盘的实例，同样将蛇对象传入

                    try {
                        Thread.sleep(INTERVAL);
                        // 在运动之间需要间隔，用sleep方法达到停顿的效果
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    snk.snakeMove();
                    // 调用移动方法

                    pack();
                    // 绘制默认大小的窗口
                }
            }
        }).start();

    }

}

class MyKeyEventPostProcessor implements KeyEventPostProcessor {

    private Snake snk;

    public boolean postProcessKeyEvent(KeyEvent e) {

        Direction dir = null;
        // 创建一个Direction枚举类引用
        switch (e.getKeyCode()) {
        case KeyEvent.VK_UP:
            dir = Direction.UP;
            break;
        case KeyEvent.VK_DOWN:
            dir = Direction.DOWN;
            break;
        case KeyEvent.VK_LEFT:
            dir = Direction.LEFT;
            break;
        case KeyEvent.VK_RIGHT:
            dir = Direction.RIGHT;
            break;
        }
        // 根据不同的方向键，将获取的值存放在dir中

        if (dir != null)
            snk.setMoveDir(dir);
        // 如果获取到的值是上下左右四个方向键中一个，那么将dir存放到Snake类的moveDir变量中
        return true;
    }

    public void setSnk(Snake snk) {
        this.snk = snk;
    }

}

class BoardComponent extends JComponent {

    public static final int Width = Configure.WIDTH;
    public static final int Height = Configure.HEIGTH;
    public static final int TileWidth = Configure.TILE_WIDTH;
    public static final int TileHeight = Configure.TILE_HEIGHT;
    public static final int Row = Configure.ROW;
    public static final int Column = Configure.COL;
    private static final int XOffset = (Width - Column * TileWidth) / 2;
    private static final int YOffset = (Height - Row * TileHeight) / 2;
    // 从Configure文件中读取的游戏数据

    private Snake snk;

    public void setSnake(Snake snk) {
        this.snk = snk;
    }

    /**
     * 我们覆盖了这个以用来打印
     * 
     * @param g
     */
    public void paintComponent(Graphics g) {
        drawDecoration(g);
        drawFill(g);
    }

    /**
     * 绘制实心的蛇身体以及食物
     * 
     * @param g
     */
    public void drawFill(Graphics g) {

        g.setColor(Color.GREEN);

        for (SnakePos sp : snk.getSnakeBody())
            g.fillRect(sp.col * TileWidth + XOffset, sp.row * TileHeight
                    + YOffset, TileWidth, TileHeight);
        // 遍历蛇的身体，将每一块蛇都上色
        Food fd = snk.getFood();

        g.setColor(Color.BLUE);

        // 将当前的食物上色
        g.fillRect(fd.col * TileWidth + XOffset, fd.row * TileHeight + YOffset,
                TileWidth, TileHeight);
    }

    /**
     * 绘制游戏的边界红色框
     * 
     * @param g
     */
    public void drawDecoration(Graphics g) {
        g.setColor(Color.RED);
        g.drawRect(XOffset, YOffset, Column * TileWidth, Row * TileHeight);
    }

    /**
     * 我们覆盖了这个方法来表示出这个类的组件的大小
     * 
     * @return 返回这个类的组件本身应该有多大
     */
    public Dimension getPreferredSize() {
        return new Dimension(Width, Height);
        // 返回一个Dimension对象，表示这个组件的大小
    }
}



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


/**
 * 
 * 
 * 
 *         configuration information
 */
public class Configure {

    public static final int WIDTH = 400;
    public static final int HEIGTH = 300;
    // Height and width of window.

    public static final int TILE_WIDTH = 16;
    public static final int TILE_HEIGHT = 16;
    // The height and width of each snakePos.

    public static final int ROW = 15;
    public static final int COL = 20;
    // The number of rows and columns of the game.

    public static final int INTERVAL = 300;
    // Snake moving time interval.
}


/**
 * 
 * 
 * 
 */
public enum Direction {
    UP, DOWN, LEFT, RIGHT;
    // 上下左右四个方向
}


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

import java.util.LinkedList;

/**
 * 
 * 
 * 
 *         食物类
 */
public class Food extends SnakePos {

    public int row;
    public int col;
    // 表示食物所在的行列

    public static final int Row = Configure.ROW;
    public static final int Column = Configure.COL;
    // 从Configure文件中读取的游戏行列

    Food() {
        randomPos();
        // 随机设置这个对象的行列变量
    }

    /**
     * 获取蛇的snakeBody链表，让食物与蛇身不重叠
     * 
     * @param snakeBody
     *            表示蛇身体的链表
     * @return 返回这个类实例化的对象本身
     */
    public Food getSnake(LinkedList<SnakePos> snakeBody) {
        while (checkSame(snakeBody))
            randomPos();
        // 如果发现食物的位置和蛇身体重叠，则重新随机食物的位置
        return this;
        // 返回这个对象本身，为创建实例时带来方便
    }

    /**
     * 检查蛇身体链表中是否有一块与当前食物坐标相同
     * 
     * @param snakeBody
     *            表示蛇身体的链表
     * @return 如果有重复返回true
     */
    private boolean checkSame(LinkedList<SnakePos> snakeBody) {
        for (SnakePos sp : snakeBody)
            if (sp.row == this.row && sp.col == this.col)
                return true;
        // 循环遍历是否有重复
        return false;
    }

    /**
     * 随机该对象的行和列变量
     */
    private void randomPos() {
        this.row = (int) (Math.random() * Row);
        this.col = (int) (Math.random() * Column);
    }
}
