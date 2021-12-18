
CC=g++

CFLAGS=-c
CU=-lcppunit
all: search cluster utest

search: SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o DiscreteFrechet.o config.o curve.o frechet.o interval.o point.o simplification.o
	$(CC) -o search SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o DiscreteFrechet.o config.o curve.o frechet.o interval.o point.o simplification.o -O2 -ggdb3

cluster: ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o CurveElement.o DiscreteFrechet.o TreeNode.o config.o curve.o frechet.o interval.o point.o simplification.o
	$(CC) -o cluster ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o CurveElement.o DiscreteFrechet.o TreeNode.o config.o curve.o frechet.o interval.o point.o simplification.o -O2  -ggdb3

utest: UnitTestMain.o TableF.o Helpers.o IdDistancePair.o config.o curve.o frechet.o interval.o point.o simplification.o
	$(CC) -o utest UnitTestMain.o TableF.o Helpers.o IdDistancePair.o config.o curve.o frechet.o interval.o point.o simplification.o $(CU) -ldl

UnitTestMain.o: UnitTestMain.cpp
	$(CC) $(CFLAGS) $(CU) -ldl UnitTestMain.cpp

SearchMain.o: SearchMain.cpp
	$(CC) $(CFLAGS) -O2 -ggdb3 SearchMain.cpp

DiscreteFrechet.o: DiscreteFrechet.cpp
	$(CC) $(CFLAGS) -O2 DiscreteFrechet.cpp

LSHash.o: LSHash.cpp
	$(CC) $(CFLAGS) LSHash.cpp

Neighbours.o: Neighbours.cpp
	$(CC) $(CFLAGS) Neighbours.cpp

IdDistancePair.o: IdDistancePair.cpp
	$(CC) $(CFLAGS) IdDistancePair.cpp

Helpers.o: Helpers.cpp
	$(CC) $(CFLAGS) -O2 Helpers.cpp

VectorElement.o: VectorElement.cpp
	$(CC) $(CFLAGS) VectorElement.cpp

CurveElement.o: CurveElement.cpp
	$(CC) $(CFLAGS) CurveElement.cpp

HyperMain.o: HyperMain.cpp
	$(CC) $(CFLAGS) HyperMain.cpp

HyperCube.o: HyperCube.cpp
	$(CC) $(CFLAGS) HyperCube.cpp

TableF.o: TableF.cpp
	$(CC) $(CFLAGS) TableF.cpp

ClusterMain.o: ClusterMain.cpp
	$(CC) $(CFLAGS) -O2 -ggdb3 ClusterMain.cpp

KMeans.o: KMeans.cpp
	$(CC) $(CFLAGS) -O2 KMeans.cpp

Cluster.o: Cluster.cpp
	$(CC) $(CFLAGS) Cluster.cpp

TreeNode.o: TreeNode.cpp
	$(CC) $(CFLAGS) -O2 TreeNode.cpp

config.o: ./fred/config.cpp
	$(CC) $(CFLAGS) ./fred/config.cpp

curve.o: ./fred/curve.cpp
	$(CC) $(CFLAGS) ./fred/curve.cpp

frechet.o: ./fred/frechet.cpp
	$(CC) $(CFLAGS) ./fred/frechet.cpp

interval.o: ./fred/interval.cpp
	$(CC) $(CFLAGS) ./fred/interval.cpp

point.o: ./fred/point.cpp
	$(CC) $(CFLAGS) ./fred/point.cpp

simplification.o: ./fred/simplification.cpp
	$(CC) $(CFLAGS) ./fred/simplification.cpp

clean:
	rm -rf *o search cluster utest
