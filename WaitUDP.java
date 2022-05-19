import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class WaitUDP extends Thread {

    int port;
    DatagramSocket sock_udp;

    public WaitUDP(DatagramSocket _sock_udp){
        
        this.sock_udp = _sock_udp;
    }

    public void run(){
        try {
            // DatagramSocket sock_udp = new DatagramSocket(port);
            byte[] data = new byte[218];
            while(true){
                DatagramPacket paquet = new DatagramPacket(data, data.length);
                sock_udp.receive(paquet);
                String st = new String(paquet.getData(), 0, paquet.getLength()-3);
                try {
                    TreatPacket tp = new TreatPacket(new String(paquet.getData(), 0, paquet.getLength()));
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

    public static void main(String[] args){
        // DatagramSocket s = new DatagramSocket(9898);
        // s.close();
    }
}