import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

public class TreatTCP {

    public Socket sockfd;

    public TreatTCP(Socket _sockfd){
        this.sockfd = _sockfd;
    }

    public byte[] conditionMet(byte[]tail, int n1){
        if(tail[n1] == 42 && tail[n1+1] == 42 && tail[n1+2] == 42){
            return (Arrays.copyOfRange(tail, 0, n1+3)); //n-1
        }
        return null;
    }

    //parser jusqu'à trouver '***' return byte array
    public byte[] parseUntilStars(String entete, byte[]tail){
        if(entete.equals("GAMES") || entete.equals("REGOK") || entete.equals("UNROK") || entete.equals("GLIS!")){
            return conditionMet(tail, 2); 
        }
        else if(entete.equals("REGNO") || entete.equals("NMEMB") || entete.equals("MALL!") || entete.equals("SEND!") || entete.equals("GOBYE")){
            return conditionMet(tail, 0);
        }
        //full message
        else if(entete.equals("OGAME")){
            return conditionMet(tail, 9);
        }
        else if(entete.equals("LIST!")){
            return conditionMet(tail, 4); 
        }
        else if(entete.equals("SIZE!")){ 
            return conditionMet(tail, 8);
        }
        else if(entete.equals("PLAYR")){
            return conditionMet(tail, 14);
        }
        else if(entete.equals("WELCO")){

            return conditionMet(tail, 31); 

        }
        else if(entete.equals("POSIT")){
            return conditionMet(tail, 22);
        }
        else if(entete.equals("MOVE!")){
            
            return conditionMet(tail,8);
        }
        else if(entete.equals("MOVEF")){
            return conditionMet(tail, 13);
        }
        else if(entete.equals("GPLYR")){ 

            return conditionMet(tail, 27);
        }
        
        return null;
    }

    //return byte double array
    public byte[][] parseUntilStars(String entete, byte[]tail, int nb, int len){
        byte[][] lst = new byte[nb][];
        for(int i = 0; i<lst.length; i++){
            lst[i] = parseUntilStars(entete, Arrays.copyOfRange(tail, i*len, tail.length));
        }
        return lst;
    }

    // n, m et s 
    public String treatInfoOneByte(byte info){
        return (String.valueOf((int) info));
    }

    public String treatInfoTwoBytes(byte[] info){
        return (String.valueOf(
            ((info[1] & 0xff) << 8) + (info[0] & 0xff)
            ));
    }

    public void treatMess(String msg){ // enlever tcpEnd from system.out.println
        String entete = msg.substring(0,5);
        byte[] tail = parseUntilStars(entete, (msg.substring(5)).getBytes());
        String tcpEnd = "***";
        int n = 0;
        switch(entete){
            case "REGNO":
            case "DUNNO":
            case "NMEMB":
            case "GOBYE":
            case "MOVE!":
            case "MOVEF":
            case "MALL!": 
            case "SEND!":
            case "NSEND":
                System.out.println(entete+new String(tail));
                break;
            case "REGOK":
            case "UNROK":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                break;
            case "SIZE!":
                // for(byte b: Arrays.copyOfRange(tail, 3, 5)) {
                //     System.out.print(b+" ");
                // }
                // System.out.println();
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+ treatInfoTwoBytes(Arrays.copyOfRange(tail, 3, 5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail, 6, 8)) +tcpEnd);
                break;
            case "GAMES":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                n = tail[1];
                if(n != 0){
                    byte[][]l = parseUntilStars("OGAME", msg.substring(tail.length+5).getBytes(), n, 12);
                    for(byte[] k: l){
                        System.out.println("OGAME "+ treatInfoOneByte(k[6])+" "+treatInfoOneByte(k[8])+tcpEnd);
                    }
                }
                break;
            case "LIST!":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoOneByte(tail[3])+tcpEnd); //tail[1] et tail[3]
                n = tail[3];
                if(n != 0){
                    byte[][]tmp = parseUntilStars("PLAYR", msg.substring(tail.length+5).getBytes(), n, 17);
                    for(byte[] t: tmp){
                        System.out.println(new String(t));
                    }
                }  
                break;
            case "WELCO": // à changer d'emplacement
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoTwoBytes(Arrays.copyOfRange(tail,3,5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail,6,8))+" "+treatInfoOneByte(tail[9])+" "+
                new String(Arrays.copyOfRange(tail, 11, 34)));
                String posit = (msg.substring(tail.length+5));
                
                byte[] tmp = parseUntilStars("POSIT", msg.substring(tail.length+5).getBytes());
                System.out.println(new String(tmp));
                break; 
            case "GLIS!":
                System.out.println(entete+" "+treatInfoOneByte(tail[1]) +tcpEnd);
                n = tail[1];
                if(n != 0){
                    /*
                    GPLYR tharsiya 001 001 1234***
                     */
                    byte[][] aux = parseUntilStars("GPLYR", msg.substring(tail.length+5).getBytes(), n, 30);
                    for(byte[] a: aux){
                        System.out.println(new String(a));
                    }
                }     
        }
        
    }


}