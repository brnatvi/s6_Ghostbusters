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
    DatagramSocket sock_udp;
    InetAddress address ;

    //Connexion TCP du client au serveur
    public Client(String adresse, String port){
        try {
            sockfd = new Socket(adresse, Integer.parseInt(port));
            in = new BufferedReader(new InputStreamReader(sockfd.getInputStream()));
            out = new PrintWriter(new OutputStreamWriter(sockfd.getOutputStream()));

        }

        catch(Exception e){
            System.out.println(e);
            e.printStackTrace();
        }
    }

    //Lire buffer
    public String read(BufferedReader in, char[] buf, int offset, int len){
        try{
            in.read(buf, offset, len);
        }
        catch(IOException e){
            System.out.println(e);
        }
        return (new String(buf)); 
    }

    public void getMessage(BufferedReader in){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
        TreatTCP.treatMess(msg);
        
    }
  
    //Partie send
    public void sendMessage(PrintWriter out){
        
        String msg = sc.nextLine();
        byte[] toSend = SendTCP.sendMess(msg);
        out.print(new String(toSend));
        out.flush();
        
        //dans SendTCP
    }

    //TODO: vérifier que pseudo est bien 8 caractères
 
    // VRAI TEST
    public static void main(String[] args){       
        Client p = new Client(args[0], args[1]);
        while(true){
            p.getMessage(p.in);
            p.sendMessage(p.out);    
        }       
             
    }

}

