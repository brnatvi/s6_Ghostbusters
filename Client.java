import java.io.*;
import java.net.*;
import java.util.Scanner;

class Client {

    //TODO: close correctement
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
        return (new String(buf)); 
    }

    //parser jusqu'à trouver '***'
    public String parseUntilStars(String msg){
        int ind;
        int stars = 0;
        for(ind = 0; ind < msg.length(); ind++){
            try {
                if(
                    (msg.charAt(ind) == '*') && 
                    (msg.charAt(ind) == msg.charAt(ind+1) && msg.charAt(ind+1) == msg.charAt(ind+2)) && 
                    (!msg.substring(ind+3).isEmpty()) 
                ) {
                    return msg.substring(0,ind+3)+parseUntilStars(msg.substring(ind+3));
                }
                else if(
                    (msg.charAt(ind) == '*') && 
                    (msg.charAt(ind) == msg.charAt(ind+1) && msg.charAt(ind+1) == msg.charAt(ind+2)) 
                ){
                    
                    return msg.substring(0,ind+3);
                }
            }
            catch(Exception e){

            } 
        }
        return "";
    }

    //récupérer message avec peu d'appels systemes
    public void getMessage(BufferedReader in){
        String msg = "";
        char[] buf = new char[1024];
        msg = read(in, buf, 0, 1024);
        int begin = 5;
        String en_tete = msg.substring(0,5);
        System.out.print(en_tete+parseUntilStars(msg.substring(begin)));
        // if(en_tete.equals("GAMES")){
        //     System.out.print(en_tete+parseUntilStars(msg.substring(begin)));
        // }
        // else if(en_tete.equals("REGOK")){
            
        // }
        
        // else if(en_tete.equals("REGNO")){
        //     System.out.print(en_tete+parseUntilStars(msg.substring(begin)));
        // }
        // else if(en_tete.equals("UNROK")){

        // }
        // else if(en_tete.equals("SIZE!")){
            
        // }
        // else if(en_tete.equals("LIST!")){
            
        // }   
        // else if(en_tete.equals("PLAYR")){
            
        // }
        // else if(en_tete.equals("DUNNO")){
        //     System.out.println(en_tete + msg.substring(begin, begin+3));
        // }     
    }

    //envoie un message
    public void sendMessage(PrintWriter out){
        String msg = sc.nextLine();
        out.print(msg);
        out.flush();
    }

    public static void main(String[] args){
        Client p = new Client(args[0], args[1]);
        while(true){
            p.getMessage(p.in);
            p.sendMessage(p.out);
        }

        
    }
}