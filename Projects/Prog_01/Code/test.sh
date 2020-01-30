echo -e "--------Test for too few arguments--------"
./launcherscript.sh 
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test for an incorrect executable-----"
./launcherscript.sh ./wrong 1 2
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n------Test with the largest argument in the first position------"
./launcherscript.sh ./primes 5 2 1
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n--------Test with the largest argument in the middle position------"
./launcherscript.sh ./primes 1 5 2
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n------Test with the largest argument in the last position------"
./launcherscript.sh ./primes 1 2 5
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n------Test with four arguments------"
./launcherscript.sh ./primes 1 2 3 4
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-------Test for extremely high delta--------"
./launcherscript.sh ./primes 1 10000
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n------Test with letters------"
./launcherscript.sh ./primes 1 2 a
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test with negative numbers-----"
./launcherscript.sh ./primes -2 3
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n----Test with floating-point numbers-----"
./launcherscript.sh ./primes 1 1.2
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test with 0 as an argument-----"
./launcherscript.sh ./primes 0 1 2
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test without ./-------"
./launcherscript.sh primes 1 2 3
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n------Test with a long list------"
./launcherscript.sh ./primes 1 2 3 4 5 6 7 8 9 10
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test with repeats-------"
./launcherscript.sh ./primes 1 2 3 3
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test of all repeats-------"
./launcherscript.sh ./primes 2 2 2 
read -n 1 -s -r -p "Press any key to continue"

echo -e "\n-----Test of an absolute path------"
./launcherscript.sh /home/aidan/CSC412/Projects/Prog_01/Code/primes 1 2 3
read -n 1 -s -r -p "Press any key to continue"