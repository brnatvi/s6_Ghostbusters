package client;

import java.net.*;
import javax.swing.*;

public class WaitUDP extends Thread {

    int port;
    DatagramSocket sock_udp;
    JTextArea answer;
    boolean fin = false;

    public WaitUDP(DatagramSocket _sock_udp){
        
        this.sock_udp = _sock_udp;
    }

    public void setTxt(JTextArea txt){
        this.answer = txt;
    }

    public void run(){
        try {
            
            byte[] data = new byte[218];
            while(!sock_udp.isClosed()){
                DatagramPacket paquet = new DatagramPacket(data, data.length);
                sock_udp.receive(paquet);
                String st = new String(paquet.getData(), 0, paquet.getLength()-3);
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
                System.out.println(st);
            }
            
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }

    
}