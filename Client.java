import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;
import java.util.concurrent.*;

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
    public void getMessage(){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
        // TreatTCP treat = new TreatTCP(sockfd);
        // treat.treatMess(msg);
        treat = new TreatTCP(msg);
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
                mc.start();
                
            }
            catch(Exception e){
                e.printStackTrace();
            }

        }
    }
  
    //Partie send
    // , LabyrintheVue l
    public void sendMessage(String msg){
        // String msg = sc.nextLine();
        System.out.println(msg);
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
        // LabyrintheVue l = new LabyrintheVue(10,12, 100, 100, p.in, p.out, p.sockfd);
        LinkedList<String> players = new LinkedList<String>();
        LinkedList<Integer> games = new LinkedList<Integer>();
        int nbGame, totalGames, width, height = 0;

        try {
            p.getMessage();
            Scenario sc = new Scenario(p.treat);
            System.out.println(p.treat);
            totalGames = p.treat.totalGames;
            games = p.treat.games;

            p.sendMessage(sc.inscription(totalGames, games));
            p.getMessage();
            nbGame = p.treat.nbGame;
            
            TimeUnit.SECONDS.sleep(3);
            
            p.sendMessage(sc.sizeAndList("SIZE?", nbGame));
            System.out.println(nbGame);
            p.getMessage();
            System.out.println(p.treat);
            TimeUnit.SECONDS.sleep(3);
            height = p.treat.x;
            width = p.treat.y;
            System.out.println(height);
            System.out.println(width);
            p.sendMessage(sc.sizeAndList("LIST?", nbGame));
            players = p.treat.players;
            p.getMessage();
            TimeUnit.SECONDS.sleep(3);

            p.sendMessage("START***");
            TimeUnit.SECONDS.sleep(3);
            p.getMessage();
            
            int coup = 50;
            while(coup > 0){
                p.sendMessage(sc.movePlayer());
                p.getMessage();
                TimeUnit.SECONDS.sleep(3);

                String s = sc.chatMessage();
                if(s != ""){
                    p.sendMessage(s);
                    p.getMessage();
                    TimeUnit.SECONDS.sleep(3);
                }
                
                coup --;
            }
            p.sendMessage(sc.quit());
            p.getMessage();
        }
        catch(Exception e){
            e.printStackTrace();
        }

             
    }

}

