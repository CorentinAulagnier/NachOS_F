# Compilation
clear

make > compilation.txt

cd build

echo ------------------------------
echo ------------------------------
echo PARTIE 2
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie2

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

echo ------------------------------
echo ------------------------------
echo PARTIE 4
echo ------------------------------
echo ------------------------------
read touch
./nachos-userprog -x testPartie4

echo ------------------------------
echo ------------------------------
echo PARTIE 5
echo ------------------------------
echo ------------------------------
read touch



echo ------------------------------
echo ------------------------------
echo PARTIE 6
echo ------------------------------
echo ------------------------------
echo PASSER SUR LES AUTRES TERMINAUX
echo ------------------------------
echo FIN
echo ------------------------------
