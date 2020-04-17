# params m
max_t=$(expr $1 - 1)
for n in 8 32 64 128
do
	for t in $(seq 2 $max_t);
	do
		echo "n $n t $t :"
		./benchmark all -m $1 -n $n -t $t -x -y
		./benchmark recon -m $1 -n $n -t $t -l
	done
done
