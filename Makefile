
CC=g++

CFLAGS=-c
all: search cluster

search: SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o
	$(CC) -o search SearchMain.o LSHash.o HyperCube.o TableF.o Neighbours.o IdDistancePair.o Helpers.o VectorElement.o CurveElement.o

cluster: ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o
	$(CC) -o cluster -O2 ClusterMain.o KMeans.o Cluster.o VectorElement.o LSHash.o HyperCube.o TableF.o Neighbours.o Helpers.o IdDistancePair.o

SearchMain.o: SearchMain.cpp
	$(CC) $(CFLAGS) SearchMain.cpp

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
	$(CC) $(CFLAGS) ClusterMain.cpp

KMeans.o: KMeans.cpp
	$(CC) $(CFLAGS) KMeans.cpp

Cluster.o: Cluster.cpp
	$(CC) $(CFLAGS) Cluster.cpp

clean:
	rm -rf *o search cluster