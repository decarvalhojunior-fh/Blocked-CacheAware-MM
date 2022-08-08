#!/bin/sh
for procs in 4 2 1
do    
    for type in blocked simple
    do    
        for opt in o3 o0
        do
             for size in 2400 3200 4000 4800 5600 6400
             do
                for turns in 1 2 3
                do
		    echo "$procs threads / $type / $opt / size=$size"
                    ./mbmm_$opt $size $procs $type >> output.$procs.$type.$opt.$size
                done
	     done
        done
    done
done


