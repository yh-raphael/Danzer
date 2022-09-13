#!/bin/sh
#PBS -V
#PBS -N danzer-cu-all
#PBS -q debug
#PBS -A etc
#PBS -l select=1:ncpus=68
#PBS -l walltime=04:00:00
#PBS -m abe
#PBS -M yhcho.raphael@gmail.com

cd $PBS_O_WORKDIR

module purge
module load gcc/8.3.0


./danzer -f 2 -i /scratch/jklee/dup_sample/abaqus -o ./fstat_files/fstat_file-abaqus.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/amber -o ./fstat_files/fstat_file-amber.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/ansys -o ./fstat_files/fstat_file-ansys.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/bwa -o ./fstat_files/fstat_file-bwa.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/cesm -o ./fstat_files/fstat_file-cesm.csv

./danzer -f 2 -i /scratch/jklee/dup_sample/charmm -o ./fstat_files/fstat_file-charmm.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/gamess -o ./fstat_files/fstat_file-gamess.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/gaussian -o ./fstat_files/fstat_file-gaussian.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/grims -o ./fstat_files/fstat_file-grims.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/gromacs -o ./fstat_files/fstat_file-gromacs.csv


./danzer -f 2 -i /scratch/jklee/dup_sample/inhouse -o ./fstat_files/fstat_file-inhouse.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/inhouse_gridw2 -o ./fstat_files/fstat_file-inhouse_gridw2.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/lammps -o ./fstat_files/fstat_file-lammps.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/lsdyna -o ./fstat_files/fstat_file-lsdyna.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/mom -o ./fstat_files/fstat_file-mom.csv

./danzer -f 2 -i /scratch/jklee/dup_sample/mpas -o ./fstat_files/fstat_file-mpas.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/msc -o ./fstat_files/fstat_file-msc.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/namd -o ./fstat_files/fstat_file-namd.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/nastran -o ./fstat_files/fstat_file-nastran.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/openfoam -o ./fstat_files/fstat_file-openfoam.csv


./danzer -f 2 -i /scratch/jklee/dup_sample/pytorch -o ./fstat_files/fstat_file-pytorch.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/qchem -o ./fstat_files/fstat_file-qchem.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/qe -o ./fstat_files/fstat_file-qe.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/qmc -o ./fstat_files/fstat_file-qmc.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/ramses -o ./fstat_files/fstat_file-ramses.csv

./danzer -f 2 -i /scratch/jklee/dup_sample/roms -o ./fstat_files/fstat_file-roms.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/siesta -o ./fstat_files/fstat_file-siesta.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/tf -o ./fstat_files/fstat_file-tf.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/vasp -o ./fstat_files/fstat_file-vasp.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/VASP -o ./fstat_files/fstat_file-VASP.csv


./danzer -f 2 -i /scratch/jklee/dup_sample/wrf -o ./fstat_files/fstat_file-wrf.csv
./danzer -f 2 -i /scratch/jklee/dup_sample/WRF -o ./fstat_files/fstat_file-WRF.csv

