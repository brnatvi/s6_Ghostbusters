package client;

import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class SendTCP extends Thread {

    private int port_udp;
    public String msg;
    public PrintWriter out;
    
    boolean fin = false;

    public SendTCP(String _msg, PrintWriter _out){
        this.msg = _msg;
        this.out = _out;
        
    }

    public int getPortUDP(){
        return port_udp;
    }
    
    // n, m, s
    public byte[] sendOneInfoByte(int info){
        byte[] nb = new byte[1];
        nb[0] =  (byte)((info >> 0) & 0xff);
        return nb;
    }


    public char[] sendCharArray(String val, int length){
        char[] res = new char[length];
        int ind = length-1;
        for(int i = val.length()-1; i >= 0; i--){
            res[ind] = val.charAt(i);
            ind--;
        }
        for(int j = ind ; j >= 0; j--){
            res[ind] = '0';
            ind--;
        }
        return res;
    }

    public byte[] charArrayToByteArray(char[] tab){
        byte[] res = new byte[tab.length];
        for(int i = 0; i<tab.length; i++){
            res[i] = (byte) tab[i];
        }
        return res;
    }

    // id, port
    public String sendString(byte[] id, int len){
        byte[] id_len = new byte[len];
        System.arraycopy(id, 0, id_len, 0, len);
        return new String(id_len);
    }


    public void run(){
        String str_entete = msg.substring(0,5);
        byte[] entete = str_entete.getBytes();
        byte[] space = new byte[1]; space[0] = 42;
        byte[] tcpEnd = (msg.substring(msg.length()-3)).getBytes();
        byte[] toSend = null;
        switch(str_entete){
            case "GAME?":
            case "START":
            case "GLIS?":
            case "IQUIT":
            case "MALL?":
            case "SEND?":
                toSend = msg.getBytes();
                break;
            case "NEWPL":
                toSend = msg.getBytes();
                port_udp = Integer.parseInt(msg.substring(15,19));
                break;
            case "REGIS" :
                byte[] entete_id_port = msg.substring(0,20).getBytes();
                byte[] m = sendOneInfoByte(Integer.parseInt(msg.substring(msg.length()-3-1, msg.length()-3)));
                toSend = new byte[entete_id_port.length+m.length+tcpEnd.length];
                System.arraycopy(entete_id_port, 0, toSend, 0, entete_id_port.length);
                System.arraycopy(m, 0, toSend, entete_id_port.length, 1);
                System.arraycopy(tcpEnd, 0, toSend, entete_id_port.length+1, 3);
                port_udp = Integer.parseInt(msg.substring(15,19));
                break;
            case "UNREG" :
            case "SIZE?" :
            case "LIST?" : 
                m = sendOneInfoByte(Integer.parseInt(msg.substring(6, msg.length()-3)));
                toSend = new byte[entete.length + space.length + 1 + tcpEnd.length];
                System.arraycopy(entete, 0 ,toSend, 0, entete.length);
                System.arraycopy(space, 0, toSend, entete.length, 1);
                System.arraycopy(m, 0, toSend, entete.length+1, 1);
                System.arraycopy(tcpEnd, 0, toSend, entete.length+2, 3);
                break;
            case "UPMOV":
            case "DOMOV":
            case "LEMOV":
            case "RIMOV":
                String d = msg.substring(6, msg.length()-3);  
                byte[] d_byte = charArrayToByteArray(sendCharArray(d,3)); 
                toSend = new byte[entete.length + space.length + d_byte.length+ tcpEnd.length];
                System.arraycopy(entete, 0, toSend, 0, entete.length);
                System.arraycopy(space, 0, toSend, entete.length, 1);
                System.arraycopy(d_byte, 0, toSend, entete.length+1, d_byte.length);
                System.arraycopy(tcpEnd, 0, toSend, entete.length+1+d_byte.length, 3);
                
                break;

            
        }
        out.print(new String(toSend));        
        out.flush();
        
    }

    


}