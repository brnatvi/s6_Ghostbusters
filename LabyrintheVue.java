import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class LabyrintheVue extends JPanel{

    private int hFrame, wFrame; 
    JTextField answer = new JTextField();
    JTextField clientAnswer = new JTextField();
    JLabel id_label = new JLabel("Identifiant");
    JTextField id = new JTextField();
    JLabel mess_label = new JLabel("Message");
    JTextField mess = new JTextField();
    JLabel nb_partie_label = new JLabel("N° de partie");
    JTextField nb_partie = new JTextField();
    JLabel dist_label = new JLabel("Distance");
    JTextField dist = new JTextField();
    JLabel port_udp_label = new JLabel("Port UDP");
    JTextField port_udp = new JTextField();
    final JButton send = new JButton("Send");
    String[] entete = {"--Entete--","NEWPL","REGIS","SIZE?","LIST?", "GAME?"};
    JComboBox<String> jComboBox;

    LabyrintheController ctrl;





    public LabyrintheVue(int hf, int wf){
        super();
        setSize(wf,hf);
        jComboBox = new JComboBox<>(entete);
        this.add(jComboBox);
    }

    public void setCtrl(LabyrintheController _ctrl){
        this.ctrl = _ctrl;
    }

    public void grid(int widthLaby, int heightLaby){
        MyButton[][] buttons = new MyButton[widthLaby][heightLaby];
        GridLayout g = (new GridLayout(widthLaby, heightLaby));
        this.setLayout(g);
        for (int i = 0; i < widthLaby; i++) {
            for (int j = 0; j < heightLaby; j++) {
                buttons[i][j] = new MyButton("("+i + "," + j+")");

                this.add(buttons[i][j]);
            }
        }
        
    }

    public void answerField(){
        JLabel label = new JLabel("Réponse serveur");
        
        answer.setEnabled(false);
        this.add(label);
        answer.setText("----");
        answer.setPreferredSize(new Dimension(900,50));
        answer.setForeground(Color.BLACK);
        this.add(answer);

        JLabel label1 = new JLabel("Message client");
        clientAnswer.setEnabled(false);
        clientAnswer.setPreferredSize(new Dimension(900,50));
        clientAnswer.setForeground(Color.BLACK);
        this.add(clientAnswer);
        
    }

    public void addTextField(JTextField j){
        this.add(j);
    }

    public void addLabel(JLabel l){
        this.add(l);
    }

    public void removeTextField(JTextField j){
        this.remove(j);
    }

    public void removeLabel(JLabel l){
        this.remove(l);
    }

    public void clientAnswer(){

        id.setPreferredSize(new Dimension(900,50));
        mess.setPreferredSize(new Dimension(900,50));
        nb_partie.setPreferredSize(new Dimension(900,50));
        dist.setPreferredSize(new Dimension(900,50));
        port_udp.setPreferredSize(new Dimension(900,50));
    }

    public void initializeText(){
        id.setText("");
        mess.setText("");
        nb_partie.setText("");
        dist.setText("");
        port_udp.setText("");
    }

    public String getMessageToSend(){
        String toSend = "";
        String entete = (String)jComboBox.getSelectedItem();
        String space = " ";
        String tcpEnd = "***";
        toSend += entete;
        toSend += space;
        switch(entete) {
            case "NEWPL":
                toSend += id.getText();
                toSend += space;
                toSend += port_udp.getText();
                break;
            case "REGIS":
                toSend += id.getText();
                toSend += space;
                toSend += port_udp.getText();
                toSend += space;
                toSend += nb_partie.getText();
                break;
            case "UNREG":
            case "SIZE?":
            case "LIST?":
                toSend += nb_partie.getText();
                break;
            case "GAME?":
                break;
            
        }
        toSend += tcpEnd;
        return toSend;
    }

    
    // public void frame
    public void frame(){
        JFrame frame = new JFrame("GHOST LAB");
        answerField();
        
        clientAnswer();
        
        frame.add(this);
        frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
        frame.setContentPane(this);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        frame.pack();
        frame.setVisible(true);
    }

}