# Danzer

The Deduplication Analyzer

KISTI collaboration project

How to run?
->

./danzer -f 1 -s 4096 -m 1 -i /scratch/jklee/dup_sample/ -o ./fstat_files/
./danzer -f 1 -s 4096 -m 1 -i /scratch/jklee/dup_sample/abaqus -o ./fstat_files/fstat_file-abaqus.txt


-> Measure file size distribution (-f option: 2)

./danzer -f 2 -i /mnt/nvmssd/kvdata/ -o ../measure_file_2.csv