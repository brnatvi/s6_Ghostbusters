import java.util.Arrays;

public class TreatTCP {

    private static int LEN_END = 3;
    private static int LEN_WHITE = 1;
    private static int LEN_ID = 8;
    private static int SZ_BYTE = 1;
    private static int STAR = 42;
    private static String tcpEnd = "***";

    public static byte[] conditionMet(byte[] tail, int n1){
        if(tail[n1] == STAR && tail[n1+1] == STAR && tail[n1+2] == STAR){
            System.out.println("met?Met!");
            return (Arrays.copyOfRange(tail, 0, n1 + LEN_END));
        }
        return null;
    }

    //parser jusqu'à trouver '***' return byte array
    public static byte[] parseUntilStars(String entete, byte[]tail){
        if(entete.equals("GAMES") || entete.equals("REGOK") || entete.equals("UNROK")){
            return conditionMet(tail, LEN_WHITE + SZ_BYTE);
        }
        else if(entete.equals("REGNO")){
            return conditionMet(tail, 0);
        }
        //full message
        else if(entete.equals("OGAME")){
            return conditionMet(tail, LEN_WHITE + LEN_ID);
        }
        else if(entete.equals("LIST!")){
            return conditionMet(tail, LEN_WHITE + SZ_BYTE);
        }
        else if(entete.equals("PLAYR")){
            return conditionMet(tail, LEN_WHITE + LEN_ID);
        }
        return null;
    }

    //return byte double array
    public static byte[][] parseUntilStars(String entete, byte[]tail, int nb, int len){
        byte[][] lst = new byte[nb][];
        for(int i = 0; i<lst.length; i++){
            lst[i] = parseUntilStars(entete, Arrays.copyOfRange(tail, i*len, tail.length));
        }
        return lst;
    }

    // n, m et s 
    public static String treatInfoOneByte(byte info){
        return (String.valueOf((int) info));
    }

    public static void treatMess(String msg){
        String entete = msg.substring(0,5);
        byte[] tail = parseUntilStars(entete, (msg.substring(5)).getBytes());
        //String tcpEnd = "***";
        int n = 0;
        System.out.println(entete);
        switch(entete){
            case "REGNO":
            case "DUNNO":
                System.out.println(entete+new String(tail));
                break;
            case "REGOK":
            case "UNROK":
                System.out.println(entete+" "+treatInfoOneByte(tail[1])+tcpEnd);
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
                System.out.println("a");
                System.out.println(entete+" "+treatInfoOneByte(tail[0])+" "+treatInfoOneByte(tail[2])+tcpEnd); //tail[1] et tail[3]
                System.out.println("b");
                n = tail[2];
                System.out.println("c");
                if(n != 0){
                    byte[][]tmp = parseUntilStars("PLAYR", msg.substring(tail.length+5).getBytes(), n, 17);
                    for(byte[] t: tmp){
                        System.out.println(new String(t));
                    }
                }
                System.out.println("d");
        }
        
    }
}