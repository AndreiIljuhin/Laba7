#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

char* int_to_char(int i) {
	char* str = new char[15];
	str[0] = '\0';
	while (i > 0) {
		for (int i = 14; i > 0; i--)
			str[i] = str[i - 1];
		str[0] = (unsigned char)(48 + i % 10);
		i = i / 10;
	}
	return str;
}

struct Stud {
	char FIO[50]; //фио
	char gender; //пол
	int group; //номер группы
	int group_number; //номер в группе
	int exam_grades[3]; //оценки на экзамены
	int grades_standings[5]; //оценки за дифф. зачеты
	char form_of_training[15]; //форма обучения
	struct tm timeinfo; //время последнего изменения

	void Display_data() {
		cout << FIO << endl;
		cout << gender << endl;
		cout << "Группа " << group << " номер в группе " << group_number << endl;
		cout << "Оценки за экзамены прошлый семестр : " << exam_grades[0] << " " << exam_grades[1] << " " << exam_grades[2] << endl;
		cout << "Оценки за дифф. зачеты прошлый семестр : " << grades_standings[0] << " " << grades_standings[1] << " " << grades_standings[2] << " " << grades_standings[3] << " " << grades_standings[4] << " " << endl;
		cout << "Форма обучения " << form_of_training << endl;
		cout << "Последнее изменение данных о студенте: " << asctime(&timeinfo) << endl;
	}

	void put_in_readable_file(char* filename) {
		ofstream file(filename, ios_base::app);
		char* put_str;
		file.write(FIO, strlen(FIO));
		file.put('\n');
		file.put(gender);
		file.put('\n');
		put_str = int_to_char(group);
		file.write(put_str, strlen(put_str));
		file.put('\n');
		put_str = int_to_char(group_number);
		file.write(put_str, strlen(put_str));
		file.put('\n');
		for (int i = 0; i < 3; i++) {
			file.put((unsigned char)(48 + exam_grades[i]));
			file.put(' ');
		}
		file.put('\n');
		for (int i = 0; i < 5; i++) {
			file.put((unsigned char)(48 + grades_standings[i]));
			file.put(' ');
		}
		file.put('\n');
		file.write(form_of_training, strlen(form_of_training));
		file.put('\n');
		file.write(asctime(&timeinfo), strlen(asctime(&timeinfo)));
		file.put('\n');
		file.write("-----------------------\n\n", 26);

		file.close();
	}
};

// запись структуры в файл
template <typename stru>
void put_in_service_file(char* filename, stru* s)
{
	FILE* fp;
	char* c;
	int size = sizeof(stru); // количество записываемых байтов

	if ((fp = fopen(filename, "ab")) == NULL)
	{
		perror("Error occured while opening file");
	}
	// устанавливаем указатель на начало структуры
	c = (char*)s;
	// посимвольно записываем в файл структуру
	for (int i = 0; i < size; i++)
	{
		putc(*c++, fp);
	}
	fclose(fp);
}

// -------Список групп---------
struct student_groups {
	Stud** group = 0;
	int info = 0;
	student_groups* next_group = 0;
};

void new_group(student_groups*& last_group) {
	int info;
	cout << "Введите номер группы: ";
	cin >> info;
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	Stud** new_group = new Stud * [30];
	for (int i = 0; i < 30; i++)
		new_group[i] = nullptr;
	student_groups* new_list_group = new student_groups;
	new_list_group->info = info;
	new_list_group->group = new_group;
	last_group->next_group = new_list_group;
	last_group = new_list_group;
}

void new_group(student_groups*& last_group, int info) {
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	Stud** new_group = new Stud * [30];
	for (int i = 0; i < 30; i++)
		new_group[i] = nullptr;
	student_groups* new_list_group = new student_groups;
	new_list_group->info = info;
	new_list_group->group = new_group;
	last_group->next_group = new_list_group;
	last_group = new_list_group;
}

student_groups* search_group(student_groups* first_group, int group_info) {
	while (!(first_group->info == group_info or first_group->next_group == 0))
		first_group = first_group->next_group;

	if (first_group->info == group_info)
		return first_group;
	else return 0;
}
// ----------------------------

// загрузка из файла массива структур
void load_data_from_database(student_groups* first_group, student_groups*& last_group, char* filename)
{
	ifstream file(filename, ios_base::binary);
	file.seekg(0, ios::end);
	int size = file.tellg();
	file.clear();
	file.seekg(0);
	for (int i = 0; i < size / sizeof(Stud); i++) {
		Stud* student = new Stud;
		char* c = (char*)student;
		for (int j = 0; j < sizeof(Stud); j++) {
			file.get(*c);
			c++;
		}
//		student->Display_data();
		student_groups* group = search_group(first_group, student->group);
		if (group == 0) {
			new_group(last_group, student->group);
			group = last_group;
		}
		group->group[student->group_number - 1] = student;
	}

	file.close();
}

bool new_student_record(Stud**& S, int student_group_number, int group_info) {
	if (S[student_group_number] == nullptr) {
		S[student_group_number] = new Stud;
		cout << "Введите фио: ";
		cin.getline(S[student_group_number]->FIO, 50);
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		do {
			cout << "Пол: ";
			cin >> S[student_group_number]->gender;
			if (!(S[student_group_number]->gender == 'm' or S[student_group_number]->gender == 'f'))
				cout << "Допускаются варианты 'm' или 'f'\n";
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		} while (!(S[student_group_number]->gender == 'm' or S[student_group_number]->gender == 'f'));

		S[student_group_number]->group = group_info;
		S[student_group_number]->group_number = student_group_number + 1;

		cout << "Оценки за 3 экзамена: ";
		for (int i = 0; i < 3; i++) {
			do {
				cin >> S[student_group_number]->exam_grades[i];
				if (!(S[student_group_number]->exam_grades[i] >= 0 and S[student_group_number]->exam_grades[i] <= 5))
					cout << "Допускаются только оценки по 5-ти бальной шкале, 0 означает отсутствие оценки\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			} while (!(S[student_group_number]->exam_grades[i] >= 0 and S[student_group_number]->exam_grades[i] <= 5));
		}

		cout << "Оценки за 5 дифф. зачетов: ";
		for (int i = 0; i < 5; i++) {
			do {
				cin >> S[student_group_number]->grades_standings[i];
				if (!(S[student_group_number]->grades_standings[i] >= 0 and S[student_group_number]->grades_standings[i] <= 5))
					cout << "Допускаются только оценки по 5-ти бальной шкале, 0 означает отсутствие оценки\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			} while (!(S[student_group_number]->grades_standings[i] >= 0 and S[student_group_number]->grades_standings[i] <= 5));
		}

		do {
			cout << "Форма обучения: ";
			cin.getline(S[student_group_number]->form_of_training, 15);
			if (!(!strcmp(S[student_group_number]->form_of_training, "full time") or !strcmp(S[student_group_number]->form_of_training, "extramural") or !strcmp(S[student_group_number]->form_of_training, "part time")))
				cout << "Допускаются варианты: full time(очная), extramural(заочная), part time(очно-заочная)\n";
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		} while (!(!strcmp(S[student_group_number]->form_of_training, "full time") or !strcmp(S[student_group_number]->form_of_training, "extramural") or !strcmp(S[student_group_number]->form_of_training, "part time")));

		time_t rawtime;
		time(&rawtime); // текущая дата, выраженная в секундах
		S[student_group_number]->timeinfo = *localtime(&rawtime); // текущая дата, представленная в нормальной форме
		cout << "Текущая дата/время : " << asctime(&S[student_group_number]->timeinfo);

		return 1;
	}
	else {
		cout << "Запись о суденте уже существует\n";

		return 0;
	}
}

bool data_change(Stud**& S, int student_group_number) {
	if (S[student_group_number] != nullptr) {
		cout << "1 - Изменить ФИО\n";
		cout << "2 - Пол\n";
		cout << "3 - Номер группы\n";//?
		cout << "4 - Номер в группе\n";//?
		cout << "5 - Форму обучения\n";
		int choice;
		cin >> choice;

		if (choice == 1) {
			cout << "Введите фио: ";
			cin.getline(S[student_group_number]->FIO, 50);
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		}
		if (choice == 2) {
			do {
				cout << "Пол: ";
				cin >> S[student_group_number]->gender;
				if (!(S[student_group_number]->gender == 'm' or S[student_group_number]->gender == 'f'))
					cout << "Допускаются варианты 'm' или 'f'\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			} while (!(S[student_group_number]->gender == 'm' or S[student_group_number]->gender == 'f'));
		}
		if (choice == 3) {
			cout << "Номер группы: ";
			cin >> S[student_group_number]->group;
//			student_groups* group = search_group(first_group, S[student_group_number]->group);
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		}
		if (choice == 4) {
			cout << "Номер в группе: ";
			cin >> S[student_group_number]->group_number;
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		}
		if (choice == 5) {
			do {
				cout << "Форма обучения: ";
				cin.getline(S[student_group_number]->form_of_training, 15);
				if (!(!strcmp(S[student_group_number]->form_of_training, "full time") or !strcmp(S[student_group_number]->form_of_training, "extramural") or !strcmp(S[student_group_number]->form_of_training, "part time")))
					cout << "Допускаются варианты: full time(очная), extramural(заочная), part time(очно-заочная)\n";
				cin.clear();
				cin.ignore(cin.rdbuf()->in_avail());
			} while (!(!strcmp(S[student_group_number]->form_of_training, "full time") or !strcmp(S[student_group_number]->form_of_training, "extramural") or !strcmp(S[student_group_number]->form_of_training, "part time")));
		}

		time_t rawtime;
		time(&rawtime); // текущая дата, выраженная в секундах
		S[student_group_number]->timeinfo = *localtime(&rawtime); // текущая дата, представленная в нормальной форме
		cout << "Текущая дата/время : " << asctime(&S[student_group_number]->timeinfo);
		return 1;
	}
	else {
		cout << "Записи о суденте не существует\n";

		return 0;
	}
}

void Display_all_student_data(student_groups* first_group) {
	do {
		for (int i = 0; i < 30; i++) {
			if (first_group->group[i] != nullptr)
				first_group->group[i]->Display_data();
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
}

void top_students(student_groups* first_group) {
	do {
		for (int i = 0; i < 30; i++) {
			if (first_group->group[i] != nullptr) {
				float average_mark = 0; //Средний балл

				for (int j = 0; j < 3; j++) {
					average_mark += first_group->group[i]->exam_grades[j];
				}
				for (int j = 0; j < 5; j++) {
					average_mark += first_group->group[i]->grades_standings[j];
				}

				if (average_mark / 8 >= 4)
					first_group->group[i]->Display_data();
			}
				
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
}

void students_gender(student_groups* first_group) {
	int cout_m = 0, cout_f = 0;
	do {
		for (int i = 0; i < 30; i++) {
			if (first_group->group[i] != nullptr) {
				first_group->group[i]->gender == 'm' ? (cout_m++) : (cout_f++);
			}
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
	cout << "Количество студентов мужского пола: " << cout_m << "\nженского: " << cout_f << endl;
}

void scholarship_students(student_groups* first_group) {
	int counter = 0;
	do {
		for (int i = 0; i < 30; i++) {
			if (first_group->group[i] != nullptr) {
				bool is_triple = false; //Есть ли 3ка

				for (int j = 0; j < 3; j++) {
					if (first_group->group[i]->exam_grades[j] <= 3) is_triple = true;
				}
				for (int j = 0; j < 5; j++) {
					if (first_group->group[i]->grades_standings[j] <= 3) is_triple = true;
				}

				if (!is_triple and !strcmp(first_group->group[i]->form_of_training,"full time"))
					counter++;
			}
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
	cout << "Количество студентов, получающих стипендию: " << counter << endl;
}

void students_without_scholarship(student_groups* first_group) {
	student_groups* current_group = first_group;
	cout << "Студенты, не получающие стипендию\n";
	cout << "Учащиеся только на отлично:\n";
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				bool only_5 = true;

				for (int j = 0; j < 3; j++) {
					if (current_group->group[i]->exam_grades[j] != 5) only_5 = false;
				}
				for (int j = 0; j < 5; j++) {
					if (current_group->group[i]->grades_standings[j] != 5) only_5 = false;
				}

				if (only_5 and strcmp(current_group->group[i]->form_of_training, "full time"))//Если только 5 и не очная форма обучения
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
	current_group = first_group;
	cout << "Учащиеся только на хорошо:\n";
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				bool only_4 = true;

				for (int j = 0; j < 3; j++) {
					if (current_group->group[i]->exam_grades[j] != 4) only_4 = false;
				}
				for (int j = 0; j < 5; j++) {
					if (current_group->group[i]->grades_standings[j] != 4) only_4 = false;
				}

				if (only_4 and strcmp(current_group->group[i]->form_of_training, "full time"))//Если только 4 и не очная форма обучения
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
	current_group = first_group;
	cout << "Учащиеся на хорошо и отлично:\n";
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				bool is_triple = false; //Есть ли 3ка
				bool only_4 = true;
				bool only_5 = true;

				for (int j = 0; j < 3; j++) {
					if (current_group->group[i]->exam_grades[j] <= 3) is_triple = true;
					if (current_group->group[i]->exam_grades[j] != 4) only_4 = false;
					if (current_group->group[i]->exam_grades[j] != 5) only_5 = false;
				}
				for (int j = 0; j < 5; j++) {
					if (current_group->group[i]->grades_standings[j] <= 3) is_triple = true;
					if (current_group->group[i]->exam_grades[j] != 4) only_4 = false;
					if (current_group->group[i]->exam_grades[j] != 5) only_5 = false;
				}

				if (!only_4 and !only_5 and !is_triple and strcmp(current_group->group[i]->form_of_training, "full time"))//Нет троек и ниже, не только 4, не только 5 и не очная форма обучения
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
}

void students_recording_time(student_groups* first_group) {
	student_groups* current_group = first_group;
	int day;
	cout << "Вывести все записи, сделанные в день: ";
	cin >> day;
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				if (current_group->group[i]->timeinfo.tm_mday == day)
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
	current_group = first_group;
	cout << "Все записи, сделанные после полудня: ";
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				if (current_group->group[i]->timeinfo.tm_hour >= 12)
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
	current_group = first_group;
	cout << "Все записи, сделанные до полудня: ";
	do {
		for (int i = 0; i < 30; i++) {
			if (current_group->group[i] != nullptr) {
				if (current_group->group[i]->timeinfo.tm_hour < 12)
					current_group->group[i]->Display_data();
			}
		}
		current_group = current_group->next_group;
	} while (current_group != 0);
}

void students_by_number(student_groups* first_group) {
	int number;
	cout << "Номер студентов в группе: ";
	do {
		cin >> number;
		if (!(number > 0 and number < 31))
			cout << "Максимальное число студентов в группе - 30, минимальное - 1\n";
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
	} while (!(number > 0 and number < 31));
	do {
		if (first_group->group[number - 1] != nullptr) {
			first_group->group[number - 1]->Display_data();
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
}

void data_output_to_file(student_groups* first_group) {
	ofstream file((char*)"students.txt");
	file.close();
	file.open((char*)"students_service.txt");
	file.close();
	do {
		for (int i = 0; i < 30; i++) {
			if (first_group->group[i] != nullptr) {
				first_group->group[i]->put_in_readable_file((char*)"students.txt");
				put_in_service_file((char*)"students_service.txt", first_group->group[i]);
			}
		}
		first_group = first_group->next_group;
	} while (first_group != 0);
}

struct Conf {
	char FIO[50]; //фио
	char gender; //пол
	char academic_degree[50];//Степень
	char university[50];//Университет
	bool performance_status;//Выступающий или нет
	char topic_of_speech[50] = "\0";//Тема выступления
	char hometown[50];//Город проживания
	bool invited;//Приглашенный или нет

	void Display_data();
};
void new_conf_record(Conf* c, int& N);
void load_data_from_database(Conf* conference, char* filename, int& N_conf);
void output_conference_participants(Conf* c, int N, char* FIO, char* gender, char* academic_degree, char* university, int performance_status, char* topic_of_speech, char* hometown, int invited, int negation);

int main()
{
	setlocale(LC_ALL, "ru");

	student_groups* first_group;
	student_groups* last_group;
	first_group = last_group = new student_groups;
	first_group->group = new Stud * [30];//Создаю первую группу студентов
	for (int i = 0; i < 30; i++)
		first_group->group[i] = nullptr;
	first_group->info = 9893;
	bool exit = 1;
	load_data_from_database(first_group, last_group, (char*)"students_service.txt");
	Conf* conference2020 = new Conf[100];
	Conf* c = nullptr;
	int N_conf = 0;
	ofstream file;

	while (exit) {
		first:
		system("cls");
		cout << "1 -  Создание новой записи о студенте\n";
		cout << "2 -  Внесение изменений в уже имеющуюся запись\n";
		cout << "3 -  Вывести информацию о всех студентах\n";
		cout << "4 -  Вывод информации о всех студентов группы N\n";
		cout << "5 -  Вывод топа самых умных студентов с наивысшим по рейтингу среднем баллом за прошедшую сессию\n";
		cout << "6 -  Вывод количества студентов мужского и женского пола\n";
		cout << "7 -  Определение количества студентов, которые будут получать стипендию (стипендия начисляется если у студента нет 3 и форма обучения - очная)\n";
		cout << "8 -  Вывод данных о студентах, которые не получают стипендию; учатся только на «хор»; учатся на «хор» и «отл»; учатся только на «отл»\n";
		cout << "9 -  Вывод всех записей, сделанных в день, который введёт пользователь. Вывод всех записей, сделанных после полудня.Вывод всех записей, сделанных до полудня\n";
		cout << "10 - Вывод данных о студентах, имеющий номер в списке – k\n";
		cout << "11 - Вывод данных о всех студентах в файл students и обновление базы данных\n";
		cout << "12 - Добавить новую группу студентов\n";
		cout << "13 - Переход к следующей части задания\n";

		int choise;
		cin >> choise;
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		int group_info;//Номер группы
		student_groups* groups;//Найденные группы помещаем сюда

		switch (choise)
		{
		case 1:
			cout << "Номер группы? ";
			cin >> group_info;
			groups = search_group(first_group, group_info);

			if (groups != 0) {
				int student_group_number;
				cout << "Номер студента в группе: ";
				do {
					cin >> student_group_number;
					if (!(student_group_number > 0 and student_group_number < 31))
						cout << "Максимальное число студентов в группе - 30, минимальное - 1\n";
					cin.clear();
					cin.ignore(cin.rdbuf()->in_avail());
				} while (!(student_group_number > 0 and student_group_number < 31));
				new_student_record(groups->group, student_group_number - 1, group_info);
			}
			else cout << "Группы нет в списке\n";
			break;

		case 2:
			cout << "Номер группы? ";
			cin >> group_info;
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
			groups = search_group(first_group, group_info);

			if (groups != 0) {
				int student_group_number;
				cout << "Номер студента в группе: ";
				do {
					cin >> student_group_number;
					if (!(student_group_number > 0 and student_group_number < 31))
						cout << "Максимальное число студентов в группе - 30, минимальное - 1\n";
					cin.clear();
					cin.ignore(cin.rdbuf()->in_avail());
				} while (!(student_group_number > 0 and student_group_number < 31));
				data_change(first_group->group, student_group_number - 1);
			}
			else cout << "Группы нет в списке\n";
			break;

		case 3:
			Display_all_student_data(first_group);
			break;

		case 4:
			cout << "Номер группы? ";
			cin >> group_info;
			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
			groups = search_group(first_group, group_info);

			if (groups != 0) {
				for (int i = 0; i < 30; i++) {
					if (groups->group[i] != nullptr)
						groups->group[i]->Display_data();
				}
			}
			else cout << "Группы нет в списке\n";
			break;

		case 5:
			top_students(first_group);
			break;

		case 6:
			students_gender(first_group);
			break;

		case 7:
			scholarship_students(first_group);
			break;

		case 8:
			students_without_scholarship(first_group);
			break;

		case 9:
			students_recording_time(first_group);
			break;

		case 10:
			students_by_number(first_group);
			break;

		case 11:
			data_output_to_file(first_group);
			system("students.txt");
			break;

		case 12:
			new_group(last_group);
			break;

		case 13:
			exit = 0;
			break;
		}

		cout << endl;
		system("Pause");
	}

	//загрузка из файла конференции
	load_data_from_database(conference2020, (char*)"conf.txt", N_conf);
	do {
		system("cls");
		cout << "На территории университета скоро пройдет научная конференция.\n"
			<< "Создайте структуру, в которой будет храниться информация о гостях,\n"
			<< "которые приглашены на неё : ФИО, статус(выступающий / слушатель),\n"
			<< "ученная степень, наименование университета, из которого прибыли,\n"
			<< "тема выступления, если статус – выступающий.\n"
			<< "Внесите информацию о студентах в достаточном количестве.\n"
			<< "Выведите информацию : о всех выступающих, о всех иногородних\n"
			<< "приглашенных, о всех студентах, которые приглашены выступать, о всех\n"
			<< "студентах из университета, в котором проходит конференция,\n"
			<< "принимающих участие в конференции,(по умолчанию это университет «ЛЭТИ»)\n\n";
		cout << "1 - Добавить нового участника конференции\n";
		cout << "2 - Вывести информацию о всех участниках конференции\n";
		cout << "3 - Вывести информацию о всех выступающих\n";
		cout << "4 - Вывести информацию о всех иногородних приглашенных\n";
		cout << "5 - Вывести информацию о всех приглашенных выступающих студентах\n";
		cout << "6 - Вывести информацию о всех участниках из «ЛЭТИ»\n";
		cout << "7 - Сохранить изменения в файл conf\n";
		cout << "8 - Перейти к первой части программы\n";
		int choise;
		cin >> choise;
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());

		switch (choise) {
		case 1:
			new_conf_record(conference2020, N_conf);
			break;
			
		case 2:
			output_conference_participants(conference2020, N_conf, 0, 0, 0, 0, -1, 0, 0, -1, -1);
			break;

		case 3:
			output_conference_participants(conference2020, N_conf, 0, 0, 0, 0, 1, 0, 0, -1, -1);
			break;

		case 4:
			output_conference_participants(conference2020, N_conf, 0, 0, 0, 0, -1, 0, (char*)"SPB", 1, 7);
			break;

		case 5:
			output_conference_participants(conference2020, N_conf, 0, 0, 0, 0, 1, 0, 0, 1, -1);
			break;

		case 6:
			output_conference_participants(conference2020, N_conf, 0, 0, 0, (char*)"LETI", -1, 0, 0, -1, -1);
			break;

		case 7:
			file.open("conf.txt");
			file.close();
			c = conference2020;
			for (int i = 0; i < N_conf; i++) {
				put_in_service_file((char*)"conf.txt", c);
				c++;
			}
			break;

		case 8:
			goto first;
			break;
		}
		system("Pause");
	} while (1);

	return 0;
}


void Conf::Display_data() {
	cout << FIO << endl;
	cout << gender << endl;
	cout << "Ученая степень: " << academic_degree << endl;
	cout << "Университет: " << university << endl;
	if (performance_status) {
		cout << "Выступающий\n";
		cout << "Тема выступления: " << topic_of_speech << endl;
	}
	else
		cout << "Не выступающий\n";
	cout << "Город проживания: " << hometown << endl;
	if (invited)
		cout << "Приглашенный\n";
	else
		cout << "Студент «ЛЭТИ»\n";
	cout << endl;
}

void new_conf_record(Conf* c, int& N) {
	cout << "Введите фио: ";
	cin.getline(c[N].FIO, 49);
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	do {
		cout << "Пол: ";
		cin >> c[N].gender;
		if (!(c[N].gender == 'm' or c[N].gender == 'f'))
			cout << "Допускаются варианты 'm' или 'f'\n";
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
	} while (!(c[N].gender == 'm' or c[N].gender == 'f'));
	cout << "Ученая степень: ";
	cin.getline(c[N].academic_degree, 50);
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	cout << "Университет: ";
	cin.getline(c[N].university, 50);
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	cout << "1 - выступающий, 0 - слушатель: ";
	cin >> c[N].performance_status;
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	if (c[N].performance_status) {
		cout << "Тема выступления: ";
		cin.getline(c[N].topic_of_speech, 50);
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
	}
	cout << "Город проживания: ";
	cin.getline(c[N].hometown, 50);
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	cout << "1 - приглашенный, 0 - студент «ЛЭТИ»: ";
	cin >> c[N].invited;
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());

	N++;
}

void load_data_from_database(Conf* conference, char* filename, int& N_conf)
{
	ifstream file(filename, ios_base::binary);
	file.seekg(0, ios::end);
	int size = file.tellg();
	N_conf = size / sizeof(Conf);
	file.clear();
	file.seekg(0);
	for (int i = 0; i < N_conf; i++) {
		char* c = (char*)conference;
		for (int j = 0; j < sizeof(Conf); j++) {
			file.get(*c);
			c++;
		}
	}

	file.close();
}

void output_conference_participants(Conf* c, int N, char* FIO, char* gender, char* academic_degree, char* university, int performance_status, char* topic_of_speech, char* hometown, int invited, int negation) {
	for (int i = 0; i < N; i++) {
		bool out = 1;
		if (FIO != 0 and !((!strcmp(c[i].FIO, FIO)) == (negation != 1)))
			out = 0;
		if (gender != 0 and (c[i].gender != *gender))
			out = 0;
		if (academic_degree != 0 and !((!strcmp(c[i].academic_degree, academic_degree)) == (negation != 3)))
			out = 0;
		if (university != 0 and !((!strcmp(c[i].university, university)) == (negation != 4)))
			out = 0;
		if (performance_status != -1 and (c[i].performance_status != performance_status))
			out = 0;
		if (topic_of_speech != 0 and !((!strcmp(c[i].topic_of_speech, topic_of_speech)) == (negation != 6)))
			out = 0;
		if (hometown != 0 and !((!strcmp(c[i].hometown, hometown)) == (negation != 7)))
			out = 0;
		if (invited != -1 and (c[i].invited != invited))
			out = 0;

		if (out)
			c[i].Display_data();
	}
}