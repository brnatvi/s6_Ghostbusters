import java.io.*;
import java.net.*;
import java.util.Scanner;

class Client {

    //close correctement
    private BufferedReader in;
    private PrintWriter out;
    private Socket sockfd;
    private Scanner sc = new Scanner(System.in);


    public void getMessage(){
        Thread get = new Thread(new Runnable() {
            char recu;
            int stars = 0;

            @Override
            public void run(){
                try {
                    while((recu = (char)in.read()) != '*' || stars != 3){ 
                        System.out.print(recu);
                        if(recu == '*') {
                            stars ++;   
                        }
                        if(stars == 3 && in.ready()){ //Si on a trois étoiles mais qu'il reste encore à lire, on continue en réinitialisant stars à 0
                            stars = 0;
                        }
                    }
                    stars = 0;
                    
                    System.out.println();
                    
                }
                catch(Exception e){
                    e.printStackTrace();
                }
            }

        });
        get.start();

    }

    public void sendMessage(){
        Thread send = new Thread(new Runnable(){
            String msg;     
            
            @Override
            public void run(){
                
                while(true){
                    msg = sc.nextLine();
                    out.print(msg);
                    out.flush();
                } 
            }

        });
        send.start();
    }

    public void communication(){
       
        getMessage();
        sendMessage();
        
    }

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

    public static void main(String[] args){
        Client p = new Client("localhost", "4242");
        p.communication();
    }
}