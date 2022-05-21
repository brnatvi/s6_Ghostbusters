Groupe 54,  BRAGINA Natalia (22015704) et VIGNESWARAN Tharsiya (21971806)

Le projet est divisé en trois parties : serveur (/serveur), client principal avec GUI (/client) et client secondaire avec CLI (/c_client).

Compilation :

En langage C
make

En langage Java
javac *.java


Run :
./server <port> <max size>
N.B. <max size> = 0 ou >10
     si <max size> = 0 alors la selection aleatoire de dimentions de labirinthe de 10 a 999
                        else la selection aleatoire de dimentions de labirinthe de 10 a <max size>"

et
java Client <nom de machine> <port>


L'adresse multicast utilisée est 239.255.255.250

Le client secondaire permet de lancer le programme en ḋCheat-modeṠ - execution automatique de traversee du labyrinthe.

Run en cheat-mode a partir du repertoire /c_client:

./ghost_client <server hostname> <player id> <port>
N.B. <player Id> doit contenir 8 caracteres, <port> - 4 caracteres

Puis suivre les menus textuels :
- 3 -> Create new game
- 2 -> Start game
- 7 -> On/off cheat mode

Logging.
Echange de messages entre client et serveur respect code couleur : rouge - messages de serveur, vert  messages du client.
Les erreurs etc sont affichees selon le niveau "trace", "debug", "info", "warn", "error", "fatal" (librairie log.c)


Utilisation de librairies.
Dans notre projet nous avons utilise une librairie de liste doublement chainee et les librairies externes suivantes :
- Logger : https://github.com/rxi/log.c 
- Generateur du labyrinthe : article initiale https://habr.com/ru/post/262345/ et le code https://github.com/mersinvald/maze-generator_offscreen    


Architecture.
Serveur:
le serveur.c utilise les fonctions decrites en server_aux_functions.c afin detablir la connection et de traiter les requetes recu du client;
la composition de messages en respect du protocole seffectue a laide de protocol.c ;
le moteur du jeu est engine.c et les librairies utilisees sont list.c, log.c, maze_generator.c.

Client GUI: 
le client lance une interface graphique qui permet de faciliter l'envoi de messages avec une entête de message à choisir
et les paramètres à remplir qui vont avec. L'interface affiche également une grille de taille indiquée par le serveur.
Si le message n'est pas en accord avec le protocole, le message ne sera pas envoyé.
Il est possible également de visualiser les réponses reçues du serveur et les messages que le client lui-même envoie.
Le client récupère et envoie des messages grâce aux protocoles TCP, UDP et Multicast selon la situation.

Client CLI:
main.c utilise socket.c avec les fonctions d'établissement de connexions tcp, udp et multicast et le moteur engine.c ;
utils.c consiste les fonctions auxiliaires dinput et de changement de couleurs du texte et les librairies utilisees sont list.c, log.c

Tests
Nous avons teste notre programme avec ceux de binomes 31, 53, 13

