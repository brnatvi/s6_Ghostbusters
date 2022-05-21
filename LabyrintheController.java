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
    BufferedReader in;
    PrintWriter out; 
    
    

        //Variables importantes
    int portUDP;
    DatagramSocket sockUDP;
    MulticastSocket sockMult;
    Socket sockfd;

    public LabyrintheController(LabyrintheVue _vue, BufferedReader _in, PrintWriter _out, Socket _sockfd){
        this.vue = _vue;
        this.in = _in;
        this.out = _out;
        this.sockfd = _sockfd;
    }

    public void ctrlFunctions(){
        clientEntete();
        sendButton();
    }

    public void clientEntete(){
        
        vue.jComboBox.setBounds(80,50,140,20);
        vue.jComboBox.addActionListener(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e){
                JComboBox box = (JComboBox) e.getSource();
                String s = (String)box.getSelectedItem();
                switch(s){
                    case "NEWPL":
                        System.out.println(s);
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
                }
            }
        });
        
        vue.add(vue.jComboBox);
        
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
        SendTCP send = new SendTCP(msg, out);
        send.start();
        try {
            send.join();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        if(msg.substring(0,5).equals("NEWPL") || msg.substring(0,5).equals("REGIS")){ 
            try {
                portUDP = send.getPortUDP();
                sockUDP = new DatagramSocket(portUDP);
                WaitUDP wu = new WaitUDP(sockUDP);
                
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
        }
        return canSend;
    }

}
