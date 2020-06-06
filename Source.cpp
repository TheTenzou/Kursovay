#include <vector> 
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

typedef vector<vector<int>> Dna;
typedef vector<pair<Dna, int>> Population;

struct sort_pred {
	bool operator()(const std::pair<Dna, int> &left, const std::pair<Dna, int> &right) {
		return left.second < right.second;
	}
};

struct sort_des {
	bool operator()(const std::pair<Dna, int> &left, const std::pair<Dna, int> &right) {
		return left.second > right.second;
	}
};

Dna propotionalSampling(Population population) {
	double sum = 0;
	for (int i = 0; i < population.size(); ++i) {
		sum += population[i].second;
	}

	vector<double> tmp;
	for (int i = 0; i < population.size(); ++i) {
		tmp.push_back(population[i].second / sum);
	}

	sort(population.begin(), population.end(), sort_pred());
	sort(tmp.begin(), tmp.end());

	vector<double> csum;
	csum.resize(population.size());
	csum[0] = tmp[0];
	for (int i = 1; i < csum.size(); ++i) {
		csum[i] = csum[i - 1] + tmp[i];
	}

	double r = (double)rand() / RAND_MAX;

	Dna result;

	for (int i = 0; i < csum.size(); ++i) {
		if (r <= csum[i]) {
			result = population[i].first;
			break;
		}
	}
	return result;
}


Dna backwardPropotionalSampling(Population population) {
	double sum = 0;
	double max = 0;
	for (int i = 0; i < population.size(); ++i) {
		if (population[i].second > max) {
			max = population[i].second;
		}
	}

	for (int i = 0; i < population.size(); ++i) {
		sum += max - population[i].second;
	}

	vector<double> tmp;
	for (int i = 0; i < population.size(); ++i) {
		tmp.push_back((max - population[i].second) / sum);
	}

	sort(population.begin(), population.end(), sort_des());
	sort(tmp.begin(), tmp.end());

	vector<double> csum;
	csum.resize(population.size());
	csum[0] = tmp[0];
	for (int i = 1; i < csum.size(); ++i) {
		csum[i] = csum[i - 1] + tmp[i];
	}

	double r = (double)rand() / RAND_MAX;
	//cout << r << endl;
	Dna result;

	for (int i = 0; i < csum.size(); ++i) {
		if (r <= csum[i]) {
			result = population[i].first;
			break;
		}
	}
	if (!result.size()) {
		return population[0].first;
	}
	return result;
}

Dna randomSampling(Population population) {
	int index = rand() % population.size();
	return population[index].first;
}

//фитнес функция
int fitness(vector<int> efficiency, Dna dna) {
	int res = 0;
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			res += efficiency[j] * dna[i][j];
		}
	}
	return res;
}

//затрачиваемые ресурсы
int resources(vector<int> resources, Dna dna) {
	int res = 0;
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			res += resources[j] * dna[i][j];
		}
	}
	return res;
}

//максимально загруженный день
int maxLoadDay(vector<int> load, Dna dna) {
	int result = 0;
	for (int i = 0; i < dna.size(); ++i) {
		int tmp = 0;
		for (int j = 0; j < dna[i].size(); ++j) {
			tmp += load[j] * dna[i][j];
		}
		if (tmp > result) {
			result = tmp;
		}
	}
	return result;
}

int timeIterval(vector<int> dayInterval, Dna dna) {	
	vector<int> tmpV(dayInterval.size(), -1);
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			if (dna[i][j]) {
				if (tmpV[j] == -1) {
					tmpV[j] = 0;
				}
				else {
					if (tmpV[j] < dayInterval[j]) {
						//return 0;
						return 1;
					}
					tmpV[j] = 0;
				}
			}
			else {
				++tmpV[j];
			}
		}
	}
	return 1;
}

// создание случайной днк
Dna initDna(int days, int works, vector<int> load, int loadTarget, vector<int> dayInterval) {
	vector<int> tmpV(dayInterval.size(), 360);
	Dna dna(days, vector<int>(works));
	for (int i = 0; i < dna.size(); ++i) {
		int tmp = 0;
		for (int j = 0; j < dna[i].size(); ++j) {
			if ((tmp + load[j] < loadTarget) && (tmpV[j] > dayInterval[j])) {
				dna[i][j] = (rand() % 100) < 70;
				if (dna[i][j]) {
					tmp += dna[i][j] * load[j];
					tmpV[j] = 0;
				}
			}
			else {
				++tmpV[j];
			}
		}
	}
	return dna;
}

// создание изначальной популяции
Population initPopulation(int populationSize, int days, int works, vector<int> load, int loadTarget, vector<int> dayInterval) {
	Population population;
	population.resize(populationSize);

	for (int i = 0; i < population.size(); ++i) {
		population[i] = make_pair(initDna(days, works, load, loadTarget, dayInterval), 0);
	}
	return population;
}

// скрешивание
Dna crosingover(Dna dna1, Dna dna2) {
	Dna newDna = dna1;
	int parts = 3;
	int currentPart = 1;

	/*int index = 0;
	for (; currentPart < parts;) {

		for (; index < newDna.size()*currentPart / 3; ++index) {
			newDna[index] = dna1[index];
		}
		currentPart++;

		for (; index < newDna.size()*currentPart / 3; ++index) {
			newDna[index] = dna2[index];
		}
		currentPart++;
	}*/
	// old selection
	for (int i = (rand() % dna1.size()); i < newDna.size(); ++i) {
		newDna[i] = dna2[i];
	}
	return newDna;
}

//мутации
Dna mutation(Dna dna) {
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			if ((rand() % 100) < 20) {
				dna[i][j] = (dna[i][j] == 0);
			}
		}
	}
	return dna;
}

Dna decriseResources(Dna dna) {
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			if (dna[i][j]) {
				if ((rand() % 100) < 20) {
					dna[i][j] = 0;
				}
			}
		}
	}
	return dna;
}

Dna addOnes(Dna dna) {
	for (int i = 0; i < dna.size(); ++i) {
		for (int j = 0; j < dna[i].size(); ++j) {
			if (!dna[i][j]) {
				if ((rand() % 100) < 2) {
					dna[i][j] = 1;
				}
			}
		}
	}
	return dna;
}

Dna mixUpDays(Dna dna) {
	int days = dna.size();
	int countOfMixUpDays = days * 0.3;
	for (int i = 0; i < countOfMixUpDays; ++i) {
		iter_swap(dna.begin() + (rand() % days), dna.begin() + (rand() % days));
	}
	return dna;
}

Population addRondom(Population population, int populationSize, int days, int works, vector<int> eff, vector<int> load, int loadTarget, vector<int> dayInterval) {
	while (population.size() < populationSize) {
		Dna tmp = initDna(days, works, load, loadTarget, dayInterval);
		population.push_back(make_pair(tmp, fitness(eff, tmp)));
	}
	return population;
}

Population selection(Population population, double proportionOfSelection, vector<int> eff) {
	int newDnaCount = population.size() * proportionOfSelection; //Количество новых особей

	Population newPopulation = population;

	for (int i = 0; i < newDnaCount; ++i) {
		Dna tmp = crosingover(propotionalSampling(population), propotionalSampling(population));
		newPopulation.push_back(make_pair(tmp, fitness(eff, tmp)));
	}
	return newPopulation;
}

Population generationMutation(Population population, double proportionOfMutation, vector<int> eff) {
	int mutatedDnaCount = population.size() * proportionOfMutation;

	Population newPopulation = population;

	for (int i = 0; i < mutatedDnaCount; ++i) {
		Dna tmp = mutation(randomSampling(population));
		newPopulation.push_back(make_pair(tmp, fitness(eff, tmp)));
	}
	return newPopulation;
}

Population mutateOverRes(Population population, vector<int> res, vector<int> eff, int resTarget) {
	for (int i = 0; i < population.size(); ++i) {
		if (resources(res, population[i].first) > resTarget) {
			population[i].first = decriseResources(population[i].first);
			population[i].second = fitness(eff, population[i].first);
		}
	}
	return population;
}

Population mutateUnderRes(Population population, vector<int> res, vector<int> eff, int resTarget) {
	for (int i = 0; i < population.size(); ++i) {
		if (resources(res, population[i].first) < resTarget) {
			population[i].first = addOnes(population[i].first);
			population[i].second = fitness(eff, population[i].first);
		}
	}
	return population;
}

Population mutateMixDays(Population population, vector<int> dayInterval) {
	for (int i = 0; i < population.size(); ++i) {
		if (!timeIterval(dayInterval, population[i].first)) {
			population[i].first = mixUpDays(population[i].first);
		}
	}
	return population;
}

Population removeExtra(Population population, int populationSize, vector<int> eff) {
	while (population.size() > populationSize) {
		Dna tmp = backwardPropotionalSampling(population);
		if (tmp.size() != 0) {
			population.erase(find(population.begin(), population.end(), make_pair(tmp, fitness(eff, tmp))));
		}
	}
	return population;
}

Population removeUsuitable(Population population, vector<int> res, int resTarget, vector<int> load, int loadTarget, vector<int> dayInterval) {
	for (int i = 0; i < population.size();) {
		if (resources(res, population[i].first) > resTarget) {
			//cout << 1 << endl;
			population.erase(population.begin() + i);
		}
		else if (maxLoadDay(load, population[i].first) > loadTarget){
			//cout << 2 << endl;
			population.erase(population.begin() + i);
		}
		else if (!timeIterval(dayInterval, population[i].first)) {
			//cout << 3 << endl;
			population.erase(population.begin() + i);
		}
		else {
			++i;
		}
	}
	return population;
}

Dna findMostFited(Population population) {
	int max = 0;
	int index = 0;
	for (int i = 0; i < population.size(); ++i) {
		if (population[i].second > max) {
			max = population[i].second;
			index = i;
		}
	}
	return population[index].first;
}


int howManyOverFited(Population population, vector<int> res, int targetRes) {
	int count = 0;
	for (int i = 0; i < population.size(); ++i) {
		if (resources(res, population[i].first) > targetRes) {
			++count;
		}
	}
	return count;
}

Dna solve(vector<int> load, int loadTarget, vector<int> resources, int resourcesTarget, vector<int> efficiency, int targetEfficiency, vector<int> dayInterval,
	int days, int works, double proportionOfSelection, double proportionOfMutation) {
	srand(time(0));
	int populationSize = 10; // популяция

	// Оценка популяции
	Population population = initPopulation(populationSize, days, works, load, loadTarget, dayInterval);
	for (int i = 0; i < population.size(); ++i) {
		population[i].second = fitness(efficiency, population[i].first);
	}

	Dna mostFited = findMostFited(population);
	Population mostFitedV;
	mostFitedV.push_back(make_pair(mostFited, fitness(efficiency, mostFited)));

	int maxGeneration = 100;
	int currentGeneraion = 0;
	
	while (currentGeneraion < maxGeneration) {
		//cout << "Generaton: " << currentGeneraion << " Efficiency: " << fitness(efficiency, mostFited) << " Size: " << population.size() << endl;
		//cout << 
		if (population.size() > populationSize) {
			population = removeExtra(population, populationSize, efficiency);
		}
		else {
			population = addRondom(population, populationSize, days, works, efficiency, load, loadTarget, dayInterval);
		}
		population = selection(population, proportionOfSelection, efficiency);

		population = generationMutation(population, proportionOfMutation, efficiency);


		/*population = mutateUnderRes(population, resources, efficiency, resourcesTarget);

		population = mutateOverRes(population, resources, efficiency, resourcesTarget);

		population = mutateMixDays(population, dayInterval);*/
	

		population = removeUsuitable(population, resources, resourcesTarget, load, loadTarget, dayInterval);
		//qDebug() << "Generaton: " << currentGeneraion << " Efficiency: " << fitness(efficiency, mostFited) << " Size: " << population.size() << endl;
		//ui->statusbar->showMessage("Поколение" + QString::number(currentGeneraion));
	
		mostFited = findMostFited(population);
		mostFitedV.push_back(make_pair(mostFited, fitness(efficiency, mostFited)));
		if (fitness(efficiency, mostFited) > targetEfficiency) {
			return mostFited;
		}
	
		currentGeneraion++;
		cout << "Generaton: " << currentGeneraion << " Efficiency: " << fitness(efficiency, mostFited) << " Size: " << population.size() << endl;
	}
	

	int max = 0;
	int index = 0;
	for (int i = 0; i < mostFitedV.size(); ++i) {
		if (mostFitedV[i].second > max) {
			max = mostFitedV[i].second;
			index = i;
		}
	}
	return mostFitedV[index].first;
	//return mostFited;
}


int main() {
	vector<int> dif = { 1, 1, 1, 2, 2, 3, 3, 3, 2, 1, 2, 1, 2, 2, 2, 2, 1, 2 };
	vector<int> len = { 1, 1, 1, 2, 3, 4, 5, 2, 2, 1, 3, 1, 2, 2, 3, 3, 2, 4 };
	vector<int> res = { 1, 1, 1, 2, 1, 2, 4, 3, 2, 1, 2, 1, 3, 2, 3, 3, 2, 5 };
	vector<int> eff = { 2, 2, 2, 1, 3, 4, 5, 6, 1, 2, 3, 4, 2, 1, 2, 3, 1, 5 };
	vector<int> ti = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

	//Dna ans = solve(dif, len, 8, res, 50, eff, 160, ti, 5, 18, 0.3, 0.3);
	Dna ans = solve(len, 8, res, 50, eff, 160, ti, 5, 18, 0.3, 0.7);

	/*s
	for (int i = 0; i < ans.size(); ++i) {
		for (int j = 0; j < ans[i].size(); ++j) {
			cout << ans[i][j] << ' ';
		}
		cout << endl;
	}
	*/
	setlocale(LC_ALL, "rus");
	cout << endl << "Макимальная продолдительность за день: " << maxLoadDay(len, ans) << endl;
	cout << "Суммарные ресурсы за весь промежуток: " << resources(res, ans) << endl;
	cout << "Сумарная эффективность за весь промежуток: " << fitness(eff, ans) << endl;

	ofstream file;
	file.open("answer.txt");
	for (int i = 0; i < ans[0].size(); ++i) {
		for (int j = 0; j < ans.size(); ++j) {
			file << ans[j][i] << '	';
		}
		file << endl;
	}

	system("pause");
	return 0;
}