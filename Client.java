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

        //parser jusqu'à trouver '***' return byte array
    public static byte[] parseUntilStars(String entete, byte[]tail){
        if(entete.equals("GAMES") || entete.equals("REGOK") || entete.equals("UNROK")){
            if(tail[2] == 42 && tail[3] == 42 && tail[4] == 42){
                return (Arrays.copyOfRange(tail,0,5));
            }  
        }
        else if(entete.equals("REGNO")){
            if(tail[0] == 42 && tail[1] == 42 && tail[3] == 42){
                return (Arrays.copyOfRange(tail, 0, 4));
            }
        }
        //full message
        else if(entete.equals("OGAME")){
            
            if(tail[9] == 42 && tail[10] == 42 && tail[11] == 42){
                return (Arrays.copyOfRange(tail, 0, 12));
            }
        }
        return null;
    }

        //return byte double array
    public static byte[][] parseUntilStars(String entete, byte[]tail, int nb){
        byte[][] lst = new byte[nb][];
        int lenog = 11; //OGAME
        for(int i = 0; i<lst.length; i++){
            lst[i] = parseUntilStars(entete, Arrays.copyOfRange(tail, i*12, tail.length));
        }
        return lst;
    }

    public void getMessage(BufferedReader in){
        char[] buf = new char[1024];
        String msg = read(in, buf, 0, 1024);
        
        int begin = 5;
        String entete = msg.substring(0,5);
        byte[] tail = parseUntilStars(entete, (msg.substring(begin)).getBytes());
        
        
        byte[] info = Arrays.copyOfRange(tail, 1, tail.length-3);//tail.substring(0,tail.length()-3);
        byte[] tcpEnd = Arrays.copyOfRange(tail, tail.length-3,tail.length);//tail.substring(tail.length()-3);

        //différencier GAMES pour obtenir la suite s'il y en a -> OGAME
        if(entete.equals("GAMES") || entete.equals("REGOK") || entete.equals("UNROK")){
            System.out.println(entete +" "+treatInfoOneByte(info) + new String(tcpEnd));

            if(entete.equals("GAMES") && !treatInfoOneByte(info).equals("0")){
                System.out.println(Integer.parseInt(treatInfoOneByte(info)));
                byte[][]l = parseUntilStars("OGAME", msg.substring(tail.length+5).getBytes() ,Integer.parseInt(treatInfoOneByte(info)));
                for(byte[] k: l){
                    System.out.println("OGAME "+String.valueOf((int) k[6]) +" "+String.valueOf((int) k[8]) +"***");
                }
            }
        }
        
        else if(entete.equals("REGNO") || entete.equals("DUNNO")){
            System.out.println(entete +" "+ new String(tail));
        }
        
        else if(entete.equals("LIST!")){
            System.out.println("LIST! "+String.valueOf((int) info[0]) +" "+String.valueOf((int) info[2])+"***");
            byte[][]l = parseUntilStars("PLAYR", msg.substring(tail.length+5).getBytes(), (int)info[2]);
            for(byte[] k: l){
                System.out.println(new String(k));
            }
        }
        
    }

        //treat msg with one option which is written in one byte {GAMES, REGOK, UNROK}
    //param would become byte[]info
    public static String treatInfoOneByte(byte[] info){
        return (String.valueOf((int) info[0]));
    }
 
  
    //Partie send
    public void sendMessage(PrintWriter out){
        //TODO no nextline possible
        String msg = sc.nextLine();
        byte[] a = msg.getBytes();
        
        if(msg.substring(0,5).equals("REGIS")){
            byte[]res = sendRegis(msg);
            
            out.print(new String(res));
            out.flush();
        }
        else if(msg.substring(0,5).equals("NEWPL")){
            byte[]res = sendNewpl(msg);
            out.print(new String(res));
            out.flush();
        }
        else if(msg.substring(0,5).equals("UNREG")){
            byte[]res = sendUnreg(msg);
            out.print(new String(res));
            out.flush();
        }
        else if(msg.substring(0,5).equals("LIST?") || msg.substring(0,5).equals("SIZE?")){
            byte[]res = sendList(msg);
            out.print(new String(res));
            out.flush();
        }
        else {
            out.print(a);
            out.flush();
        }

    }

    //TODO: vérifier que pseudo est bien 8 caractères
    public static byte[] sendNewpl(String msg){
        return msg.getBytes();
    }
    //send REGIS
    public static byte[] sendRegis(String msg){ //PrintWriter
        byte[] entete = (msg.substring(0,20)).getBytes();

        byte[] nb = new byte[1];
        int m = Integer.parseInt(msg.substring(20, msg.length()-3));
        nb[0] = (byte)((m >> 0) & 0xff);

        byte[] tcpEnd = (msg.substring(msg.length()-3)).getBytes(); 

        byte[] toSend = new byte[entete.length + nb.length + 3];
        System.arraycopy(entete, 0, toSend, 0, entete.length);
        System.arraycopy(nb, 0, toSend, entete.length, 1);
        System.arraycopy(tcpEnd, 0, toSend, entete.length+1, 3);
        return toSend;
    }

    public static byte[] sendUnreg(String msg){
        byte[] entete = (msg.substring(0,6)).getBytes();
        int m = Integer.parseInt(msg.substring(6, msg.length()-3));
        byte[] nb = new byte[1];
        nb[0] = (byte)((m >> 0) & 0xff);
        System.out.println(m);

        byte[] tcpEnd = (msg.substring(msg.length()-3)).getBytes();
        byte[] toSend = new byte[entete.length + nb.length + 3];
        System.arraycopy(entete, 0 ,toSend, 0, entete.length);
        System.arraycopy(nb, 0, toSend, entete.length, 1);
        System.arraycopy(tcpEnd, 0, toSend, entete.length+1, 3);
        for(byte b: toSend){
            System.out.print(b+" ");
        }
        System.out.println();
        return toSend;
    }

    public static byte[] sendList(String msg){
        byte[] entete = (msg.substring(0,6)).getBytes();
        int m = Integer.parseInt(msg.substring(6, msg.length()-3));
        byte[] nb = new byte[1];
        nb[0] = (byte)((m >> 0) & 0xff);

        byte[] tcpEnd = (msg.substring(msg.length()-3)).getBytes();
        byte[] toSend = new byte[entete.length + nb.length + 3];
        System.arraycopy(entete, 0 ,toSend, 0, entete.length);
        System.arraycopy(nb, 0, toSend, entete.length, 1);
        System.arraycopy(tcpEnd, 0, toSend, entete.length+1, 3);
        return toSend;
    }

    

    // VRAI TEST
    public static void main(String[] args){       
        Client p = new Client(args[0], args[1]);
        while(true){
            p.getMessage(p.in);
            p.sendMessage(p.out);
           
        }       
             
    }



}

