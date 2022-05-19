import java.io.*;
import java.net.*;
import java.nio.*;
import java.util.Arrays;
import java.util.Scanner;

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

    public void getMessage(BufferedReader in){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
        // TreatTCP treat = new TreatTCP(sockfd);
        // treat.treatMess(msg);
        TreatTCP treat = new TreatTCP(sockfd, msg);
        treat.start();
        try {
            treat.join();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        if(msg.substring(0,5).equals("GOBYE")){
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
                // try {
                //     mc.join();
                // }
                // catch(Exception e){
                //     e.printStackTrace();
                // }
            }
            catch(Exception e){
                e.printStackTrace();
            }

        }
    }
  
    //Partie send
    public void sendMessage(PrintWriter out){
        String msg = sc.nextLine();
        // SendTCP send = new SendTCP();
        // byte[] toSend = send.sendMess(msg);
        // out.print(new String(toSend));
        // out.flush();
        SendTCP send = new SendTCP(msg, out);
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
                // try {
                //     wu.join();
                // }
                // catch(Exception e){
                //     e.printStackTrace();
                // }
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
        while(!p.fin){
            p.getMessage(p.in);
            p.sendMessage(p.out);    
        }       
             
    }

}

