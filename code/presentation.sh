# Compilation
clear
make > compilation.txt

cd build


#################################################################
# Partie 2
echo ------------------------------
echo ------------------------------
echo PARTIE 2
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie2


#################################################################
# Partie 3
echo ------------------------------
echo ------------------------------
echo PARTIE 3
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie3

echo ------------------------------
echo ------------------------------
echo PARTIE 3 Philosophes
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie3Philosophes

echo ------------------------------
echo ------------------------------
echo PARTIE 3 Producteur Consommateur
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie3ProducerConsumer


#################################################################
# Partie 4
echo ------------------------------
echo ------------------------------
echo PARTIE 4
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie4

echo ------------------------------
echo ------------------------------
echo PARTIE 4 Tentative de creation de trop de processus
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x test_toomuchproc

echo ------------------------------
echo ------------------------------
echo PARTIE 4 Producteur Consommateur & Philosophes
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x test_prodphilo


#################################################################
# Partie 5
echo ------------------------------
echo ------------------------------
echo PARTIE 5
echo ------------------------------
echo ------------------------------
read touch
./nachos-filesys -testfile


#################################################################
# Partie 6
echo ------------------------------
echo ------------------------------
echo PARTIE 6
echo ------------------------------
echo ------------------------------
echo PASSER SUR LES AUTRES TERMINAUX
echo ------------------------------
echo ------------------------------
