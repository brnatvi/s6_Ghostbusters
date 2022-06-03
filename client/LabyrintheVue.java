package client;

import javax.swing.*;
import java.awt.*;

public class LabyrintheVue extends JPanel{

    private int hFrame, wFrame; 
    JScrollBar scrollBar; 
    JTextArea answer = new JTextArea("");
    JTextArea clientAnswer = new JTextArea();
    JLabel id_label = new JLabel("Identifiant", SwingConstants.RIGHT);
    JTextField id = new JTextField();
    JLabel mess_label = new JLabel("Message", SwingConstants.RIGHT);
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

    LabyrintheController ctrl;

    public LabyrintheVue(int hf, int wf){
        super();
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
        answer.setEditable(false);
        this.add(label);
        answer.setText("----");
        answer.setPreferredSize(new Dimension(900,50));
        JScrollPane scroll = new JScrollPane(answer);
        scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
        this.add(scroll);

        JPanel panel = new JPanel();
        JLabel label1 = new JLabel("Message client");
        clientAnswer.setEditable(false);
        clientAnswer.setPreferredSize(new Dimension(900,50));        
        panel.add(label1);
        panel.add(clientAnswer);
        this.add(panel);
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
        id_label.setPreferredSize(new Dimension(500,25));
        id.setPreferredSize(new Dimension(500,25));
        mess_label.setPreferredSize(new Dimension(500,25));
        mess.setPreferredSize(new Dimension(500,25));
        nb_partie.setPreferredSize(new Dimension(100,25));
        dist.setPreferredSize(new Dimension(25,25));
        port_udp.setPreferredSize(new Dimension(250,25));
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

    public void functions(){
        answerField();
        clientAnswer();
    }
    
}