#include <iostream>
#include <map>
#include <fstream>
#include <vector>
using namespace std;

struct Date {
    int d, m, y;
    Date(int d = 1, int m = 1, int y = 2026) : d(d), m(m), y(y) {}
    bool operator<(const Date& other) const {
        if (y != other.y) return y < other.y;
        if (m != other.m) return m < other.m;
        return d < other.d;
    }
};

struct Time {
    int h;
    Time(int h = 0) : h(h) {}
    bool operator<(const Time& other) const { return h < other.h; }
};

class Thermometer {
private:
    map<Date, map<Time, double>> data;
    Date startDate;
    Time startTime;

public:
    Thermometer() : startDate(1, 1, 2026), startTime(0) {}

    void setStart(Date d, Time t) { startDate = d; startTime = t; }

    void getStart(Date& d, Time& t) { d = startDate; t = startTime; }

    void setObs(Date d, Time t, double temp) { data[d][t] = temp; }

    double getTemp(Date d, Time t) {
        if (data.count(d) && data[d].count(t)) return data[d][t];
        return -999;
    }

    void setSeries(Date d, vector<double> temps) {
        for (int i = 0; i < 24; i++) data[d][Time(i)] = temps[i];
    }

    double avgDate(Date d) {
        double sum = 0; int cnt = 0;
        for (auto& p : data[d]) { sum += p.second; cnt++; }
        return cnt ? sum / cnt : 0;
    }

    double avgMonth(int m) {
        double sum = 0; int cnt = 0;
        for (auto& dateObs : data)
            if (dateObs.first.m == m)
                for (auto& timeObs : dateObs.second)
                {
                    sum += timeObs.second; cnt++;
                }
        return cnt ? sum / cnt : 0;
    }

    double avgAll() {
        double sum = 0; int cnt = 0;
        for (auto& dateObs : data)
            for (auto& timeObs : dateObs.second)
            {
                sum += timeObs.second; cnt++;
            }
        return cnt ? sum / cnt : 0;
    }

    double avgDayNight(int m, bool isDay) {
        double sum = 0; int cnt = 0;
        for (auto& dateObs : data)
            if (dateObs.first.m == m)
                for (auto& timeObs : dateObs.second) {
                    int h = timeObs.first.h;
                    if ((isDay && h >= 6 && h <= 18) || (!isDay && (h >= 19 || h <= 5)))
                    {
                        sum += timeObs.second; cnt++;
                    }
                }
        return cnt ? sum / cnt : 0;
    }

    void save(string fname) {
        ofstream f(fname);
        f << startDate.d << " " << startDate.m << " " << startDate.y << " " << startTime.h << endl;
        for (auto& dateObs : data)
            for (auto& timeObs : dateObs.second)
                f << dateObs.first.d << " " << dateObs.first.m << " " << dateObs.first.y << " "
                << timeObs.first.h << " " << timeObs.second << endl;
    }

    void load(string fname) {
        ifstream f(fname);
        data.clear();
        f >> startDate.d >> startDate.m >> startDate.y >> startTime.h;
        int d, m, y, h; double temp;
        while (f >> d >> m >> y >> h >> temp)
            data[Date(d, m, y)][Time(h)] = temp;
    }
};

// Пример использования
int main() {
    Thermometer t;

    cout << "=== Термометр. Наблюдения за 2026 год ===\n\n";

    // Задаём наблюдения
    t.setObs(Date(15, 6, 2026), Time(12), 26.5);
    t.setObs(Date(15, 6, 2026), Time(18), 23.0);
    t.setObs(Date(20, 12, 2026), Time(8), -5.5);
    t.setObs(Date(20, 12, 2026), Time(14), -2.0);

    // Устанавливаем начальную дату
    t.setStart(Date(1, 1, 2026), Time(0));

    // Получаем начальную дату
    Date sd; Time st;
    t.getStart(sd, st);
    cout << "Начало наблюдений: " << sd.d << "." << sd.m << "." << sd.y << " " << st.h << ":00\n\n";

    // Выводим температуры
    cout << "Температура 15.06.2026 12:00: " << t.getTemp(Date(15, 6, 2026), Time(12)) << "°C\n";
    cout << "Температура 15.06.2026 18:00: " << t.getTemp(Date(15, 6, 2026), Time(18)) << "°C\n";
    cout << "Температура 20.12.2026 08:00: " << t.getTemp(Date(20, 12, 2026), Time(8)) << "°C\n\n";

    // Средние температуры
    cout << "Средняя за 15 июня: " << t.avgDate(Date(15, 6, 2026)) << "°C\n";
    cout << "Средняя за июнь: " << t.avgMonth(6) << "°C\n";
    cout << "Средняя за декабрь: " << t.avgMonth(12) << "°C\n";
    cout << "Средняя за весь год: " << t.avgAll() << "°C\n\n";

    // Дневная/ночная температура
    cout << "Дневная средняя за июнь: " << t.avgDayNight(6, true) << "°C\n";
    cout << "Ночная средняя за июнь: " << t.avgDayNight(6, false) << "°C\n";
    cout << "Дневная средняя за декабрь: " << t.avgDayNight(12, true) << "°C\n";
    cout << "Ночная средняя за декабрь: " << t.avgDayNight(12, false) << "°C\n\n";

    // Серия наблюдений для одного дня
    cout << "Записываем серию наблюдений для 1 января 2026...\n";
    vector<double> jan1temps(24);
    for (int i = 0; i < 24; i++) jan1temps[i] = -10 + i * 0.5;
    t.setSeries(Date(1, 1, 2026), jan1temps);

    cout << "Средняя за 1 января: " << t.avgDate(Date(1, 1, 2026)) << "°C\n\n";

    // Сохраняем и загружаем
    t.save("weather_2026.txt");
    cout << "Данные сохранены в weather_2026.txt\n";

    Thermometer t2;
    t2.load("weather_2026.txt");
    cout << "Данные загружены из файла\n";
    cout << "Проверка: температура 15.06.2026 12:00 = " << t2.getTemp(Date(15, 6, 2026), Time(12)) << "°C\n";

    return 0;
}