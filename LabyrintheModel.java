import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class LabyrintheModel {

    BufferedReader in;
    PrintWriter out; 
    
    int portUDP;
    DatagramSocket sockUDP;
    MulticastSocket sockMult;
    Socket sockfd;
    TreatTCP treat;
    WaitUDP wu;
    WaitMultiCast mc;

    boolean start = false;
    boolean fin = false;
    String ip = "";

    public LabyrintheModel(BufferedReader _in, PrintWriter _out, Socket _sockfd){
        this.in = _in;
        this.out = _out;
        this.sockfd = _sockfd;
    }

    public String read(BufferedReader in, char[] buf, int offset, int len){
        try{
            in.read(buf, offset, len);
        }
        catch(IOException e){
            e.printStackTrace();
        }
        return (new String(buf)); 
    }

    public void getMessage(JTextArea txt){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
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
                sockUDP.close();
                sockMult.leaveGroup(InetAddress.getByName(ip));
                sockfd.close();
                wu.fin = false;
                mc.fin = false;
            }
            catch(Exception e){
                e.printStackTrace();
            }
        }
        if(msg.substring(0,5).equals("WELCO")){
            try{
                
                sockMult = new MulticastSocket(treat.port_mult);
                ip = treat.getIP();
                mc = new WaitMultiCast(sockMult, ip);
                mc.setText(txt);
                mc.start();
                
            }
            catch(Exception e){
                e.printStackTrace();
            }

        }
        
    }

    public Grid generateLaby(){
        if(treat.height != 0 && treat.width != 0){
            Grid laby = new Grid(treat.height,treat.width);
            laby.grid();
            return laby;
        }
        return null;
    }

    public int getX(){
        return treat.x;
    }

    public int getY(){
        return treat.y;
    }


}