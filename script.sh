#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=Hist_Pthreads_I
#SBATCH --output=Hist_Pthreads_I.out
#SBATCH --error=Hist_Pthreads_I.err
#SBATCH --time=0-00:59
#SBATCH --hint=compute_bound

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32

#Apaga os tempos
rm times/*

#Apaga os binarios
rm bin/*

#Apagar os resultados
rm data/*

#Compila o código
gcc -g -Wall src/hist_pthreads.c -o bin/hist_pthreads -lm -lpthread -std=gnu99
gcc -g -Wall src/hist_serial.c -o bin/hist_serial -lm -std=gnu99

tentativas=10 #Quantas vezes o código será executado

    for size in 50000 70000 90000 150000 #tamanho do problema
		do
            let sizeF=size*1000   	
			echo -e "\n$sizeF\t\t\c" >> "times/serial.txt" 

			timerSeq=0
			for tentativa in $(seq $tentativas) #Cria uma vetor de 1 até "tentativas"
			do
				#Executar o código.
				valorSeq=`./bin/hist_serial $size`
				#echo $valor
				timerSeq=$((valorSeq+timerSeq))

			done

			echo -e `echo "scale=2;${timerSeq}/100/${tentativas}"|bc` >> "times/serial.txt"

		done


    for cores in 4 8 16 32 #números de cores utilizados
    do
        for size in 50000 70000 90000 150000 #tamanho do problema
        do  
            let sizeF=size*1000	
            echo -e "\n$cores\t${sizeF}\t\t\c" >> "times/pthreads.txt" 

            timerPth=0
            for tentativa in $(seq $tentativas) #Cria uma vetor de 1 até "tentativas"
            do
                #Executar o código.
                valorPth=`srun ./bin/hist_pthreads $size $cores`
                #echo $valorPth
                timerPth=$((valorPth+timerPth))

            done

            echo -e `echo "scale=2;${timerPth}/100/${tentativas}"|bc` >> "times/pthreads.txt"
        done
    done
exit
