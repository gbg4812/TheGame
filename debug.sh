read -p "Seed: " seed
read -p "Main player: " player1
read -p "Secondary player: " player2

make $1 OPTIMIZE=0 DEBUG=1

echo run $player2 $player2 $player2 $player1 -s $seed "<default.cnf >OUT.txt" >gdbargs.txt
gdb -x gdbargs.txt Game
