#include <iostream>
#include <sstream>
#include <Windows.h>
#include <vector>

#define uint unsigned int // сокращение записи "unsigned int" в "uint"

class Fraction {

private:

	uint first;  // Числитель.
	uint second; // Знаменатель.

public:

	// Вместо Read()
	void init_from_input() {
		std::cout << "Введите числитель: ";
		std::cin >> first;
		std::cout << "Введите знаменатель: ";
		std::cin >> second;
	}

	// Вместо Display()
	void print() {
		std::cout << "Числитель: " << first << std::endl;
		std::cout << "Знаменатель: " << second << std::endl;
		std::cout << "Дробный вид: " << first << "/" << second << std::endl;
	}

	void nesokr() {
		uint NOD = 0;
		uint this_first = first;
		uint this_second = second;
		while (this_first != 0 && this_second != 0) {
			if (this_first > this_second) {
				this_first = this_first % this_second;
			}
			else {
				this_second = this_second % this_first;
			}
		}
		NOD = this_first + this_second;
		std::cout << "(nesokr) НОД: " << NOD << std::endl;
		first /= NOD;
		second /= NOD;
	}

	Fraction() {
		first = 0;
		second = 0;
	}
};

class Goods {

private:

	char* name;
	char* registration_date;
	uint price;
	uint amount;
	uint waybill_number;

public:
	
	// Вместо Read()
	void init_from_input() {
		std::cout << "Введите наименование товара: ";
		std::cin >> name;
		std::cout << "Дата регистрации: ";
		std::cin >> registration_date;
		std::cout << "Цена: ";
		std::cin >> price;
		std::cout << "Количество: ";
		std::cin >> amount;
		std::cout << "Номер накладной: ";
		std::cin >> waybill_number;
	}

	// Вместо Display()
	void print() {
		std::cout << "Информация о товаре:" << std::endl;
		std::cout << "Наименование: " << name << std::endl;
		std::cout << "Дата регистрации: " << registration_date << std::endl;
		std::cout << "Цена: " << price << std::endl;
		std::cout << "Количество: " << amount << std::endl;
		std::cout << "Номер накладной: " << waybill_number << std::endl;
		std::cout << "Стоимость: " << get_cost() << std::endl;
	}

	void set_waybill_number(uint new_waybill_number) { waybill_number = new_waybill_number; }

	uint get_waybill_number() { return waybill_number; }

	void set_registration_date(char* new_registration_date) { registration_date = new_registration_date; }

	char* get_registration_date() { return registration_date; }

	void set_name(char* name) { this->name = name; }

	char* get_name() { return name;  }

	void set_price(uint new_price) { price = new_price; }

	uint get_price() { return price; }

	void add_amount(uint add_amount) { amount += add_amount; }

	void substract_amount(uint substract_amount) {
		if (amount - substract_amount > amount) amount = 0;
		else amount -= substract_amount;
	}

	void add_price(uint add_price) { this->price += add_price; }

	void substract_price(uint substract_price) {
		if ((price - substract_price) > price) price = 0;
		else price -= substract_price;
	}

	void set_amount(uint new_amount) { amount = new_amount;}

	uint get_amount() { return amount; }

	uint get_cost() { return amount * price; }

	void print_cost() { printf("%s (%u шт.) по цене %u руб. - %u руб.\n", name, amount, price, get_cost()); }

	Goods() {
		name = new char[33];
		registration_date = new char[11];
		price = 0;
		amount = 0;
		waybill_number = 0;
	}

	~Goods() {
		delete name;
		delete registration_date;
	}
};

void print_menu() {
	printf(
	"------------------------------\n"
	"          Команды:\n"
	"> name <get/set> [<new_name>]                                                - Изменение / Вывод наименования товара.\n"
	"> amount <set/get/add/substract> [<new_amount/add_amount/substract_amount>]  - Изменение / Вывод количества товара.\n"
	"> price <set/get/add/substract> [<new_price/add_price/substract_price>]      - Изменение / Вывод цены товара.\n"
	"> registration_date <set/get> [<new_registration_date>]                      - Изменение / Вывод даты регистрации товара.\n"
	"> waybill_number <get/set> [<new_waybill_number>]                            - Изменение / Вывод номера накладной товара.\n"
	"> cost                                                                       - Вывод стоимости товара.\n"
	"------------------------------\n");
}

inline void clear_console() { system("cls"); }

inline bool wait_and_clear() {
	std::cout << "(Нажмите ВВОД)";
	std::cin.get();
	clear_console();
	return true;
}

// Реализация взаймодествия с данными
// используя ввод/вывод команд через консоль.
bool handle_input(Goods *product) {
	std::string input;
	std::vector<std::string> arguments;
	
	int arguments_size = 128;

	printf("> ");
	std::getline(std::cin, input);

	// Разделения единой строки на отдельные части
	uint input_end = input.find_first_of(' ');
	while (input_end != -1) {
		arguments.push_back(input.substr(0, input_end));
		input.erase(0, input_end + 1);
		input_end = input.find_first_of(' ');
	}
	arguments.push_back(input);

	if (arguments[0] == "name") { // @Название
		if (arguments[1] == "set") { // name set <...>
			if (!arguments[2].empty()) {
				product->set_name((char*) arguments[2].c_str());
				printf("Новое наименование товара: %s\n", product->get_name());
			} else {
				printf("Вы не ввели новое наименование товара!\n");
			}
		} else if (arguments[1] == "get") { // name get
			printf("Наименование товара: %s\n", product->get_name());
		}
	} else if (arguments[0] == "amount") { // @Количество
		if (arguments[1] == "set") { // amount set <...>
			if (!arguments[2].empty()) {
				product->set_amount(std::stoul(arguments[2]));
				printf("Новое количество товара: %u\n", product->get_amount());
			} else {
				printf("Вы не ввели новое количество товара!\n");
			}
		} else if (arguments[1] == "get") { // amount get
			printf("Количество товара: %u\n", product->get_amount());
		} else if (arguments[1] == "add") { // amount add <...>
			if (!arguments[2].empty()) {
				product->add_amount(std::stoul(arguments[2]));
				printf("Новое количество товара: %u\n", product->get_amount());
			} else {
				printf("Вы не ввели количество добавляемого товара!\n");
			}
		} else if (arguments[1] == "substract") { // amount substract <...>
			if (!arguments[2].empty()) {
				product->substract_amount(std::stoul(arguments[2]));
				printf("Новое количество товара: %u\n", product->get_amount());
			} else {
				printf("Вы не ввели количество вычитаемого товара!\n");
			}
		}
	} else if (arguments[0] == "price") { // @Цена
		if (arguments[1] == "set") { // price set <...>
			if (!arguments[2].empty()) {
				product->set_price(std::stoul(arguments[2]));
				printf("Новая цена товара: %u\n", product->get_price());
			} else {
				printf("Вы не ввели новое наименование товара!\n");
			}
		} else if (arguments[1] == "get") { // price get
			printf("Цена товара: %u\n", product->get_price());
		} else if (arguments[1] == "add") { // price add <...>
			if (!arguments[2].empty()) {
				product->add_price(std::stoul(arguments[2]));
				printf("Новая цена товара: %u\n", product->get_price());
			} else {
				printf("Вы не ввели количество добавляемой цены товара!\n");
			}
		} else if (arguments[1] == "substract") { // price subtract <...>
			if (!arguments[2].empty()) {
				product->substract_price(std::stoul(arguments[2]));
				printf("Новая цена товара: %u\n", product->get_price());
			} else {
				printf("Вы не ввели количество вычитаемой цены товара!\n");
			}
		}
	} else if (arguments[0] == "registration_date") { // @Дата_Регистрации
		if (arguments[1] == "set") { // registration_date set <...>
			if (!arguments[2].empty()) {
				product->set_registration_date((char*) arguments[2].c_str());
				printf("Новая дата регистрации товара: %s\n", product->get_registration_date());
			} else {
				printf("Вы не ввели новую дату регистрации товара!\n");
			}
		} else if (arguments[1] == "get") { // registration_date get
			printf("Дата регистрации товара: %s\n", product->get_registration_date());
		}
	} else if (arguments[0] == "waybill_number") { // @Номер_Накладной
		if (arguments[1] == "set") { // waybill_number set <...>
			if (!arguments[2].empty()) {
				product->set_waybill_number(std::stoul(arguments[2]));
				printf("Новый номер накладной товара: %u\n", product->get_waybill_number());
			} else {
				printf("Вы не ввели новый номер накладной товара!\n");
			}
		} else if (arguments[1] == "get") { // waybill_number get
			printf("Номер накладной товара: %u\n", product->get_waybill_number());
		}
	} else if (arguments[0] == "cost") { // @Стоимость
		product->print_cost();
	} else if (arguments[0] == "exit") { // @Выход
		return false;
	} else {
		printf("Неизвестная команда %s!\n", arguments[0].c_str());
	}

	return wait_and_clear();
}

int mainn() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	printf("Первая часть (1:17):\n\n");

	Fraction fraction;
	fraction.init_from_input(); // Read()
	fraction.nesokr();
	fraction.print(); // Display()

	printf("\nВторая часть (2:14):\n\n");

	Goods product;
	product.init_from_input(); // Read()
	product.print(); // Display()

	// Игнорируем \n после прошлых std::cin,
	// так как из-за него std::getline пропускает ввод.
	std::cin.ignore(UINT16_MAX, '\n');
	do {
		print_menu();
	} while (handle_input(&product));

	system("pause");
	return 0;
}