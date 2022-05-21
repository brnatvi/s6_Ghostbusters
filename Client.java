import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;
import java.util.concurrent.*;
import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.nio.charset.*;
import java.lang.Object;

class Client {

    BufferedReader in;
    PrintWriter out;
    Socket sockfd;
   

    //Connexion TCP du client au serveur
    public Client(String adresse, String port){ 
        try {
            
            sockfd = new Socket(adresse, Integer.parseInt(port));
            in = new BufferedReader(new InputStreamReader(sockfd.getInputStream()));
            out = new PrintWriter(new OutputStreamWriter(sockfd.getOutputStream()));
            
        }

        catch(Exception e){
            
            e.printStackTrace();
        }
    }

 
    // VRAI TEST
    public static void main(String[] args){       
        Client p = new Client(args[0], args[1]);
        LabyrintheVue vue = new LabyrintheVue(100, 100);
        LabyrintheModel model = new LabyrintheModel(p.in, p.out, p.sockfd);
        LabyrintheController ctrl = new LabyrintheController(vue, model);
        
        ctrl.ctrlFunctions();
        vue.setCtrl(ctrl);
        vue.functions();
        
        JPanel pane = new JPanel();
        GridLayout g = new GridLayout(2,0);
        pane.setLayout(g);
        JFrame frame = new JFrame("GHOST LAB");
        frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
        pane.add(vue);
        frame.setContentPane(pane);
    
        while(!model.fin){
            
            model.getMessage(vue.answer);
            
            if(ctrl.generateLaby() != null){
                
                pane.removeAll();
                pane.add(vue);
                pane.add(ctrl.generateLaby());
                pane.updateUI();
                ctrl.getPosition();
            }
 
            
        }
        

             
    }

}

