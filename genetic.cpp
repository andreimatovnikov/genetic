#include <iostream>
#include <vector>
#include <random>

struct Item {
    int value, weight;
};

int POP_SIZE = 100;
int GENERATIONS = 200;
double CROSSOVER_RATE = 0.8;
double MUTATION_RATE = 0.05;

std::vector<Item> items;
int capacity;

std::mt19937 generator(time(nullptr));

struct Chromosome {
    std::vector<bool> genes;
    int fitness;

    Chromosome(int n) : genes(n), fitness(0) {}

    void evaluate() {
        int totalWeight = 0, totalValue = 0;
        for (size_t i = 0; i < genes.size(); ++i) {
            if (genes[i]) {
                totalWeight += items[i].weight;
                totalValue += items[i].value;
            }
        }
        fitness = (totalWeight <= capacity) ? totalValue : 0;
    }
};

Chromosome randomChromosome(int n) {
    Chromosome ch(n);
    for (int i = 0; i < n; ++i)
        ch.genes[i] = generator() % 2;
    ch.evaluate();
    return ch;
}

Chromosome tournamentSelection(std::vector<Chromosome>& population) {
    int a = generator() % POP_SIZE;
    int b = generator() % POP_SIZE;
    return (population[a].fitness > population[b].fitness) ? population[a] : population[b];
}

std::pair<Chromosome, Chromosome> crossover(const Chromosome& p1, const Chromosome& p2) {
    int n = p1.genes.size();
    std::uniform_int_distribution<int> dist(1, n - 2);
    int point = dist(generator);

    Chromosome ch1 = p1, ch2 = p2;
    for (int i = point; i < n; ++i) {
        swap(ch1.genes[i], ch2.genes[i]);
    }
    ch1.evaluate();
    ch2.evaluate();
    return std::pair<Chromosome, Chromosome>(ch1, ch2);
}

void mutate(Chromosome& c) {
    for (bool bit : c.genes) {
        if ((double)generator() / generator.max() < MUTATION_RATE) {
            bit = !bit;
        }
    }
    c.evaluate();
}

int main() {
    int n;
    std::cin >> n;
    std::cin >> capacity;

    items.resize(n);

    for (int i = 0; i < n; ++i) {
        std::cin >> items[i].value >> items[i].weight;
    }

    std::vector<Chromosome> population;
    for (int i = 0; i < POP_SIZE; ++i)
        population.push_back(randomChromosome(n));

    Chromosome best = population[0];

    for (int gen = 0; gen < GENERATIONS; ++gen) {
        std::vector<Chromosome> new_population;

        while (new_population.size() < POP_SIZE) {
            Chromosome parent1 = tournamentSelection(population);
            Chromosome parent2 = tournamentSelection(population);

            std::pair<Chromosome, Chromosome> children = { parent1, parent2 };

            if ((double)generator() / generator.max() < CROSSOVER_RATE)
                children = crossover(parent1, parent2);

            mutate(children.first);
            mutate(children.second);

            new_population.push_back(children.first);
            new_population.push_back(children.second);
        }

        population = new_population;

        for (const auto& c : population) {
            if (c.fitness > best.fitness)
                best = c;
        }
    }

    std::cout << best.fitness << "\n";

    for (size_t i = 0; i < best.genes.size(); ++i)
        if (best.genes[i])
            std::cout << i << " ";
    std::cout << std::endl;

    return 0;
}