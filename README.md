# formal2021-practice

### Earley algorithm

Standart CF-grammar parsing algorithm. All the functions are covered with tests. 
To build project use command:
 mkdir build && cd build && cmake ..
To run tests and generate cov. report use:
 make coverage_report
To run the interactive mode:
 make earley && ./earley



### LR(1) algorithm

Standart LR-parser for grammars LR(1) type.
Building and running tests is similar to Earley.
To run the interactive mode:
 make LR_1_algo && ./LR_1_algo
