#include <iostream>
#include <stddef.h>
#include "mpi.h"
#include <ctime>
#define N 10000000

struct time_def {
    double time_calc;
    double def_of_PI;
};

struct dot {
    double x;
    double y;
};

bool IsPointInCircle(double R, double x, double y)
{
    return ((x * x + y * y) < R * R);
}

time_def MonteCarloMethod(int Npoints, dot mass[]) {
    time_t start = clock();
    int Nincircle = 0;
    srand(time(0));
    for (int i = 0; i < Npoints; i++)
    {
        if (IsPointInCircle(1.0, mass[i].x, mass[i].y)) {
            Nincircle++;
        }
    }
    time_t end = clock();
    time_def result;
    result.time_calc = end - start;
    result.def_of_PI = Nincircle * 4.0 / Npoints;

    return result;
}

int main(int argc, char** argv)
{
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype dotStructureType;
    int len[3] = { 1, 1, 1 };
    MPI_Aint pos[3] = { offsetof(dot, x), offsetof(dot, y), sizeof(dot) };
    MPI_Datatype typ[3] = { MPI_DOUBLE,MPI_DOUBLE, MPI_UB };
    MPI_Type_create_struct(3, len, pos, typ, &dotStructureType);
    MPI_Type_commit(&dotStructureType);

    MPI_Datatype time_defStructureType;
    int len1[3] = { 1, 1, 1 };
    MPI_Aint pos1[3] = { offsetof(time_def, time_calc), offsetof(time_def, def_of_PI), sizeof(time_def) };
    MPI_Datatype typ1[3] = { MPI_DOUBLE,MPI_DOUBLE, MPI_UB };
    MPI_Type_create_struct(3, len1, pos1, typ1, &time_defStructureType);
    MPI_Type_commit(&time_defStructureType);

    if (rank == 0) {
        dot* mass = new dot[N];
        for (int i = 0; i < N; i++) {
            double x = rand();
            double y = rand();
            mass[i].x = x / RAND_MAX;
            mass[i].y = y / RAND_MAX;
        }
        for (int i = 1; i < size; i++) {
            MPI_Send(mass, N / (size-1) * (i), dotStructureType, i, 0, MPI_COMM_WORLD);
        }
    }

    if (rank != 0) {
        dot* mass = new dot[N / (size-1) * (rank)];
        MPI_Recv(mass, N / (size-1) * (rank), dotStructureType, 0, 0, MPI_COMM_WORLD, &status);
        time_def info[1] = { MonteCarloMethod(N / (size - 1) * (rank), mass) };
        MPI_Send(info, 1, time_defStructureType, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
 
    if (rank == 0) {
        time_def info[1];
        for (int i = 1; i < size; i++) {
            MPI_Recv(info, 1, time_defStructureType, i, 1, MPI_COMM_WORLD, &status);
            std::cout << "Process #" << i << " with " << N / (size - 1) * (i) <<" number of dots return PI = " << info[0].def_of_PI
                << " with time " << info[0].time_calc << " milliseconds\n";
        }
    }
    MPI_Finalize();
    return 0;
}