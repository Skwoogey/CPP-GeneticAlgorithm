// GeneticAlgorithm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define sqr(x) (x)*(x)

#define frand(i) (float)rand()/RAND_MAX * length[i%3] + offset[i%3]
#define irand(i) rand() % length[i%3] + offset[i%3]

#define usedrand(i) irand(i)


#define FUNC_LENGTH 200
#define GENES_QUANTITY 15
#define MUTATION_RATE 5

#define POPULATION_SIZE 100

#define GAUSS_FUNC_QUANTITY GENES_QUANTITY/3
#define GENES_TO_MUTATE POPULATION_SIZE*GENES_QUANTITY*MUTATION_RATE/100

using namespace std;

double GF[FUNC_LENGTH];

int length[3] = { 2000, FUNC_LENGTH, 5 };
int offset[3] = { 500, 0, 1 };


double func(double *genes, double x)
{
	double res = 0;
	for (int i = 0; i < GAUSS_FUNC_QUANTITY; i++)
	{
		res += genes[3 * i] * exp(-sqr((x - genes[3 * i + 1]) / genes[3 * i + 2]));
	}

	return res;
}

class individual
{
	double genes[GENES_QUANTITY];
	double fitness;

public:
	individual()
	{
		for (int i = 0; i < GENES_QUANTITY; i++)
		{
			genes[i] = usedrand(i);
		}	

		FF();
	}

	individual(const individual &mate1, const individual &mate2)
	{
		for (int i = 0; i < GENES_QUANTITY; i++)
		{
			if (rand() % 2)
				genes[i] = mate1.genes[i];
			else
				genes[i] = mate2.genes[i];
		}

		FF();
	}

	void FF()
	{
		fitness = 0;
		double TF;
		for (int i = 0; i < FUNC_LENGTH; i++)
		{
			TF = func(genes, i);
			fitness += fabs(GF[i] - TF);
		}
	}

	double get_fitness()
	{
		return fitness;
	}

	void mutate()
	{
		int gene = rand() % GENES_QUANTITY;
		genes[gene] = usedrand(gene);
		/*
		{
			genes[gene] *= ((frand() - 0.5)* exp((fitness - 2000)/1500) + 1);

			if (genes[gene] < offset[gene % 3])
				genes[gene] = offset[gene % 3];

			if (genes[gene] > offset[gene % 3] + length[gene % 3])
				genes[gene] = offset[gene % 3] + length[gene % 3];
		}
		*/

		FF();
	}

	void show()
	{
		for (int i = 0; i < GAUSS_FUNC_QUANTITY; i++)
		{
			cout << "A" << i << ": " << genes[3*i] << endl;
			cout << "Phi" << i << ": " << genes[3*i + 1] << endl;
			cout << "p" << i << ": " << genes[3*i + 2] << endl;
		}

		cout << "fitness: " << fitness << endl;
	}

	inline bool operator> (const individual& r)
	{
		return (fitness > r.fitness);
	}

	inline bool operator< (const individual& r)
	{
		return (fitness < r.fitness);
	}
};

class GA
{
	vector<individual> population;
	double GoalFitness = 5;
	unsigned int gen = 0;

	void sort()
	{
		std::sort(population.begin(), population.end());
	}

	void crossover()
	{
		population.erase(population.begin() + POPULATION_SIZE/2, population.end());
		
		for (int i = 0; i < POPULATION_SIZE / 2; i++)
			population.push_back(individual(population[rand() % (POPULATION_SIZE / 2)], population[rand() % (POPULATION_SIZE / 2)]));
	}

	void mutate()
	{
		for (int i = 0; i < GENES_TO_MUTATE; i++)
		{
			population[rand() % POPULATION_SIZE].mutate();
		}
	}

public:

	GA()
	{
		for (int i = 0; i < POPULATION_SIZE; i++)
		{
			individual temp;
			population.push_back(temp);
		}

		sort();
	}

	void NextGen()
	{
		gen++;

		crossover();
		mutate();
		sort();
	}

	int fit_enough()
	{
		return (population[0].get_fitness() < GoalFitness);
	}

	void show()
	{
		cout << "generation:" << gen << endl;
		//cout << "population size:" << population.size() << endl;
		cout << "best individual:\n";
		population[0].show();

		//cout << "worst individual:\n";
		//population[POPULATION_SIZE-1].show();

		cout << "\n";
	}
};

HANDLE ghMutex = CreateMutex(
	NULL,
	FALSE,
	NULL);

DWORD WINAPI myThread(LPVOID lpParameter)
{
	GA* pop = (GA*)lpParameter;

	while (1)
	{
		WaitForSingleObject(ghMutex, INFINITE);

		pop->show();

		ReleaseMutex(ghMutex);
			
		Sleep(1000);
	}

	return 0;
}

int main()
{
	srand(time(NULL));
	double GP[GENES_QUANTITY] = {
		1000, 20, 1,
		1500, 40, 1,
		2000, 60, 1,
		2500, 80, 1,
		3000, 100, 1,
	};

	cout << GENES_TO_MUTATE << endl;

	
	for (int i = 0; i < GENES_QUANTITY; i++)
	{
		GP[i] = usedrand(i);
	}
	

	for (int i = 0; i < FUNC_LENGTH; i++)
		GF[i] = func(GP, i);

	GA pop;

	DWORD myThreadID;
	//HANDLE myHandle = CreateThread(0, 0, myThread, &pop , 0, &myThreadID);

	unsigned int gens = 0;

	while(1)
	{
		gens++;

		//WaitForSingleObject(ghMutex, INFINITE);

		pop.NextGen();

		if (gens % 1000 == 0)
			pop.show();

		if (pop.fit_enough())
			break;

		//ReleaseMutex(ghMutex);
	}

	//CloseHandle(myHandle);

	pop.show();
	cout << "total generations: " << gens << endl;

	system("pause");

    return 0;
}

