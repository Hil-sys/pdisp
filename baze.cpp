#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>

// Класс Process
class Process {
public:
    int id;
    int required_resources; // Общее количество ресурсов, необходимое для завершения
    int allocated_resources; // Текущий запас ресурсов у процесса

    Process(int id_, int req) : id(id_), required_resources(req), allocated_resources(0) {}

    bool isCompleted() const {
        return allocated_resources >= required_resources;
    }

    int remainingResources() const {
        return required_resources - allocated_resources;
    }
};

// Класс ResourceManager
class ResourceManager {
public:
    int total_resources; // Общий пул ресурсов
    std::vector<Process> processes;

    ResourceManager(int total, const std::vector<Process>& procs)
        : total_resources(total), processes(procs) {}

    // Проверка, есть ли еще процессы, которые не завершены
    bool hasActiveProcesses() const {
        for (const auto& p : processes) {
            if (!p.isCompleted()) {
                return true;
            }
        }
        return false;
    }

    // Проверка, есть ли достаточно ресурсов для процесса
    bool canAllocate(const Process& p, int request) const {
        return (request <= total_resources) && (request <= p.remainingResources());
    }

    // Выдача ресурсов процессу
    void allocateResources(int processIndex, int request) {
        processes[processIndex].allocated_resources += request;
        total_resources -= request;
    }

    // Вывод текущего состояния
    void printStatus() const {
        std::cout << "\nТекущий статус ресурсов:\n";
        std::cout << "Общий пул ресурсов: " << total_resources << "\n";
        for (const auto& p : processes) {
            std::cout << "Процесс " << p.id
                << " — выделено: " << p.allocated_resources
                << " / требуется: " << p.required_resources
                << " — осталось: " << p.remainingResources()
                << (p.isCompleted() ? " (завершен)" : "") << "\n";
        }
        std::cout << std::endl;
    }
};

// Вспомогательная функция для получения случайного числа
int getRandomNumber(int min, int max) {
    static std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Функция для обработки пользовательского ввода
bool getUserDecision() {
    std::string input;
    while (true) {
        std::cout << "Выдать ресурсы? (Yes/No): ";
        std::getline(std::cin, input);
        // Приведение к нижнему регистру
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        if (input == "yes") return true;
        if (input == "no") return false;
        std::cout << "Пожалуйста, введите 'Yes' или 'No'.\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    int numProcesses;
    int totalResources;

    // Ввод количества процессов
    std::cout << "Введите количество процессов: ";
    std::cin >> numProcesses;
    std::cin.ignore(); // очистка буфера

    // Ввод общего количества ресурсов
    std::cout << "Введите общее количество ресурсов: ";
    std::cin >> totalResources;
    std::cin.ignore();

    std::vector<Process> processes;
    // Ввод требований каждого процесса
    for (int i = 0; i < numProcesses; ++i) {
        int req;
        std::cout << "Введите требуемое количество ресурсов для процесса " << i + 1 << ": ";
        std::cin >> req;
        std::cin.ignore();
        processes.emplace_back(i + 1, req);
    }

    ResourceManager manager(totalResources, processes);

    // Основной цикл
    while (manager.hasActiveProcesses() && manager.total_resources > 0) {
        

        for (size_t i = 0; i < manager.processes.size(); ++i) {
            auto& process = manager.processes[i];

            if (process.isCompleted()) continue; // пропускаем завершенные процессы

            int maxRequest = process.remainingResources();
            if (maxRequest == 0) continue; // уже завершен

            int request = getRandomNumber(1, maxRequest);

            std::cout << "Процесс " << process.id << " запрашивает " << request << " ресурсов.\n";

            if (manager.canAllocate(process, request)) {
                if (getUserDecision()) {
                    manager.allocateResources(i, request);
                    std::cout << "Ресурсы выданы.\n";
                    manager.printStatus();
                }
                else {
                    std::cout << "Ресурсы не выданы.\n";
                    manager.printStatus();
                }
            }
            else {
                std::cout << "Недостаточно ресурсов для выдачи.\n";
            }

            // Проверка, завершился ли процесс
            if (process.isCompleted()) {
                std::cout << "Процесс " << process.id << " завершен.\n";
            }

            // Проверка условия выхода
            if (!manager.hasActiveProcesses() || manager.total_resources == 0) {
                break;
            }
        }
    }

    std::cout << "\nРабота завершена.\n";
    manager.printStatus();

    return 0;
}
