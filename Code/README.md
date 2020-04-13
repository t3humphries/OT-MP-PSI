# Over-Threshold Multi-Party Private Set Intersection

## Description
This is the library for performing threshold private set intersection intersection in a multiparty fashion. 

## Dependencies
* GMP==6.1.2
* NTL==11.4.1
* Paillier library: [libhcs](https://github.com/tiehuis/libhcs)


## Making the files
run ```make``` in ```server``` and ```benchmark``` directory.

## Benchmarking

Run ```./server``` in the ```server``` directory before running benchmarks
The ```./benchmark``` file in the ```benchmark``` directory can be run as follows:

#### Commands:  
	all: run the entire benchmarking process  
	sharegen: run the share generation process  
	recon: run the reconstruction process  
#### Options:  
    -h	Show this help message  
	-m	Number of parties (default=10)  
	-n	Max number of elements (default=10)  
	-t	Threshold (default=2)  
	-b	Prime bit length (default=1024)  
	-f	Regenerate elements for benchmarking instance (default=false)  
	-k	Address of keyholder server (default 127.0.0.1)  
	-r	Number of times to repeat experiment (has defaults)  
	-s	Choice of Scheme 0=both,1,2 (default 0)   
	-l  Log results in file (default false)
	-x  Don't run the reconstruction in the "all" command
	-y  Fast Share Generation in the "all" command, for generating shares fast, without the actual protocol


## Examples
```./benchmark all -m 10 -n 100 -t 2 -b 2048 -k 127.0.0.1 -r 1 -s 0```

```./benchmark sharegen -t 2 -b 2048 -k 127.0.0.1 -r 1 -s 0```

```./benchmark recon -m 10 -n 100 -t 2 -b 2048 -k 127.0.0.1 -r 1 -s 0```

