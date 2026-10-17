#include <stdio.h>
#pragma warning(disable : 4996)
#include <windows.h>
#include <locale.h>
#include <string.h>

#define INV_LEN	10
#define MAX_ITEMS    10
#define MAX_NAME_LEN 32
char user_name[MAX_NAME_LEN] = { 0 };
char items[MAX_ITEMS][MAX_NAME_LEN] = {
		 "0 Пусто", "1 Дерево", "2 Камень", "3 Семена", "4 Лейка", "5 Ведро", "6 Удобрение", "7 Корзина", "8 Лейка", "9 Ведро"
};

void Your_Name(void) {
	int er_inp_flag = 1;
	while (er_inp_flag == 1) {
		printf("Как Вас зовут?\n");

		int res = scanf("%32s", user_name);   // пишем сразу в глобальный буфер
		int c = getchar();

		if (res != 1) {
			printf("Ошибка ввода!\n");
			while ((c = getchar()) != '\n' && c != EOF) {}
		}
		else if (c != '\n' && c != EOF && c != ' ' && c != '\t') {
			printf("Ошибка ввода! Имя должно содержать не больше 32 символов!\n");
			while ((c = getchar()) != '\n' && c != EOF) {}
		}
		else {
			printf("Здравствуйте, %s!\n", user_name);
			er_inp_flag = 0;
		}
	}
}



char item_names[MAX_ITEMS][MAX_NAME_LEN];
int  item_count = 0;

void fill_defaults() {
	for (int i = 0; i < MAX_ITEMS; i++) {
		strncpy(item_names[i], items[i], MAX_NAME_LEN - 1);
		item_names[i][MAX_NAME_LEN - 1] = '\0';   // гарантируем терминатор
	}
	item_count = MAX_ITEMS;
}

void read_items() {
	FILE* fp = fopen("items.txt", "r");
	if (!fp) {
		printf("Ошибка: файл инвентаря не найден. Инвентарь будет заполнен по умолчанию.\n");
		fill_defaults();
		return 0;
	}


	int n = 0;
	while (n < MAX_ITEMS && fgets(item_names[n], MAX_NAME_LEN, fp)) {
		item_names[n][strcspn(item_names[n], "\n")] = '\0';
		n++;
	}
	item_count = n;
	fclose(fp);
}


int main(void)

{
	// имя, введённое пользователем на русском, отображается корректно, только если все 3 выключены (?)
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	setlocale(LC_ALL, ".UTF-8");

	int current_day = 1;
	int current_hour = 8;
	int inventory[INV_LEN] = { 0 };
	
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
	Your_Name();// узнаём имя пользователя
	read_items(); // создаём инвентарь
	for (int i = 0; i < item_count; i++)
		printf("%s\n", item_names[i]);
	int iters = 1;
	while (iters == 1) {
		printf("0 - Выход\n"
			"1 - Посмотреть на часы\n"
			"2 - Промотать время (поработать)\n"
			"3 - Посмотреть инвентарь\n"
			"4 - Положить предмет в слот\n"
			"5 - Выбросить предмет\n"
			"6 - Уникальные находки\n"
			"7 - Поиск предмета в рюкзаке по названию\n"
			"8 - Записать состояние в дневник фермера\n"
			"9 - Расшифровать старые записи\n");


		int us_inp;

		// проверка на дурачка
		if ((scanf_s("%d", &us_inp) != 1) || (us_inp > 9) || (us_inp < 0)) {
			printf("Ошибка ввода, попробуйте ещё раз!\n");
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {}
			continue;
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
				printf("Слот %d: [%d] (%s)\n", i, inventory[i], items[i] + 2); // сдвигаем указатель на 2, чтобы убрать нумерацию id
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
			for (int i = 0; i < 10; i++) {
				if (inventory[i] == 0) continue;
				int flag = 1;
				int cnt = 1;
				for (int j = 0; j < 10; j++) {
					if ((i != j) && (items2[i] == items2[j])) {
						cnt += 1;
						printf("[%d] не уникален, их [%d]\n", items2[i], cnt);
						break;// Найден дубликат
					}					
				}
				if (cnt == 1) {
					printf("[%d] уникален\n", items2[i]);
					
				}
			}
			break;
		case 7:
			printf("7 - Поиск предмета в рюкзаке по названию\n");
			break;
		case 8: {
			printf("Введите запись: ");

			char user_routine[256];
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {}
			if (!fgets(user_routine, sizeof user_routine, stdin)) break;
			user_routine[strcspn(user_routine, "\n")] = '\0';

			FILE* fp = fopen("diary.txt", "a");
			if (!fp) { printf("Ошибка открытия файла.\n"); break; }

			fprintf(fp, "Фермер: %s\nТекущее время: День %d, %0d:00\nСписок предметов в инвентаре:\n", user_name, current_day, current_hour);
			for (int i = 0; i < 10; i++)
			{
				fprintf(fp, "%s ", items[i] + 2); // сдвигаем указатель на 2, чтобы убрать нумерацию id
			}
			fprintf(fp, "\nЗаметка:\n%s\n", user_routine);
			for (int i = 0; i < 68; i++)
			{
				fprintf(fp, "-");
			}
			fprintf(fp, "\n");
			fclose(fp);
			printf("Запись сохранена.\n");
			break;
		}
		case 9:
			printf("9 - Расшифровать старые записи\n");
			break;
		}
	}




	return 0;
}