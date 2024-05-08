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
The program accepts 4 parameters.

The 4 parameters are:
| Parameter 	   |            Explanation    	                                | 
|:-------------: |:--------------------------------------------------------:	|
|     train      |     Trains the model    	                                  |
|     analyze    |  Analyzes a text file to determine the class               |
|     -h     	   |         Display this help message                        	|

### Train
  Using this parameter it can be added 4 addictional parameters.

| Parameter 	|                 Explanation                          | Default Value 	|
|:---------:	|:---------------------------------------------------: |:-------------:	|
|     o     	|  Name of the output file containing the trained data |  output.bin   	|
|     k     	|     Set the size of the kmer           	             |     10        	|
|     s     	|     Sets the value of alpha/smoothing 	             |     1       	  |
|     h     	|     Display the help message       	                 |              	|


It's important to note that:
- k must be a positive integer
- a must be a decimal value greater than 0

### Analyze

  Using this parameter it can be added 4 addictional parameters.

| Parameter 	|              Explanation               |
|:---------:	|:-------------------------------------: |
|     o     	|   File to output result to             |
|     h     	|     Display the help message           |
