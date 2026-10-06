#include <stdio.h>
#pragma warning(disable : 4996)
#include <windows.h>
#include <locale.h>

#define INV_LEN	10


int main(void)

{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	setlocale(LC_ALL, ".UTF-8");
	// ids:
	// 0 - пусто
	// 1 - дерево
	// 2- камень
	// 3 - семена
	// 4- грабли
	// 5 - лопата
	// 6 - удобрение
	// 7 - ведро
	// 8 - лейка
	// 9 - лейка

	int current_day = 1;
	int current_hour = 8;
	int inventory[INV_LEN] = { 0 };
	char items[INV_LEN][20] = {
		 "Пусто", "Дерево", "Камень", "Семена", "Лейка", "Ведро", "Удобрение", "Корзина", "Лейка", "Ведро"
	};
	int items2[INV_LEN] = {
		0, 1, 2, 3, 4, 8, 5, 6, 7, 8
	};

	// присваиваем каждому элементу инвентаря ID 
	//for (int i = 0; i < 10; i++)
	//{
	//	inventory[i] = i;
	//}

	//for (int i = 0; i < INV_LEN; ++i)
		//printf("%d ", inventory[i]);
	int iters = 1;
	while (iters == 1) {
		printf("0 - Выход\n"
			"1 - Посмотреть на часы\n"
			"2 - Промотать время (поработать)\n"
			"3 - Посмотреть инвентарь\n"
			"4 - Положить предмет в слот\n"
			"5 - Выбросить предмет\n"
			"6 - Уникальные находки\n");


		int us_inp;

		// проверка на дурачка
		if ((scanf_s("%d", &us_inp) != 1) || (us_inp > 6) || (us_inp < 0)) {
			printf("Ошибка ввода");
			// почему если ввести не букву, а неподходящее число, программа зависает?
			//scanf_s("%*s"); // чистим буфер потока ввода
			return 0;
		}


		switch (us_inp) {
		case 0:
			printf("0 - Выход\n");
			iters = 0;
			return 0;
		case 1:
			printf("1 - Посмотреть на часы\n");
			printf("Текущее время: День %d, %0d:00\n", current_day, current_hour);
			break;
		case 2:
			printf("Промотать время (поработать)\n");
			int work_hours;
			printf("Сколько часов работать?\n");
			scanf_s("%d", &work_hours);
			current_hour = (current_hour + work_hours) % 24;
			current_day = current_day + (current_hour + work_hours) / 24;
			printf("Текущее время: День %d, %0d:00\n", current_day, current_hour);
			break;
		case 3:
			printf("3 - Посмотреть инвентарь\n");
			for (int i = 0; i < 10; i++)
			{
				printf("Слот %d: [%d] (%s)\n", i, inventory[i], items[i]);
			}
			printf("\n");
			break;
		case 4:
			printf("4 - Положить предмет в слот\n");
			int num_slot;
			int cnt_slot;
			printf("Введите номер слота: ");
			scanf_s("%d", &num_slot);
			printf("Сколько предметов положить? ");
			scanf_s("%d", &cnt_slot);
			inventory[num_slot] += cnt_slot;
			//printf("slot %d, %d items", num_slot, inventory[num_slot]);
			break;
		case 5:
			printf("5 - Выбросить предмет\n");
			int num_slot_null;
			printf("Введите номер слота: ");
			scanf_s("%d", &num_slot_null);
			inventory[num_slot_null] = 0;
			//printf("slot %d, %d items", num_slot_null, inventory[num_slot_null]);
			break;
		case 6:
			printf("6 - Уникальные находки\n");
			//	for (int i = 0; i < 10; i++)
			//	{
			//		printf("%s ", items[i]);
			//	}
			//	printf("\n");
			for (int i = 0; i < 10; i++) {
				if (inventory[i] == 0) continue;
				int flag = 1;
				int cnt = 1;
				for (int j = 0; j < 10; j++) {
					if ((i != j) && (items2[i] == items2[j])) {
					//if ((i != j) && (strcmp(items[i], items[j]) == 0)) {
					//	flag = 0;
						cnt += 1;
						printf("[%d] не уникален, их [%d]\n", items2[i], cnt);
						break;// Найден дубликат
					}
					//else if (items[i] != items[j]) {
/*					else if (items2[i] != items2[j]) {
						
						break;
					}*/
					
				}
				if (cnt == 1) {
					printf("[%d] уникален\n", items2[i]);
					
				}
			}


			break;
		}
	}




	return 0;
}