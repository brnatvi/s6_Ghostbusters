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
        JFrame frame = vue.frame();
        

        while(!model.fin){
            
            model.getMessage(vue.answer);
            vue.afficheGrid(model.generateLaby());
        }
        

             
    }

}

