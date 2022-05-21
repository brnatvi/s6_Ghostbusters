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

    //TODO: close correctement
    BufferedReader in;
    PrintWriter out;
    Socket sockfd;
    Scanner sc = new Scanner(System.in);

    //UDP
    int port_udp;
    DatagramSocket sock_udp;
    MulticastSocket sock_mult;
    InetAddress address ;
    boolean fin = false;

    TreatTCP treat;
    SendTCP send;

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

    //Lire buffer
    public String read(BufferedReader in, char[] buf, int offset, int len){
        try{
            in.read(buf, offset, len);
        }
        catch(IOException e){
            e.printStackTrace();
        }
        return (new String(buf)); 
    }

    //LabyrintheVue l
    public void getMessage(JTextField txt){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
        // TreatTCP treat = new TreatTCP(sockfd);
        // treat.treatMess(msg);
        treat = new TreatTCP(msg);
        treat.setText(txt);
        treat.start();
        try {
            treat.join();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        if(msg.substring(0,5).equals("GOBYE") || msg.substring(0,5).equals("ENDGA")){
            try {
                fin = true;
                in.close();
                out.close();
                sock_udp.close();
                sock_mult.leaveGroup(InetAddress.getByName(treat.getIP()));
                sockfd.close();

            }
            catch(Exception e){
                e.printStackTrace();
            }
        }
        if(msg.substring(0,5).equals("WELCO")){
            try{
                               
                sock_mult = new MulticastSocket(treat.port_mult);
                WaitMultiCast mc = new WaitMultiCast(sock_mult, treat.getIP());
                mc.setText(txt);
                mc.start();
                
            }
            catch(Exception e){
                e.printStackTrace();
            }

        }
    }
  
    //Partie send
    // , LabyrintheVue l
    public void sendMessage(JTextField txt){
        String msg = sc.nextLine();
        // System.out.println(msg);
        send = new SendTCP(msg, out);
        send.start();
        try {
            send.join();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        if(msg.substring(0,5).equals("NEWPL") || msg.substring(0,5).equals("REGIS")){ 
            try {
                port_udp = send.getPortUDP();
                sock_udp = new DatagramSocket(port_udp);
                WaitUDP wu = new WaitUDP(sock_udp);
                wu.setTxt(txt);
                wu.start();
                
            }
            catch(Exception e){
                e.printStackTrace();
            }
        }
    }
    //TODO: vérifier que pseudo est bien 8 caractères
 
    // VRAI TEST
    public static void main(String[] args){       
        Client p = new Client(args[0], args[1]);
        LabyrintheVue vue = new LabyrintheVue(100, 100);
        LabyrintheController ctrl = new LabyrintheController(vue, p.in, p.out, p.sockfd);
        ctrl.ctrlFunctions();
        vue.setCtrl(ctrl);
        vue.frame();
        while(!p.fin){
            p.getMessage(vue.answer);
            
        }
        

             
    }

}

