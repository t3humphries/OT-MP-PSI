# Over-Threshold Multi-Party Private Set Intersection

## Description
This is exprimental code for the paper "Practical Over-Threshold Multi-Party Private Set Intersection". It benchmarks over threshold private set intersection  in a multiparty fashion. 

## Dependencies
Experiments are to be run on a Unix machine. Also, the following dependencies are needed:

* GMP==6.1.2
* NTL==11.4.1
* Paillier library: [libhcs](https://github.com/tiehuis/libhcs)


## Building the project
Run ```make``` in ```server``` and ```benchmark``` directory.

## Benchmarking

Run ```./server``` in the ```server``` directory before running benchmarks
Now the ```./benchmark``` executable in the ```benchmark``` directory can be run as decribed below:

#### Commands:
	all: run the entire benchmarking process, generate all shares, reconstruct in all bins
	sharegen: run a single share generation process, micro-benchmark
	recon: run the reconstruction process in only one bin, micro-benchmark

#### Options:
	-h	Show this help message
	-m	Number of parties (default=10)
	-n	Max number of elements (default=10)
	-t	Threshold (default=2)
	-b	Prime bit length (default=2048)
	-c	Constant for number of bins (comm. vs. comp. tradeoff) (default=1)
	-f	Regenerate elements for benchmarking instance (default=false)
	-k	IP Address of keyholder server (default 127.0.0.1)
	-r	Number of times to repeat experiment (has defaults)
	-s	Choice of Scheme 0=both,1,2 (default 0) 
	-l	Log results in file (default false) 
	-x	Don't run the reconstruction in the "all" command 
	-y	Fast Share Generation in the "all" command, for generating shares fast, without the actual protocol


## Examples
```./benchmark all -m 10 -n 100 -t 2 -b 2048 -k 127.0.0.1 -s 0```

```./benchmark sharegen -t 2 -b 2048 -k 127.0.0.1 -r 1 -s 0```

```./benchmark recon -m 10 -n 100 -t 2 -b 2048 -r 1 -s 0```

