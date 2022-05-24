import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;

public class MyButton extends JButton{

    public MyButton(){
        super();
        changeColor();
    }

    public MyButton(String s){
        super(s);
        changeColor();
    }

    public void changeColor(){
        this.addMouseListener(new MouseAdapter(){
            @Override
            public void mousePressed(MouseEvent event){
                // left click
                if(event.getButton() == MouseEvent.BUTTON1){
                    
                    
                }
                // right click
                else if(event.getButton() == MouseEvent.BUTTON3){
                    
                    setBackground(Color.BLACK);
                    setForeground(Color.WHITE);
                }
            }
        });
    }
}