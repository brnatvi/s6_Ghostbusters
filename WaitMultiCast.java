import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;
import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;

public class WaitMultiCast extends Thread {

    MulticastSocket sock_mult;
    String ip;
    JTextField answer;

    public WaitMultiCast(MulticastSocket _sock_mult, String _ip){
        this.sock_mult = _sock_mult;
        this.ip = _ip;
    }

    public void setText(JTextField _answer){
        this.answer = _answer;
    }

    public void run(){
        try {
            // MulticastSocket sock_mult = new MulticastSocket(port);
            
            sock_mult.joinGroup(InetAddress.getByName(ip));
            byte[] data = new byte[218];
            while(true){
                DatagramPacket paquet = new DatagramPacket(data, data.length);
                sock_mult.receive(paquet);
                
                try {
                    TreatPacket tp = new TreatPacket(new String(paquet.getData(), 0, paquet.getLength()));
                    tp.setTxt(answer);
                    tp.start();
                    try {
                        tp.join();
                    }
                    catch(Exception e){
                        e.printStackTrace();
                    }
                }
                catch(Exception e){
                    e.printStackTrace();
                }
                // String st = new String(paquet.getData(), 0, paquet.getLength()-3);
                // System.out.println(st);
            }
            
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }
}