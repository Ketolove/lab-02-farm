#include <stdio.h>
#pragma warning(disable : 4996)
#include <windows.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

#define INV_LEN	10
#define MAX_ITEMS    10
#define MAX_NAME_LEN 32
char user_name[MAX_NAME_LEN] = { 0 };
char items[MAX_ITEMS][MAX_NAME_LEN] = {
		 "0 Пусто", "1 Дерево", "2 Камень", "3 Семена", "4 Лейка", "5 Ведро", "6 Удобрение", "7 Корзина", "8 Грабли", "9 Лопата"
};

void Your_Name(void) {
	int er_inp_flag = 1;
	while (er_inp_flag == 1) {
		printf("Как Вас зовут?\n");

		int res = scanf("%31s", user_name);   // пишем сразу в глобальный буфер
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
char** supplies_names = NULL;   // уникальные имена
int* supplies_totals = NULL;   // суммарные количества
int supplies_count = 0;
int item_count = 0;
int current_day = 1;
int current_hour = 8;
int inventory[INV_LEN] = { 0 };

void show_time(void) {
	printf("Текущее время: День %d, %0d:00\n", current_day, current_hour);
	return;
}

void work(void) {
	int work_hours;
	printf("Сколько часов работать?\n");
	scanf_s("%d", &work_hours);
	current_hour = (current_hour + work_hours) % 24;
	current_day = current_day + (current_hour + work_hours) / 24;
	printf("Текущее время: День %d, %0d:00\n", current_day, current_hour);
	return;
}

void show_inventory(void) {
	for (int i = 0; i < INV_LEN; i++) {
		int id = inventory[i];                 // ID предмета в слоте i
		printf("Слот %d: [%d] - %s\n",
			i,
			id,
			item_names[id] + 2);            // +2 — убираем "N " из каталога
	}
	return;
}

void put_item(void) {
	int num_slot, item_id;
	printf("Введите номер слота (от 0 до %d): ", INV_LEN - 1);
	if (scanf_s("%d", &num_slot) != 1) {
		printf("Ошибка ввода!\n");
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {}
		return;
	}
	printf("Введите ID предмета: ");
	if (scanf_s("%d", &item_id) != 1) {
		printf("Ошибка ввода!\n");
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {}
		return;
	}

	if (num_slot < 0 || num_slot >= INV_LEN) {
		printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
		return;
	}
	if (item_id < 0 || item_id >= MAX_ITEMS) {
		printf("Ошибка: нет такого ID! (допустимо 0..%d)\n", MAX_ITEMS - 1);
		return;
	}

	inventory[num_slot] = item_id;
	printf("В слот %d записан предмет [%d] (%s)\n",
		num_slot, item_id, item_names[item_id] + 2);
}

void clean_slot(void) {
	int num_slot_null;
	printf("Введите номер слота: ");
	if (scanf_s("%d", &num_slot_null) != 1) {
		printf("Ошибка ввода!\n");
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {}
		return;
	}
	if (num_slot_null < 0 || num_slot_null >= INV_LEN) {
		printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
		return;
	}
	inventory[num_slot_null] = 0;
	printf("Слот %d очищен\n", num_slot_null);
	return;
}

void show_uniques(void) {
	// Шаг 1: вывести текущий инвентарь
	for (int i = 0; i < INV_LEN; i++) {
		int id = inventory[i];
		printf("Слот %d: [%d] - %s\n", i, id, item_names[id] + 2);
	}
	printf("\n");

	// Шаг 2: для каждого ID посчитать повторы среди непустых слотов
	int any = 0;
	for (int id = 1; id < item_count; id++) {          // 0 = "Пусто" пропускаем
		int cnt = 0;
		for (int slot = 0; slot < INV_LEN; slot++) {
			if (inventory[slot] == id) cnt++;
		}
		if (cnt == 0) continue;                        // этого ID в инвентаре нет
		any = 1;
		if (cnt == 1)
			printf("ID %d (%s) - уникален\n", id, item_names[id] + 2);
		else
			printf("ID %d (%s) - не уникален, лежит в %d слотах\n",
				id, item_names[id] + 2, cnt);
	}

	if (!any) printf("В инвентаре нет предметов.\n");
	return;
}

void fill_defaults(void) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		strncpy(item_names[i], items[i], MAX_NAME_LEN - 1);
		item_names[i][MAX_NAME_LEN - 1] = '\0';   // гарантируем терминатор
	}
	item_count = MAX_ITEMS;
	return;
}

void read_items(void) {
	FILE* fp = fopen("items.txt", "r");
	if (!fp) {
		printf("Ошибка: файл инвентаря не найден. Инвентарь будет заполнен по умолчанию.\n");
		fill_defaults();
		return;
	}


	int n = 0;
	item_count = n;
	if (item_count < MAX_ITEMS) {
		// дополнить оставшиеся слоты дефолтными именами
		for (int i = item_count; i < MAX_ITEMS; i++) {
			strncpy(item_names[i], items[i], MAX_NAME_LEN - 1);
			item_names[i][MAX_NAME_LEN - 1] = '\0';
		}
		item_count = MAX_ITEMS;
	}
	fclose(fp);
}

void find_item(void) {
	char user_find[MAX_NAME_LEN];

	printf("Что Вы ищете?\n");
	if (scanf("%31s", user_find) != 1) {
		printf("Ошибка ввода!\n");
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {}
		return;
	}
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}   // дочищаем буфер

	// 1) Ищем ID по названию в каталоге
	int found_id = -1;
	for (int i = 0; i < item_count; i++) {
		if (strcmp(user_find, item_names[i] + 2) == 0) {   // +2 — пропускаем "N "
			found_id = i;
			break;
		}
	}

	if (found_id == -1) {
		printf("Предмет \"%s\" не найден в каталоге.\n", user_find);
		return;
	}

	// 2) Ищем этот ID в инвентаре
	int found_any = 0;
	for (int slot = 0; slot < INV_LEN; slot++) {
		if (inventory[slot] == found_id) {
			printf("Слот %d: [ID %d] (%s)\n",
				slot, inventory[slot], item_names[found_id] + 2);
			found_any = 1;
		}
	}

	if (!found_any) {
		printf("Предмет \"%s\" (ID %d) есть в каталоге, но в инвентаре отсутствует.\n",
			user_find, found_id);
	}
}



void free_supplies(void) {
	if (supplies_names) {
		for (int i = 0; i < supplies_count; i++)
			free(supplies_names[i]);
		free(supplies_names);
		supplies_names = NULL;
	}
	free(supplies_totals);
	supplies_totals = NULL;
	supplies_count = 0;
	return;
}

void write_diary() {
	printf("Введите запись: ");
	char user_routine[256];
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
	if (!fgets(user_routine, sizeof user_routine, stdin)) return;
	user_routine[strcspn(user_routine, "\n")] = '\0';

	FILE* fp = fopen("diary.txt", "a");
	if (!fp) { printf("Ошибка открытия файла.\n"); return; }

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
	return;
}

void count_items(void) {
	free_supplies();                     // освобождаем прошлый вызов, если был

	FILE* fp = fopen("input.txt", "r");
	if (!fp) {
		printf("Ошибка: файл input.txt не найден.\n");
		return;
	}

	char buf[1024];
	while (fgets(buf, sizeof buf, fp)) {
		char name[MAX_NAME_LEN];
		int  cnt;

		// разбор строки "Имя число"
		if (sscanf(buf, "%31s %d", name, &cnt) != 2) {
			//printf("Предупреждение: строка не распознана: %s", buf);
			continue;
		}
		if (cnt <= 0) continue;          // нулевые/отрицательные пропускаем

		// ищем имя среди уже накопленных
		int found = -1;
		for (int k = 0; k < supplies_count; k++) {
			if (strcmp(supplies_names[k], name) == 0) {
				found = k;
				break;
			}
		}

		if (found >= 0) {
			supplies_totals[found] += cnt;           // суммируем
		}
		else {
			// новое имя — расширяем оба массива на 1 элемент
			char** tmp_names = realloc(supplies_names,
				(size_t)(supplies_count + 1) * sizeof * tmp_names);
			int* tmp_totals = realloc(supplies_totals,
				(size_t)(supplies_count + 1) * sizeof * tmp_totals);
			if (!tmp_names || !tmp_totals) {
				printf("Ошибка выделения памяти.\n");
				if (tmp_names)  supplies_names = tmp_names;
				if (tmp_totals) supplies_totals = tmp_totals;
				fclose(fp);
				return;
			}
			supplies_names = tmp_names;
			supplies_totals = tmp_totals;

			size_t len = strlen(name);
			supplies_names[supplies_count] = malloc(len + 1);
			if (!supplies_names[supplies_count]) {
				printf("Ошибка выделения памяти.\n");
				fclose(fp);
				return;
			}
			memcpy(supplies_names[supplies_count], name, len + 1);
			supplies_totals[supplies_count] = cnt;
			supplies_count++;
		}
	}
	fclose(fp);

	if (supplies_count == 0) {
		printf("В input.txt не найдено корректных поставок.\n");
		return;
	}

	// запись сводки в output.txt и на экран
	FILE* out = fopen("output.txt", "w");
	if (!out) {
		printf("Ошибка открытия output.txt на запись.\n");
		return;
	}
	for (int i = 0; i < supplies_count; i++) {
		fprintf(out, "%s: %d шт.\n", supplies_names[i], supplies_totals[i]);
		printf("%s: %d шт.\n", supplies_names[i], supplies_totals[i]);
	}
	fclose(out);
	printf("Сводка записана в output.txt.\n");
}


int main(void)

{
	// имя, введённое пользователем на русском, отображается корректно, только если все 3 выключены (?)
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	setlocale(LC_ALL, ".UTF-8");

	
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
			return 0;
		}

		switch (us_inp) {
		case 0:
			printf("0 - Выход\n");
			free_supplies();
			iters = 0;
			return 0;
		case 1:
			printf("1 - Посмотреть на часы\n");
			show_time();
			break;
		case 2:
			printf("2 - Промотать время (поработать)\n");
			work();
			break;
		case 3:
			printf("3 - Посмотреть инвентарь\n");
			show_inventory();
			break;
		case 4: {
			printf("4 - Положить предмет в слот\n");
			put_item();
			break;
		}
		case 5:
			printf("5 - Выбросить предмет\n");
			clean_slot();
			break;
		case 6: {
			printf("6 - Уникальные находки\n");
			show_uniques();
			break;
		}
		case 7: {
			printf("7 - Поиск предмета в рюкзаке по названию\n");
			find_item();
			break;
		}
		case 8: {
			printf("Записать состояние в дневник фермера\n");
			write_diary();
			break;
		}
		case 9:
			printf("9 - Расшифровать старые записи\n");
			count_items();
			break;
		}
	}
	return 0;
}