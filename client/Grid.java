package client;

import javax.swing.*;
import java.awt.*;

public class Grid extends Panel{

    int height;
    int width;
    MyButton[][] buttons; 

    public Grid() {
        super();
    }

    public Grid(int _height, int _width){
        super();
        
        setSize(800, 800);
        this.height = _height;
        this.width = _width;
        buttons = new MyButton[width][height];
    }
    
    public void grid(){
        GridLayout g = (new GridLayout(width, height));
        this.setLayout(g);
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                buttons[i][j] = new MyButton("("+i + "," + j+")");

                this.add(buttons[i][j]);
            }
        }
        
    }

    public static void main(String[] args){
        JFrame frame = new JFrame("GHOST LAB");
        Grid grid = new Grid(10, 10);
        grid.grid();
        frame.add(grid);
        frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
        frame.setContentPane(grid);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        frame.pack();
        frame.setVisible(true);
    }

}