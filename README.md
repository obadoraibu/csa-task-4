# Архитектура вычислительных систем - ИДЗ 4
# Завьялов Егор БПИ-217 Вариант 24

### Условие задачи
24. Задача о привлекательной студентке. У одной очень привлекательной
студентки есть N поклонников. Традиционно в день св. Валентина очень привлекательная студентка проводит романтический вечер с одним из поклонников. Счастливый избранник заранее не известен. С утра очень привлекательная студентка получает N «валентинок» с различными вариантами романтического вечера. Выбрав наиболее заманчивое предложение, студентка извещает счастливчика о своем согласии, а остальных – об отказе. Требуется создать многопоточное приложение, моделирующее поведение студентки.

### Модель параллельных вычислений
При разработке данной многопоточной программы была попытка реализовать модель клиент-сервер с активным ожиданием. 
Клиентский поток запрашивает сервер и ждет ответа. Серверный поток ожидает запроса от клиента, затем действует в соответствии с поступившим запросом. 

### Синxропримитив
Был использован pthread_mutex

### Ограничения на входные данные
Допустимые диапазоны входных данных:
1. Количество парней от 1 до 100
2. Сид для генерации случайных чисел в пределах int 
### Поясняющие комментарии
Добавлены 

### Описание сценария работы программы
Будем считать, что выбор девушка отдаст парню с наибольшей харизмой, независимо от того, какое свидание он предложил. Парни получают случайно сгенерированные показатели харизмы и порядковый номер. Девушка в день святого валентина просыпается и начинает проверять свой почтовый ящик. N парней наперегонки высылают ей валентинки(в решении ссылку на обьект-себя). После каждой присланой валнетинки девушка получает сигнал условной переменной и проверяет не заполнился ли вектор валентинок, она заранее знает количество парней. После заполнения вектора девушка находит в нем ссылку на объект(парня) с максимальной харизмой и в его публичном поле изменяет значение переменной accepted на true, другим парням на false соответственно. После проставления своих решений девушка сообщает, что ответы направлены парням с помощью pthread_cond_broadcast. Парни узнают результаты и либо расстраиваются, либо радуются.

### Добавлен ввод через аргуументы командной строки
