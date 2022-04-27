import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class SendTCP {

    // n, m, s
    public static byte[] sendOneInfoByte(int info){
        byte[] nb = new byte[1];
        nb[0] =  (byte)((info >> 0) & 0xff);
        return nb;
    }

    // id, port
    public static String sendString(byte[] id, int len){
        byte[] id_len = new byte[len];
        System.arraycopy(id, 0, id_len, 0, len);
        return new String(id_len);
    }

    public static byte[] sendMess(String msg){
        String str_entete = msg.substring(0,5);
        byte[] entete = str_entete.getBytes();
        byte[] space = new byte[1]; space[0] = 42;
        byte[] tcpEnd = (msg.substring(msg.length()-3)).getBytes();
        byte[] toSend = null;
        switch(str_entete){
            case "NEWPL" :
            case "GAME?":
                toSend = msg.getBytes();
                break;
            case "REGIS" :
                byte[] entete_id_port = msg.substring(0,20).getBytes();
                byte[] m = sendOneInfoByte(Integer.parseInt(msg.substring(msg.length()-3-1, msg.length()-3)));
                toSend = new byte[entete_id_port.length+m.length+tcpEnd.length];
                System.arraycopy(entete_id_port, 0, toSend, 0, entete_id_port.length);
                System.arraycopy(m, 0, toSend, entete_id_port.length, 1);
                System.arraycopy(tcpEnd, 0, toSend, entete_id_port.length+1, 3);
                
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
            
        }
        return toSend;
    }

}