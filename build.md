[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/3MmVbb7f)
# Debugging 24/25 Exercise 1

Detailed instructions are in the exercise sheet. Following are your checkpoints:

- [x] Implement parser and evaluator
- [x] Implement a fuzzer
- [x] Generate *lots* of random instances with your fuzzer. Your evaluator and z3 must return the same result on generated instances
- [x] Provide detailed build instructions for your code so that we can evaluate it

Build instructions;
I used g++ version 11.4.0

```
g++ -std=c++17 -o parser parser.cpp
```

```
g++ -std=c++17 -o fuzzer fuzzer.cpp
```
