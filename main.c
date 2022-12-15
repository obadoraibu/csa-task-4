#include <pthread.h>
#include <iostream>
#include <vector>

static int N; // всего "парней"

class Boy;

std::vector<Boy*> girlsMessageBox; // векор валентинок(ссылкок на объекты Boy)
int amountOfMessages = 0; // количество полученых валентинок
pthread_mutex_t mutexGirlsMessageBox; // мьютекс для заполнения вектора валентинок
pthread_mutex_t mutexGirlsAnswer; // мьютекс для ответа (проставления accepted)
pthread_cond_t condGirlsMessageBox; // усллвная переменная для ожидания заполнения girlsMessageBox
pthread_cond_t condGirlsDecision; // усллвная переменная для ожидания проставления accepted

class Boy {
public:
    int number; // номер парня(присваивается во время создания)
    int charisma; // харизма
    int accepted = -1; // результат приглашения

    Boy(int number, int charisma) { // конструктор
        this->number = number;
        this->charisma = charisma;
    }
    void *sendValentine(void) { // поток отправки валентинки и получения ответа (парень)
        pthread_mutex_lock(&mutexGirlsMessageBox); // мьютекс

        std::cout << "(Парень " + std::to_string(number + 1) + ") Отправляет валентинку..." << std::endl;
        girlsMessageBox.push_back(this); // добавление элемента в girlsMessageBox
        amountOfMessages++; // увеличение количества валентинок
        pthread_cond_signal(&condGirlsMessageBox); // сигнал одному ожидающему condGirlsMessageBox потоку
        pthread_mutex_unlock(&mutexGirlsMessageBox); // анлок


        pthread_mutex_lock(&mutexGirlsAnswer);
        while(accepted < 0) { // пока не изменяли accepted
            std::cout << "(Парень " + std::to_string(number + 1) + ") Ждет решения девушки..." << std::endl;
            pthread_cond_wait(&condGirlsDecision, &mutexGirlsAnswer); // ожидание сигнала
        }
        if (accepted) { // если ответ положительный
            std::cout << "(Парень " + std::to_string(number + 1) + ") Согласилась!!!" << std::endl;
        } else { // в противном случае
            std::cout << "(Парень " + std::to_string(number + 1) + ") Отказала..." << std::endl;
        }
        pthread_mutex_unlock(&mutexGirlsAnswer); // анлок
    }

    static void *sendValentineHelper(void *context) { // для вызова в качестве метода
        return ((Boy *)context)->sendValentine();
    }
};

void *chooseBoy(void *shared_arg) { // поток  девушка
    pthread_mutex_lock(&mutexGirlsMessageBox);
    while (amountOfMessages < N) { // пока не все отправили
        std::cout << "Девушка ждет остальных" << std::endl;
        pthread_cond_wait(&condGirlsMessageBox, &mutexGirlsMessageBox); // ожидание сигнала
    }
    pthread_mutex_unlock(&mutexGirlsMessageBox);
    pthread_mutex_lock(&mutexGirlsAnswer);
    std::cout << "Девушка выбирает" << std::endl;
    // std::srand(1);
    int max_charisma = girlsMessageBox[0]->charisma; // выбор наиболее харизматичного
    int max = girlsMessageBox[0]->charisma;;

    for (auto & i : girlsMessageBox) {
        if (max_charisma < i->charisma) {
            max_charisma = i->charisma;
            max = i->number;
        }
    }
    for (auto & i : girlsMessageBox) {
        if (max == i->number) {
            i->accepted = 1;
        }
        else i->accepted = 0;
    }
    std::cout << "Девушка выбрала" << std::endl;
    pthread_mutex_unlock(&mutexGirlsAnswer);
    pthread_cond_broadcast(&condGirlsDecision); // сигнал всем потокам ожидающим condGirlsDecision потокам
}

int main(int argc, char *argv[]) {
    int R;
    if (argc == 3) { // аргументы командной строки
        if (std::stoi(argv[1]) > 100 || std::stoi(argv[1]) < 0) {
            std::cout << "Некорректный ввод";
            return 1;
        }
        N = std::stoi(argv[1]);
        if (std::stoi(argv[2]) > 32767 || std::stoi(argv[2]) < -32767) {
            std::cout << "Некорректный ввод";
            return 1;
        }
        R = std::stoi(argv[2]);
    } else { // данные из консоли
        do {
            std::cout << "Количество парней: ";
            std::cin >> N;
        } while (N > 100 || N < 0);
        do {
            std::cout << "Сид генерации случайных чисел: ";
            std::cin >> R;
        } while (R > 32767 ||  R < -32767);
    }
    std::srand(R);
    std::vector<pthread_t> boysThreads(N); // обьявление потоков "парни"
    std::vector<Boy> boys; // массив "парней"
    for (int i = 0; i < N; ++i) {
        Boy boy(i, std::rand() / 100);
        boys.push_back(boy);
    }
    pthread_t girl;
    pthread_mutex_init(&mutexGirlsAnswer, NULL); // инициализация мьютексов и условных переменных
    pthread_mutex_init(&mutexGirlsMessageBox, NULL);
    pthread_cond_init(&condGirlsMessageBox, NULL);
    pthread_cond_init(&condGirlsDecision, NULL);
    pthread_create(&girl, NULL, chooseBoy, NULL); // старт потока "девушка"
    for (int i = 0; i < N; ++i) {
        pthread_create(&boysThreads[i], NULL, &boys[i].sendValentineHelper, &boys[i]); // старт потоков "парни"
    }
    pthread_join(girl, NULL); // ожидаем завершения всех girl

    for (int i = 0; i < N; ++i) {
        pthread_join(boysThreads[i], NULL); // ожидаем завершения всех boysThreads
    }
    pthread_mutex_destroy(&mutexGirlsAnswer); // уничитожение мьютексов и условных переменных
    pthread_mutex_destroy(&mutexGirlsMessageBox);
    pthread_cond_destroy(&condGirlsMessageBox);
    pthread_cond_destroy(&condGirlsDecision);
}