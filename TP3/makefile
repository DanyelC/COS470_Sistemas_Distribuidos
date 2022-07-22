build-coord:
	g++ -pthread -std=c++11 -o coordinator.out coordinator/coordinator.cpp funcs/*

build-process:
	g++ -pthread -std=c++11 -o process.out process/process.cpp funcs/*

build: build-coord build-process 

coordenador: build-coord
	./coordinator.out


cenario0: build-process
	python3 process/run_study_cases.py

processos: cenario0

cenario1: build-process
	python3 process/run_study_cases.py 1

cenario2: build-process
	python3 process/run_study_cases.py 2

cenario3: build-process
	python3 process/run_study_cases.py 3

log-validator:
	python3 coordinator/log_validator.py
