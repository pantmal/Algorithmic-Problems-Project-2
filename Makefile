
CC=g++

CFLAGS=-c
CU=-lcppunit
all: search cluster utest

search: SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o DiscreteFrechet.o
	$(CC) -o search SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o DiscreteFrechet.o -O2

cluster: ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o CurveElement.o DiscreteFrechet.o TreeNode.o
	$(CC) -o cluster ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o CurveElement.o DiscreteFrechet.o TreeNode.o -O2

utest: UnitTestMain.o TableF.o Helpers.o IdDistancePair.o
	$(CC) -o utest UnitTestMain.o TableF.o Helpers.o IdDistancePair.o $(CU) -ldl

UnitTestMain.o: UnitTestMain.cpp
	$(CC) $(CFLAGS) $(CU) -ldl UnitTestMain.cpp

SearchMain.o: SearchMain.cpp
	$(CC) $(CFLAGS) -O2 SearchMain.cpp

DiscreteFrechet.o: DiscreteFrechet.cpp
	$(CC) $(CFLAGS) -O2 DiscreteFrechet.cpp

LSHash.o: LSHash.cpp
	$(CC) $(CFLAGS) LSHash.cpp

Neighbours.o: Neighbours.cpp
	$(CC) $(CFLAGS) Neighbours.cpp

IdDistancePair.o: IdDistancePair.cpp
	$(CC) $(CFLAGS) IdDistancePair.cpp

Helpers.o: Helpers.cpp
	$(CC) $(CFLAGS) Helpers.cpp

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
	$(CC) $(CFLAGS) -O2 ClusterMain.cpp

KMeans.o: KMeans.cpp
	$(CC) $(CFLAGS) -O2 KMeans.cpp

Cluster.o: Cluster.cpp
	$(CC) $(CFLAGS) Cluster.cpp

TreeNode.o: TreeNode.cpp
	$(CC) $(CFLAGS) -O2 TreeNode.cpp

clean:
	rm -rf *o search cluster utest
