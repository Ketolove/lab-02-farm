#include <stdio.h>

#define INV_LEN	10

int main(void)
{
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
	char items[INV_LEN][11] = {
		"Empty", "Wood", "Stone", "Seeds", "Rake", "Shovel", "Fertilizer", "Bucket", "VC", ""
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
		printf("0 - Exit\n"
			"1 - Look at time\n"
			"2 - Work\n"
			"3 - Look at inventory\n"
			"4 - Put item to slot\n"
			"5 - Throw away the item\n"
			"6 - Show uniques\n");


		int us_inp;

		// проверка на дурачка
		if ((scanf_s("%d", &us_inp) != 1) || (us_inp > 6) || (us_inp < 0)) {
			printf("Error input");
			// почему если ввести не букву, а неподходящее число, программа зависает?
			//scanf_s("%*s"); // чистим буфер потока ввода
			return 0;
		}


		switch (us_inp) {
		case 0:
			printf("0 - Exit\n");
			iters = 0;
			return 0;
		case 1:
			printf("1 - Look at time\n");
			printf("Current time: Day %d, %0d:00\n", current_day, current_hour);
			break;
		case 2:
			printf("2 - Work\n");
			int work_hours;
			printf("How much work?\n");
			scanf_s("%d", &work_hours);
			current_hour = (current_hour + work_hours) % 24;
			current_day = current_day + (current_hour + work_hours) / 24;
			printf("Current time: Day %d, %0d:00\n", current_day, current_hour);
			break;
		case 3:
			printf("3 - Look at inventory\n");
			for (int i = 0; i < 10; i++)
			{
				printf("Slot %d: [%d] (%s), ", i, inventory[i], items[i]);
			}
			printf("\n");
			break;
		case 4:
			printf("4 - Put item to slot\n");
			int num_slot;
			int cnt_slot;
			printf("Number of slot: ");
			scanf_s("%d", &num_slot);
			printf("How much items? ");
			scanf_s("%d", &cnt_slot);
			inventory[num_slot] += cnt_slot;
			//printf("slot %d, %d items", num_slot, inventory[num_slot]);
			break;
		case 5:
			printf("5 - Throw away the item\n");
			int num_slot_null;
			printf("Number of slot: ");
			scanf_s("%d", &num_slot_null);
			inventory[num_slot_null] = 0;
			//printf("slot %d, %d items", num_slot_null, inventory[num_slot_null]);
			break;
		case 6:
			printf("6 - Show uniques\n");
			for (int i = 0; i < 10; i++)
			{
				printf("%s ", items[i]);
			}
			printf("\n");
			break;
		}
	}




	return 0;
}