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

