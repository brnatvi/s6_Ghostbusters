import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;
import java.util.*;
import javax.swing.*;
import java.awt.Graphics;
import java.awt.*;
import java.awt.event.*;


public class TreatTCP extends Thread{

    public String msg;
    public int port_mult;
    LabyrintheVue vue;
    JTextField answer;

    private String ip;
    // Partie partie;
    
    LinkedList<Integer> games;
    int totalGames = 0;
    int height, width = 0;
    LinkedList<String> players;
    int nbGame;
    int x, y;

    public TreatTCP(String _msg){
        this.msg = _msg; 
        // partie = new Partie();  
    }

    public void setText(JTextField _txt){
        this.answer = _txt;
    }

    // parseip till #
    public String getIP(){
        return ip;
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

    // treatMess(String msg)

    public void run(){ // enlever tcpEnd from system.out.println
        String entete = msg.substring(0,5);
        byte[] tail = parseUntilStars(entete, (msg.substring(5)).getBytes());
        String tcpEnd = "***";
        int n = 0;
        String s = "";
        switch(entete){
            case "REGNO":
            case "DUNNO":
            case "NMEMB":
            case "GOBYE":
            case "MALL!": 
            case "SEND!":
            case "NSEND":
                System.out.println(entete+new String(tail));
                answer.setText(entete+new String(tail));
                break;
            case "MOVE!":
            case "MOVEF":
                System.out.println(entete+new String(tail));
                setX(new String(tail).substring(1,4));
                setY(new String(tail).substring(5,8));
                answer.setText(entete+new String(tail));
                break;
            case "REGOK":
            case "UNROK":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                answer.setText(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                setNbGame(treatInfoOneByte(tail[1]));
                break;
            case "SIZE!":
                
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+ treatInfoTwoBytes(Arrays.copyOfRange(tail, 3, 5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail, 6, 8)) +tcpEnd);
                answer.setText(entete+" "+treatInfoOneByte(tail[1])+" "+ treatInfoTwoBytes(Arrays.copyOfRange(tail, 3, 5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail, 6, 8)) +tcpEnd);

                setNbGame(treatInfoOneByte(tail[1]));
                setHeight(treatInfoTwoBytes(Arrays.copyOfRange(tail, 3, 5)));
                setWidth(treatInfoTwoBytes(Arrays.copyOfRange(tail, 6, 8)));
                
                break;
            case "GAMES":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                s = s + (entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
                n = tail[1];
                if(n != 0){
                    byte[][]l = parseUntilStars("OGAME", msg.substring(tail.length+5).getBytes(), n, 12);
                    games = new LinkedList<Integer>();
                    initGames();
                    setTotalGames(treatInfoOneByte(tail[1]));
                    for(byte[] k: l){
                        System.out.println("OGAME "+ treatInfoOneByte(k[6])+" "+treatInfoOneByte(k[8])+tcpEnd);
                        s  = s +"\n"+("OGAME "+ treatInfoOneByte(k[6])+" "+treatInfoOneByte(k[8])+tcpEnd);
                        addToGames(treatInfoOneByte(k[6]));
                    }
                }
                answer.setText(s);
                break;
            case "LIST!":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoOneByte(tail[3])+tcpEnd); //tail[1] et tail[3]
                answer.setText(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoOneByte(tail[3])+tcpEnd);
                n = tail[3];
                if(n != 0){
                    byte[][]tmp = parseUntilStars("PLAYR", msg.substring(tail.length+5).getBytes(), n, 17);
                    initPlayers();
                    for(byte[] t: tmp){
                        System.out.println(new String(t));
                        answer.setText(answer.getText()+"\n"+new String(t));
                        addToPlayers(msg.substring(tail.length+6+5, tail.length+6+13));
                        
                    }
                }  
                break;
            case "WELCO": // à changer d'emplacement
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoTwoBytes(Arrays.copyOfRange(tail,3,5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail,6,8))+" "+treatInfoOneByte(tail[9])+" "+
                new String(Arrays.copyOfRange(tail, 11, 34)));
                answer.setText(entete+" "+treatInfoOneByte(tail[1])+" "+treatInfoTwoBytes(Arrays.copyOfRange(tail,3,5))+" "+
                treatInfoTwoBytes(Arrays.copyOfRange(tail,6,8))+" "+treatInfoOneByte(tail[9])+" "+
                new String(Arrays.copyOfRange(tail, 11, 34)));
                
                this.ip = msg.substring(16,31);
                this.port_mult = Integer.parseInt(msg.substring(32, 36));


                String posit = (msg.substring(tail.length+5));
                setX(msg.substring(tail.length+5+15, tail.length+5+15+3));
                setY(msg.substring(tail.length+5+15+4, tail.length+5+15+7));
                byte[] tmp = parseUntilStars("POSIT", msg.substring(tail.length+5).getBytes());
                
                System.out.println(new String(tmp));
                answer.setText(answer.getText()+"\n"+new String(tmp));
                break; 
            case "GLIS!":
                System.out.println(entete+" "+treatInfoOneByte(tail[1]) +tcpEnd);
                answer.setText(answer.getText()+"\n"+entete+" "+treatInfoOneByte(tail[1]) +tcpEnd);
                n = tail[1];
                if(n != 0){
                    
                    byte[][] aux = parseUntilStars("GPLYR", msg.substring(tail.length+5).getBytes(), n, 30);
                    for(byte[] a: aux){
                        System.out.println(new String(a));
                        answer.setText(answer.getText()+"\n"+new String(a));
                    }
                }     
        }


        
    }

    public void initGames(){
        games = new LinkedList<Integer>();
    }

    public void initPlayers(){
        players= new LinkedList<String>();
    }

    public void setX(String val){
        x = Integer.parseInt(val);
    }

    public void setY(String val){
        y = Integer.parseInt(val);
    }

    public void setHeight(String val){
        height = Integer.parseInt(val);
        
    }

    public void setWidth(String val){
        width = Integer.parseInt(val);
        
    }

    public void setNbGame(String val){
        nbGame = Integer.parseInt(val);
    }

    public void setTotalGames(String val){
        totalGames = Integer.parseInt(val);
    }

    public void addToGames(String val){
        games.add(Integer.parseInt(val));
    }

    public void addToPlayers(String val){
        players.add(val);
    }

    public void clearGames(){
        games.clear();
    }

    public void clearPlayers(){
        players.clear();
    }

    

        public static void main(String[] args){
            String s = " 001 003";
            byte[] tail = s.getBytes();
            System.out.println((new String(tail).substring(1,4)));
            System.out.println((new String(tail).substring(5,8)));
        }


}