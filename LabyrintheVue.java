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

    JPanel p1 = new JPanel();
    GridLayout g1 = new GridLayout(2,2);

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
    String[] entete = {"--Entete--","NEWPL","REGIS","SIZE?","LIST?", "GAME?", "START", "UPMOV", "DOMOV", "LEMOV", "RIMOV", "GLIS?", "MALL?", "SEND?", "IQUIT"};
    JComboBox<String> jComboBox;
    Grid laby;

    LabyrintheController ctrl;

    public LabyrintheVue(int hf, int wf){
        super();
        p1.setLayout(g1);
        setSize(wf,hf);
        jComboBox = new JComboBox<>(entete);
        jComboBox.setBounds(80,50,140,20);
        this.add(jComboBox);
    }

    public void setCtrl(LabyrintheController _ctrl){
        this.ctrl = _ctrl;
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
        
        switch(entete) {
            case "NEWPL":
                toSend += space;
                toSend += id.getText();
                toSend += space;
                toSend += port_udp.getText();
                break;
            case "REGIS":
                toSend += space;
                toSend += id.getText();
                toSend += space;
                toSend += port_udp.getText();
                toSend += space;
                toSend += nb_partie.getText();
                break;
            case "UNREG":
            case "SIZE?":
            case "LIST?":
                toSend += space;
                toSend += nb_partie.getText();
                break;
            case "GAME?":    
            case "START":
            case "GLIS?":
            case "IQUIT":
                break;
            case "UPMOV":
            case "LEMOV":
            case "RIMOV":
            case "DOMOV":
                toSend += space;
                toSend += dist.getText();
                break;
            case "MALL?":
                toSend += space;
                toSend += mess.getText();
                break;
            case "SEND?":
                toSend += space;
                toSend += id.getText();
                toSend += space;
                toSend += mess.getText();
            
        }
        toSend += tcpEnd;
        return toSend;
    }

    
    // public void frame
    public JFrame frame(){
        JFrame frame = new JFrame("GHOST LAB");
        //Grid laby = new Grid(10, 10);
        //laby.grid();
        answerField();
        clientAnswer();
        p1.setLayout(g1);
        // p1.add(laby);
        p1.add(this);
        // frame.add(laby);
        // frame.add(this);
        frame.add(p1);
        
        frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
        
        //frame.setContentPane(this);
        frame.setContentPane(p1);
        
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        frame.pack();
        frame.setVisible(true);
        return frame;
    }

}