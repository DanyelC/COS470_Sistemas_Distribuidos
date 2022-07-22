build-coordinator:
	g++ -pthread -std=c++11 -o coordinator.out coordinator/coordinator.cpp funcs/*

build-process:
	g++ -pthread -std=c++11 -o process.out process/process.cpp funcs/*

build: build-process build-coordinator

run-coordinator: build-coordinator
	./coordinator.out

run-case0: build-process
	python3 process/run_study_cases.py

run-case1: build-process
	python3 process/run_study_cases.py 1

run-case2: build-process
	python3 process/run_study_cases.py 2

run-case3: build-process
	python3 process/run_study_cases.py 3

run-process: run-case0 run-case1 run-case2 run-case3

log-validator:
	python3 coordinator/log_validator.py