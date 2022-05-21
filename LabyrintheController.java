import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class LabyrintheController {

    LabyrintheVue vue;
    Grid laby;
    LabyrintheModel model;
    

    boolean start = false;

    public LabyrintheController(LabyrintheVue _vue, LabyrintheModel _model){
        this.vue = _vue;
        this.model = _model;
        laby = new Grid();
        
    }

    public void setGrid(Grid _laby){
        this.laby = _laby;
    }

    public void ctrlFunctions(){
        clientAvantStart();
        clientAfterStart();
        sendButton();
    }

    public void clientAvantStart(){
        vue.jComboBox.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e){
                JComboBox box = (JComboBox) e.getSource();
                String s = (String)box.getSelectedItem();
                switch(s){
                    case "NEWPL":
                        
                        vue.add(vue.id_label);
                        vue.add(vue.id);
                        vue.add(vue.port_udp_label);
                        vue.add(vue.port_udp);
                        vue.remove(vue.dist_label);
                        vue.remove(vue.dist);
                        vue.remove(vue.mess_label);
                        vue.remove(vue.mess);
                        vue.remove(vue.nb_partie_label);
                        vue.remove(vue.nb_partie);
                        
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "REGIS":
                        vue.addLabel(vue.id_label);
                        vue.addTextField(vue.id);
                        vue.addLabel(vue.port_udp_label);
                        vue.addTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.removeLabel(vue.mess_label);
                        vue.removeTextField(vue.mess);
                        vue.addLabel(vue.nb_partie_label);
                        vue.addTextField(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "SIZE?":
                    case "LIST?":
                        vue.removeLabel(vue.id_label);
                        vue.removeTextField(vue.id);
                        vue.removeLabel(vue.port_udp_label);
                        vue.removeTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.removeLabel(vue.mess_label);
                        vue.removeTextField(vue.mess);
                        vue.addLabel(vue.nb_partie_label);
                        vue.addTextField(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "GAME?":
                        vue.removeLabel(vue.id_label);
                        vue.removeTextField(vue.id);
                        vue.removeLabel(vue.port_udp_label);
                        vue.removeTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.removeLabel(vue.mess_label);
                        vue.removeTextField(vue.mess);
                        vue.removeLabel(vue.nb_partie_label);
                        vue.removeTextField(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "START":
                        vue.removeLabel(vue.id_label);
                        vue.removeTextField(vue.id);
                        vue.removeLabel(vue.port_udp_label);
                        vue.removeTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.removeLabel(vue.mess_label);
                        vue.removeTextField(vue.mess);
                        vue.removeLabel(vue.nb_partie_label);
                        vue.removeTextField(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        start = true;
                        break;
                }
            }
        });
        
        // vue.add(vue.jComboBox);
        
    }

    public void clientAfterStart(){        
        vue.jComboBox.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e){
                JComboBox box = (JComboBox) e.getSource();
                String s = (String)box.getSelectedItem();
                switch(s){
                    case "UPMOV":
                    case "DOMOV":
                    case "LEMOV":
                    case "RIMOV":
                        vue.remove(vue.id_label);
                        vue.remove(vue.id);
                        vue.remove(vue.port_udp_label);
                        vue.remove(vue.port_udp);
                        vue.add(vue.dist_label);
                        vue.add(vue.dist);
                        vue.remove(vue.mess_label);
                        vue.remove(vue.mess);
                        vue.remove(vue.nb_partie_label);
                        vue.remove(vue.nb_partie);
                        vue.add(vue.send);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "MALL?":
                        vue.remove(vue.id_label);
                        vue.remove(vue.id);
                        vue.remove(vue.port_udp_label);
                        vue.remove(vue.port_udp);
                        vue.remove(vue.dist_label);
                        vue.remove(vue.dist);
                        vue.add(vue.mess_label);
                        vue.add(vue.mess);
                        vue.remove(vue.nb_partie_label);
                        vue.remove(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "SEND?":
                        vue.add(vue.id_label);
                        vue.add(vue.id);
                        vue.removeLabel(vue.port_udp_label);
                        vue.removeTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.add(vue.mess_label);
                        vue.add(vue.mess);
                        vue.remove(vue.nb_partie_label);
                        vue.remove(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    case "GLIS?":
                    case "IQUIT":
                        vue.removeLabel(vue.id_label);
                        vue.removeTextField(vue.id);
                        vue.removeLabel(vue.port_udp_label);
                        vue.removeTextField(vue.port_udp);
                        vue.removeLabel(vue.dist_label);
                        vue.removeTextField(vue.dist);
                        vue.removeLabel(vue.mess_label);
                        vue.removeTextField(vue.mess);
                        vue.removeLabel(vue.nb_partie_label);
                        vue.removeTextField(vue.nb_partie);
                        vue.add(vue.send);
                        vue.updateUI();
                        break;
                    
                }
            }
        });
        
    }

    public void sendButton(){
        vue.send.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e){
                
                String toSend = vue.getMessageToSend();
                boolean ready = contraintes(toSend.substring(0,5));
                if(ready){
                    sendMessage(toSend);
                    vue.initializeText();
                    
                }
                
            }
        });
    }


    public void sendMessage(String msg){ 
        System.out.println(msg); 
        vue.clientAnswer.setText(msg);      
        SendTCP send = new SendTCP(msg, model.out);
        send.start();
        try {
            send.join();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        if(msg.substring(0,5).equals("NEWPL") || msg.substring(0,5).equals("REGIS")){ 
            try {
                model.portUDP = send.getPortUDP();
                model.sockUDP = new DatagramSocket(model.portUDP);
                WaitUDP wu = new WaitUDP(model.sockUDP);
                wu.setTxt(vue.answer);
                wu.start();
                
            }
            catch(Exception e){
                e.printStackTrace();
            }
        }
    }

    public boolean isAlphaNumeric(String s){
        return s != null && s.matches("^[a-zA-Z0-9]*$");
    }

    public boolean isNumeric(String s){
        return s != null && s.matches("^[0-9]*$");
    }

    public boolean checkId(String s){
        return ((s.length() == 8) && isAlphaNumeric(s));
    }

    public boolean checkPortUDP(String s){
        return ((s.length() == 4) && isNumeric(s));
    }

    public boolean checkNbPartie(String s){
        return (isNumeric(s) && Integer.parseInt(s) <= 255);
    }

    public boolean checkDist(String s){
        return (isNumeric(s) && Integer.parseInt(s) <= 999);
    }

    public boolean checkMess(String s){
        return s.length() <= 200 ;
    }

    public boolean contraintes(String entete){
        boolean canSend = false;
        switch(entete){
            case "NEWPL":
                canSend = checkId(vue.id.getText()) && checkPortUDP(vue.port_udp.getText());
                break;
            case "REGIS":
                canSend = checkId(vue.id.getText()) && checkPortUDP(vue.port_udp.getText()) && checkNbPartie(vue.nb_partie.getText());
                break;
            case "SIZE?":
            case "UNREG":
            case "LIST?":
                canSend = checkNbPartie(vue.nb_partie.getText());
                break; 
            case "START":
            case "GAME?":
            case "GLIS?":
            case "IQUIT":
                canSend = true;
                break;
            case "UPMOV":
            case "DOMOV":
            case "LEMOV":
            case "RIMOV":
                canSend = checkDist(vue.dist.getText());
                break;
            case "MALL?":
                canSend = checkMess(vue.mess.getText());
                break;
            case "SEND?":
                canSend = checkId(vue.id.getText()) && checkMess(vue.mess.getText());
                break;
        }
        return canSend;
    }

    public void initColor(){
        if(laby != null && laby.buttons != null){
            for(MyButton[] b : laby.buttons){
                for(MyButton bb: b){
                    bb.setBackground(Color.WHITE);
                }
            }
        }
    }

    public void getPosition(){
        initColor();
        if(laby != null && vue.answer.getText().length() > 5 && 
        (vue.answer.getText().substring(0,5).equals("WELCO") || vue.answer.getText().substring(0,5).equals("MOVE!") || 
        vue.answer.getText().substring(0,5).equals("MOVEF"))
        ){
            int x = model.getX(); //vue.answer.getText().substring(15,18);
            int y = model.getY(); //vue.answer.getText().substring(19,22);
            laby.buttons[x][y].setBackground(Color.RED);
        }
    }

    public Grid generateLaby(){
        if(vue.answer.getText().length() > 5 && vue.answer.getText().substring(0,5).equals("SIZE!")){
            laby = model.generateLaby();
        }
        return laby;
    }


}
