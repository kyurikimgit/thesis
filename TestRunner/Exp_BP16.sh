#/bin/bash

for class in 1 2 3 4 5 6 7 8 9 10
do
    if [ $class -eq 10 ]; then
        sizes="24 51 99 201"
    else
        sizes="25 50 100 200"
    fi

    for dims in 20
    do
        for size in $sizes
        do
            for instance in 1
            do

##### Data loader
				LOADER=../BP16Loader.so    	# ../C0Loader.so ../C6Loader.so ../CXLoader.so ../SWFLoader.so ../GWFLoader.so
## required for CXLoader
					VBPCLASS=$class
					VBPSIZE=$size
					VBPDIMENSION=$dims
					VBPINSTANCE=$instance
					INTERVAL_MIN=0.2
					INTERVAL_MAX=0.8
## required forSWFLoader and GWFLoader
					JOB_FILE=../Datasets/anon_jobs.gwf
					ALGO=(../ArcFlow.so ../LSpatterns.so ../Pani.so ../Hybrid.so ../LSD_I1.so ../LSD_I2.so ../Kou.so ../KouSort.so)
					ONLINE=false				# NOT USED YET
					STEP_WIDTH=2
					TESTTYPE=maxMachineLoad		# NOT USED YET
					DIMENSIONS=$VBPDIMENSION
					EPSILON=0.1
					RUNS=1
					START_JOB=$VBPSIZE
					RANDOM_JOB_SELECTION=false

##### Pseudo-random number generator seed
					SEED=4321

##### Jobs and environment
					JOBS=$VBPSIZE
					MACHINES=1000
					MACHINE_SIZE=1.0
					MAX_CPUS=-1
					MAX_RAM=-1
					MAX_LDS=-1
					MAX_NET=-1

### Parameters for online algorithms
					LOWERBOUND=0.1
					UPPERBOUND=0.3
					LOWERBURST=500
					UPPERBURST=3000
					ARRIVAL_SPAN=1500




					for i in ${ALGO[@]}
			do
				echo loader $LOADER > config0.txt
					echo vbpClass $VBPCLASS >> config0.txt
					echo vbpSize $VBPSIZE >> config0.txt
					echo vbpDimension $VBPDIMENSION >> config0.txt
					echo vbpInstance $VBPINSTANCE >> config0.txt
					echo algorithm $i  >> config0.txt
					echo online $ONLINE >> config0.txt 
					echo step_width $STEP_WIDTH >> config0.txt 
					echo test_type $TEST_TYPE >> config0.txt 
					echo dimensions $DIMENSIONS >> config0.txt 
					echo epsilon $EPSILON >> config0.txt 
					echo jobs $JOBS >> config0.txt 
					echo machines $MACHINES >> config0.txt 
					echo machine_size $MACHINE_SIZE >> config0.txt 
					echo runs $RUNS >> config0.txt 
					echo start_jobs $START_JOB >> config0.txt 
					echo file $JOB_FILE >> config0.txt 
					echo lower_bound $LOWER_BOUND >> config0.txt 
					echo upper_bound $UPPER_BOUND >> config0.txt 
					echo lower_burst $LOWER_BURST >> config0.txt 
					echo upper_burst $UPPER_BURST >> config0.txt 
					echo arrival_span $ARRIVAL_SPAN >> config0.txt 
					echo random_job_selection $RANDOM_JOB_SELECTION >> config0.txt 
					echo max_ram $MAX_RAM >> config0.txt 
					echo max_cpus $MAX_CPUS >> config0.txt 
					echo max_local_disk $MAX_LDS >> config0.txt 
					echo max_network $MAXNET >> config0.txt 
					echo interval_min $INTERVAL_MIN >> config0.txt 
					echo interval_max $INTERVAL_MAX >> config0.txt 
					echo Currently running: $ALGO
					RAND_SEED=$SEED taskset -c 0 ../VSSimKernel config0.txt
#RAND_SEED=$SEED valgrind --leak-check=full ../VSSimKernel config0.txt
			done	done
		done
	done
done
