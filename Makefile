DEBUG = -pg
OPENMP = -openmp

othello: othello.cpp
	icpc $(DEBUG) $(OPENMP) othello.cpp -o othello

clean:
	rm othello