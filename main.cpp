#include <iostream>
#include <vector>
#include <random>
#include <fstream>
using namespace std;

// The project contains input.txt file in the cmake folder, if you want to add input file add it to the cmake
// and enter its name (input) with .txt at the end

char generateRandomGene()
{
    return ((rand()%2) + '0');
}

int generateRandom(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

vector<string> generate(int populationSize, int chromosomeSize)
{
    vector<string> population(populationSize + 1);
    for(int i = 1; i <= populationSize; i++)
    {
        string chromosome = "#";
        for(int k = 1; k <= chromosomeSize; k++)
        {
            chromosome += generateRandomGene();
        }
        population[i] = chromosome;
    }
    return population;
}

int calculateChromosomeFitness(string chromosome, vector<int> execTime)
{
    int firstCore = 0, secondCore = 0;
    for(int i = 1; i < chromosome.size(); i++)
    {
        if(chromosome[i] == '1')
        {
            firstCore += execTime[i];
        }
        else
        {
            secondCore += execTime[i];
        }
    }
    return max(firstCore, secondCore);
}

vector<int> calculateFitness(int fitnessReverse, vector<string> population, vector<int> execTime)
{
    int size = population.size();
    vector<int> answer(size + 1);

    for(int i = 1; i < size; i++)
    {
        answer[i] = fitnessReverse - calculateChromosomeFitness(population[i], execTime);
    }

    return answer;
}

string selectParent(vector<int> &fitness, vector<string> &population)
{
    int total = 0, size = fitness.size();
    for(int i = 1; i <= size; i++)
    {
        total += fitness[i];
    }

    int random = generateRandom(0, total);
    int counter = 0, selected = -1;
    string parent;
    for(int i = 1; i < size; i++)
    {
        counter += fitness[i];
        if(counter >= random)
        {
            parent = population[i];
            selected = i;
            break;
        }
    }

    fitness.erase(fitness.begin() + selected);
    population.erase(population.begin() + selected);
    return parent;
}

int crossOver(string &parent1, string &parent2, vector<int> execTime, int maxTime)
{
    int size = parent1.size();
    int point = generateRandom(1, size - 1);
    for(int i = point + 1; i < size; i++)
    {
        swap(parent1[i], parent2[i]);
    }

    int parent1Time = calculateChromosomeFitness(parent1, execTime);
    int parent2Time = calculateChromosomeFitness(parent2, execTime);

    return max(parent1Time, parent2Time) > maxTime ? -1 : 1;
}

int mutate(string &chromosome, vector<int> execTime, int maxTime)
{
    int point = generateRandom(1, chromosome.size() - 1);
    if(chromosome[point] == '1')
    {
        chromosome[point] = '0';
    }
    else
    {
        chromosome[point] = '1';
    }

    int time = calculateChromosomeFitness(chromosome, execTime);

    return time > maxTime ? -1 : 1;
}

pair<int, int> findBest(vector<string> population, vector<int> execTime, int fitnessReverse)
{
    vector<int> fitness = calculateFitness(fitnessReverse, population, execTime);

    int index = -1, max = -1e5, score = -1;
    for(int i = 1; i < fitness.size(); i++)
    {
        if(max <= fitness[i])
        {
            index = i;
            max = fitness[i];
            score = fitnessReverse - fitness[i];
        }
    }
    return {index, score};
}

void calculate(int testCase, int maxTime, int chromosomeSize, vector<int> genesTime)
{
    // Constants for making the solution more accurate
    int populationSize = 50;
    int generations = 100;
    int fitnessReverse = 1e5;

    vector<string> population = generate(populationSize, chromosomeSize);

    while(generations--)
    {
        vector<int> fitness = calculateFitness(fitnessReverse, population, genesTime);
        string parent1 = selectParent(fitness, population);
        string parent2 = selectParent(fitness, population);
        string spareParent1 = parent1, spareParent2 = parent2;
        int maxCnt = 100;
        int goodCrossOver = -1;
        while(goodCrossOver == -1 && maxCnt--)
        {
            parent1 = spareParent1;
            parent2 = spareParent2;
            goodCrossOver = crossOver(parent1, parent2, genesTime, maxTime);
        }
        spareParent1 = parent1;
        spareParent2 = parent2;
        int goodMutation = -1;
        maxCnt = 100;
        while(goodMutation == -1 && maxCnt--)
        {
            parent1 = spareParent1;
            goodMutation = mutate(parent1, genesTime, maxTime);
        }
        goodMutation = -1;
        maxCnt = 100;
        while(goodMutation == -1 && maxCnt--)
        {
            parent2 = spareParent2;
            goodMutation = mutate(parent2, genesTime, maxTime);
        }
        population.push_back(parent1);
        population.push_back(parent2);
    }

    cout << "Test Case " << testCase << ":" << endl << endl;

    pair<int, int> best = findBest(population, genesTime, fitnessReverse);
    cout << "Best Solution: " << best.second << endl << endl;
    population[best.first].erase(population[best.first].begin());
    cout << "Chromosome Representation: " << population[best.first] << endl << endl;
    vector<int> firstCore, secondCore;
    int firstCoreTime = 0, secondCoreTime = 0;
    for(int i = 0; i < population[best.first].size(); i++)
    {
        if(population[best.first][i] == '1')
        {
            firstCore.push_back(i + 1);
            firstCoreTime += genesTime[i + 1];
        }
        else
        {
            secondCore.push_back(i + 1);
            secondCoreTime += genesTime[i + 1];
        }
    }
    cout << "First Core Total Time: " << firstCoreTime << endl;
    cout << "First Core Assigned Tasks: " << endl;
    for(auto i : firstCore)
    {
        cout << "Task #" << i << endl;
    }
    cout << endl;
    cout << "Second Core Total Time: " << secondCoreTime << endl;
    cout << "Second Core Assigned Tasks: " << endl;
    for(auto i : secondCore)
    {
        cout << "Task #" << i << endl;
    }
    cout << endl;
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "Enter File Name: ";
    string fileName;
    cin >> fileName;
    cout << endl;
    int testCases;
    fstream file(fileName);
    file >> testCases;
    for(int i = 1; i <= testCases; i++)
    {
        int maxTime, chromosomeSize;
        file >> maxTime;
        file >> chromosomeSize;
        vector<int> tasks(chromosomeSize + 1);
        for(int k = 1; k <= chromosomeSize; k++)
        {
            file >> tasks[k];
        }
        calculate(i, maxTime, chromosomeSize, tasks);
        string endln;
        getline(file,endln);
    }
    file.close();
    return 0;
}
