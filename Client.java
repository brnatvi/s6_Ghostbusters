import java.io.*;
import java.net.*;
import java.util.Scanner;

class Client {

    //close correctement
    BufferedReader in;
    PrintWriter out;
    Socket sockfd;
    Scanner sc = new Scanner(System.in);

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
        return (new String(buf)).trim();
    }

    //récupérer message avec peu d'appels systemes
    public void getMessage(BufferedReader in){
        String msg = "";
        char[] buf = new char[1024];
        msg = read(in, buf, 0, 1024);
        int begin = 5;
        String en_tete = msg.substring(0,5);
        if(en_tete.equals("GAMES")){

        }
        else if(en_tete.equals("REGOK")){
            
        }
        
        else if(en_tete.equals("REGNO")){
            System.out.println(en_tete + msg.substring(begin, begin+3));            
        }
        else if(en_tete.equals("UNROK")){

        }
        else if(en_tete.equals("SIZE!")){
            
        }
        else if(en_tete.equals("LIST!")){
            
        }   
        else if(en_tete.equals("PLAYR")){
            
        }
        else if(en_tete.equals("DUNNO")){
            System.out.println(en_tete + msg.substring(begin, begin+3));
        }     
    }

    //envoie un message
    public void sendMessage(PrintWriter out){
        String msg = sc.nextLine();
        out.print(msg);
        out.flush();
    }

    public static void main(String[] args){
        Client p = new Client(args[0], args[1]);
        p.getMessage(p.in);
        p.sendMessage(p.out);
        
    }
}