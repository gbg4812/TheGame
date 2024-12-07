read -p "How many tests?" num
read -p "Main player: " player1
read -p "Secondary player: " player2

make $1 OPTIMIZE=3 DEBUG=0
for ((i = 0; i < $num; i++)); do
    ./Game $player2 $player1 $player2 $player1 -s $i <default.cnf 2>&1 >OUT$i.txt | grep score
done
