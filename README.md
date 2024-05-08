# TAI Group #4
## Second project
## Developed by:
- Pompeu Costa, 103294
- Rafael Pinto, 103379
- Ricardo Antunes, 98275

## How to build the program
```bash
g++ -o main was_chatted.cpp helpers.cpp -O3

```

## Parameters
The program has 2 commands and 1 parameter.

| Parameter 	   |            Explanation    	                                | 
|:-------------: |:--------------------------------------------------------:	|
|     train      |     Trains the model    	                                  |
|     analyze    |  Analyzes a text file to determine the class               |
|     -h     	   |         Display this help message                        	|

### Train
  This command contains 4 parameters.

| Parameter 	|                 Explanation                          | Default Value 	|
|:---------:	|:---------------------------------------------------: |:-------------:	|
|     o     	|  Name of the output file containing the trained data |  output.bin   	|
|     k     	|     Set the size of the kmer           	             |     4        	|
|     s     	|     Sets the value of alpha/smoothing 	             |     1       	  |
|     h     	|     Display the help message       	                 |              	|


It's important to note that:
- k must be a positive integer
- a must be a decimal value greater than 0

### Analyze
  This command contains 2 parameters.

| Parameter 	|              Explanation               |
|:---------:	|:-------------------------------------: |
|     o     	|   File to output result to             |
|     h     	|     Display the help message           |
