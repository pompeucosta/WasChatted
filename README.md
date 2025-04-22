# WasChatted
WasChatted is a program developed within the scope of a university project.
It uses a Markhov model for data compression with the goal of determining of a text was rewritten by ChatGPT.
The idea is to build a program that, for a certain target text given to it, *t*, it indicates one of two possibilities: (1) the text was not rewritten by ChatGPT; (2) the text was rewritten by ChatGPT.

## How to build the program
```bash
g++ -o main was_chatted.cpp helpers.cpp -O3
```

## Parameters
The program has 2 commands and 1 parameter.

| Parameter 	   |            Explanation    	                                | 
|:-------------: |:--------------------------------------------------------:	|
|     train      |     Trains the model    	                                  |
|     analyse    |  Analyses a text file to determine the class               |
|     -h     	   |         Display this help message                        	|

### Train
  This command contains 4 parameters.

| Parameter 	|                 Explanation                          | Default Value 	|
|:---------:	|:---------------------------------------------------: |:-------------:	|
|     o     	|  Name of the output file containing the trained data |  output.bin   	|
|     k     	|     Sets the context size              	             |     4        	|
|     a     	|     Sets the value of alpha/smoothing 	             |     1       	  |
|     h     	|     Display the help message       	                 |              	|


It's important to note that:
- `k` must be a positive integer
- `a` must be a positive number

### Analyse
  This command contains 2 parameters.

| Parameter 	|              Explanation               |
|:---------:	|:-------------------------------------: |
|     o     	|   File to output result to             |
|     h     	|     Display the help message           |
