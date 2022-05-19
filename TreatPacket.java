import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class TreatPacket extends Thread {

    public String msg;

    public TreatPacket(String _msg){
        this.msg = _msg;
    }

    public byte[] conditionMet(byte[]tail, int n1){
        if(tail[n1] == 43 && tail[n1+1] == 43 && tail[n1+2] == 43){
            
            return (Arrays.copyOfRange(tail, 0, n1+3));
        }
        return null;
    }

    //parser jusqu'à trouver '+++' return byte array
    public byte[] parseUntilPlus(String entete, byte[]tail){
        if(entete.equals("GHOST")){
            return conditionMet(tail, 8);
        }
        else if(entete.equals("ENDGA")){ // ENDGA tharsiya 1000+++
            
            return conditionMet(tail, 14); 
        }
        else if(entete.equals("SCORE")){ // SCORE tharsiya 1000 001 001+++
            return conditionMet(tail, 22);
        }
        else if(entete.equals("MESSA") || entete.equals("MESSP")){
            int n = 6;
            byte[] parsed = conditionMet(tail, n);
            while(parsed == null && n <=215){
                n++;
                
                parsed = conditionMet(tail,n);
            }
            return parsed;
        }
        return null;
    }

    public void run(){
        String entete = msg.substring(0,5);
        byte[] tail = parseUntilPlus(entete, (msg.substring(5,msg.length())).getBytes());
        switch(entete){
            // case "GHOST":
            case "SCORE":
            case "MESSA":
            case "ENDGA":
                
                System.out.println(entete+new String(tail));
                break;
            
        }
        
        
    }



}