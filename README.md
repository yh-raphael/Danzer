# Danzer

The Deduplication Analyzer

KISTI collaboration project


## How to run?

### Trace phase (-f option: other than 1,2,3)

./danzer -f 4 -s 4096 -m 1 -i /scratch/jklee/dup_sample/ -o ./fstat_files/
./danzer -f 4 -s 4096 -m 1 -i /scratch/jklee/dup_sample/abaqus -o ./fstat_files/fstat_file-abaqus.txt

### FStat: Measure file size distribution (-f option: 1)

./danzer -f 1 -i /mnt/nvmssd/kvdata/ -o ../measure_fs.csv
./danzer -f 1 -i /scratch/jklee/dup_sample/abaqus -o ./fstat_files/fstat_file-abaqus.csv

### FStat: Measure cumulative distribution (-f option: 2)

./danzer -f 2 -i /mnt/nvmssd/kvdata/ -o ../measure_c_fs.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/abaqus -o ./fstat_files/fstat_file-abaqus.csv