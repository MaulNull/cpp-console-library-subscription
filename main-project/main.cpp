#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// Структура для хранения информации о докладе
struct Report {
    std::string start_time;   // формат "чч:мм"
    std::string end_time;     // формат "чч:мм"
    std::string surname;
    std::string name;
    std::string patronymic;
    std::string topic;

    // Время начала и конца в минутах от начала суток для удобства вычислений
    int start_minutes;
    int end_minutes;

    // Длительность в минутах
    int duration() const {
        return end_minutes - start_minutes;
    }
};

// Функция для преобразования времени "чч:мм" в минуты от начала суток
int timeToMinutes(const std::string& time_str) {
    int hh, mm;
    sscanf_s(time_str.c_str(), "%d:%d", &hh, &mm);
    return hh * 60 + mm;
}

// Функция для парсинга строки файла в структуру Report
bool parseLine(const std::string& line, Report& report) {
    // Формат строки:
    // начало конец фамилия имя отчество тема
    // где тема может содержать пробелы, поэтому нужно аккуратно разбивать

    std::istringstream iss(line);
    std::string start_time_str, end_time_str;

    if (!(iss >> start_time_str >> end_time_str >> report.surname >> report.name >> report.patronymic))
        return false;

    // Остаток строки - тема доклада (может содержать пробелы)
    std::getline(iss, report.topic);

    // Удаляем ведущие пробелы у темы
    if (!report.topic.empty() && report.topic[0] == ' ')
        report.topic.erase(0, 1);

    report.start_time = start_time_str;
    report.end_time = end_time_str;

    report.start_minutes = timeToMinutes(start_time_str);
    report.end_minutes = timeToMinutes(end_time_str);

    return true;
}

// Чтение данных из файла
std::vector<Report> readData(const std::string& filename) {
    std::vector<Report> data;
    std::ifstream fin(filename);

    if (!fin.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << "\n";
        return data;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        Report rep;
        if (parseLine(line, rep))
            data.push_back(rep);
        // Можно добавить обработку ошибок разбора
    }

    return data;
}

// Сортировка слиянием (Merge sort)
void merge(std::vector<Report>& arr, int left, int mid, int right,
    bool (*comp)(const Report&, const Report&)) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Report> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Report> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (comp(L[i], R[j])) {
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(std::vector<Report>& arr, int left, int right,
    bool (*comp)(const Report&, const Report&)) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid, comp);
    mergeSort(arr, mid + 1, right, comp);

    merge(arr, left, mid, right, comp);
}

// Пирамидальная сортировка (Heap sort)
void heapify(std::vector<Report>& arr, int n, int i,
    bool (*comp)(const Report&, const Report&)) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && comp(arr[l], arr[largest]))
        largest = l;

    if (r < n && comp(arr[r], arr[largest]))
        largest = r;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest, comp);
    }
}

void heapSort(std::vector<Report>& arr,
    bool (*comp)(const Report&, const Report&)) {
    int n = arr.size();

    // Построение кучи
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i, comp);

    // Извлечение элементов из кучи
    for (int i = n - 1; i >= 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0, comp);
    }
}

// Критерии сравнения

// Для сортировки по убыванию длительности доклада
bool compareDurationDesc(const Report& a, const Report& b) {
    return a.duration() > b.duration();
}

// Для сортировки по фамилии автора и теме по возрастанию
bool compareAuthorTopicAsc(const Report& a, const Report& b) {
    if (a.surname != b.surname)
        return a.surname < b.surname;
    else
        return a.topic < b.topic;
}

// Основная программа
int main() {
    setlocale(LC_ALL, "Russian");
    std::string filename;
    std::cout << "Вариант 2: Программа конференции. Выполнил Федотов Владислав" << std::endl;
    std::cout << "Введите имя файла с данными: ";
    std::getline(std::cin, filename);

    auto data = readData(filename);

    // Вывод всех докладов Иванова Ивана Ивановича
    std::cout << "\nВсе доклады Иванова Ивана Ивановича:\n";
    for (const auto& rep : data) {
        if (rep.surname == "Иванов" && rep.name == "Иван" && rep.patronymic == "Иванович") {
            std::cout << rep.start_time << " " << rep.end_time << " "
                << rep.surname << " " << rep.name << " " << rep.patronymic << " "
                << rep.topic << "\n";
        }
    }

    // Вывод всех докладов длительностью больше 15 минут
    auto long_reports = data;

    // Сортируем по убыванию длительности с помощью пирамидальной сортировки
    heapSort(long_reports, &compareDurationDesc);

    std::cout << "\nДоклады длительностью более 15 минут:\n";
    for (const auto& rep : long_reports) {
        if (rep.duration() > 15) {
            std::cout << rep.start_time << " " << rep.end_time << " "
                << rep.surname << " " << rep.name << " " << rep.patronymic << " "
                << rep.topic << "\n";
        }
    }
    // Сортировка по убыванию длительности методом слияния
    auto sorted_by_duration = long_reports;
    if (!sorted_by_duration.empty()) {
        mergeSort(sorted_by_duration, 0, sorted_by_duration.size() - 1, &compareDurationDesc);
    }

    // Сортировка по фамилии и теме по возрастанию методом слияния
    auto sorted_by_author_topic = long_reports;
    if (!sorted_by_author_topic.empty()) {
        mergeSort(sorted_by_author_topic, 0, sorted_by_author_topic.size() - 1, &compareAuthorTopicAsc);
    }
    // Можно вывести отсортированные данные при необходимости

    return 0;
}
