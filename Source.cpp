#include <iostream>
#include <fstream>
#include <string>
#include "dependencies/include/nlohmann/json.hpp"
#include <filesystem>
#include <libpq-fe.h>
#include <windows.h>
#include <ctime>
#include <iomanip>
#include <sstream>


class Converter
{
public:
	void To_txt(std::string filename)
	{
		std::filesystem::path p(filename);
		filename.erase(filename.size() - 5, 5); // ������� .json
		rename(p, std::string(filename) + ".txt");
	}
	void To_json(std::string filename)
	{
		std::filesystem::path p(filename);
		filename.erase(filename.size() - 4, 4); // ������� .txt
		rename(p, std::string(filename) + ".json");
	}
};


class Checker
{
private:
	double count = 0;
	double num_of_sol = 0; // ���������� �������
	int num_of_comp = 0; // ���������� ������ ���������
	int num_of_equals = 0; // ���������� ������ �����
	int num_of_math_sign = 0; // ���������� �������������� ��������
	int num_of_edges = 0; // ���������� ����������
	int num_of_sign = 0; // ���������� ������������ ������
	int num_of_exclam = 0; // ���������� ������ �����������
public:
	int Check_logic(nlohmann::json objJson)
	{
		int err = 0;

		if (objJson["Vertices"][0]["Content"] != "Start")
		{
			std::cout << "������ � ������������ ����� Start\n" << std::endl;
			err++;
		}

		for (int i = 0; i < size(objJson["Edges"]); i++)
		{
			if (objJson["Edges"][i]["To"] == 1)
			{
				std::cout << "������ � ����� 'To' ��� �������: " << i+1 << std::endl;
				std::cout << "�������� �������� ��������� �������\n" << std::endl;
				err++;
			}

			if (objJson["Edges"][i]["From"] == (size(objJson["Edges"]) - 1))
			{
				std::cout << "������ � ����� 'From' ��� �������: " << i+1 << std::endl;
				std::cout << "�������� �������� �������� �������\n" << std::endl;
				err++;
			}

			int from = objJson["Edges"][i]["From"];
			int to = objJson["Edges"][i]["To"];
			if (objJson["Edges"][i]["Type"] == 0 and objJson["Vertices"][from - 1]["Type"] != 3)
			{
				std::cout << "������ � �������� 'Type', ������ ���� 3, Id " << from << "\n" << std::endl;
				err++;
			}
		}

		for (int i = 0; i < size(objJson["Vertices"]); i++)
		{
			if (objJson["Vertices"][i]["Type"] == 3) //�������� ����� �������
			{
				int id_of_sol = objJson["Vertices"][i]["Id"];
				int total_type = 0;
				num_of_sol += 1;
				for (int i = 0; i < size(objJson["Edges"]); i++)
				{
					if (objJson["Edges"][i]["From"] == id_of_sol and (objJson["Edges"][i]["Type"] == 0 or objJson["Edges"][i]["Type"] == 1))
					{
						total_type += objJson["Edges"][i]["Type"];
						count += 1;
					}
				}

				if (count/num_of_sol != 2.0)
				{
					std::cout << "������ � ���������� ����� ����� �������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					num_of_sol = 0;
					count = 0;
					err++;
				}
				if (total_type == 2) {
					std::cout << "������, ��� ���������� Edges ����� Type = 1" << std::endl;
					std::cout << "������ � From: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (total_type == 0) {
					std::cout << "������, ��� ���������� Edges ����� Type = 0" << std::endl;
					std::cout << "������ � From: " << i + 1 << "\n" << std::endl;
					err++;
				}
				std::string s = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < s.size(); i++)
				{
					if (s[i] == *">" or s[i] == *"<")
					{
						num_of_comp += 1;
						if (s[i + 1] == *"=")
						{
							num_of_equals -= 1; // ������� <= ��� >= �� ���� ���� ���������, ������� ������� ������ �����
						}
					}
					if (s[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (s[i] == *"!")
					{
						num_of_exclam += 1;
					}
				}
				if (num_of_comp >= 2)
				{
					std::cout << "������ � ���������� ������ ���������, ��������: " << num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_exclam >= 2)
				{
					std::cout << "������ � ���������� ������ �����������, ��������: " << num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if ((num_of_equals + num_of_comp) >= 3 and (num_of_comp <= 1))
				{
					std::cout << "������ � ���������� ������ �����, ��������: " << num_of_equals << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if ((num_of_comp == 1) and ((num_of_equals) == 1 or (num_of_equals) == 2))
				{
					std::cout << "������ � ���������� ������ ����� � ���������, ��������: " << num_of_equals + num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				num_of_equals = 0; // �������� ��������
				num_of_comp = 0;
				num_of_exclam = 0;
			}

			if (objJson["Vertices"][i]["Type"] == 2) // �������� ����� ��������
			{
				std::string str = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < str.size(); i++)
				{
					if (str[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (str[i] == *"+" or str[i] == *"*" or str[i] == *"-" or str[i] == *"/" or (str[i] == *"s" and str[i+1] == *"q" and str[i+2] == *"r" and str[i+3] == *"t"))
					{
						num_of_math_sign += 1;
					}
				}
				if (num_of_math_sign > 1)
				{
					std::cout << "������ � ���������� �������������� ��������, ��������: " << num_of_math_sign << std::endl;
					std::cout << "������ � Id: " << i+1 << "\n" << std::endl;
					err++;
				}
				if (num_of_equals > 1)
				{
					std::cout << "������ � ���������� ������ �����, ��������: " << num_of_equals << std::endl;
					std::cout << "������ � Id: " << i+1 << "\n" << std::endl;
					err++;
				}
				num_of_math_sign = 0; // �������� ��������
				num_of_equals = 0;
			}

			if (objJson["Vertices"][i]["Content"] == "Start")
			{
				if (objJson["Vertices"][i]["Type"] != 0)
				{
					std::cout << "������ � �������� Type ����� 'Start'" << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Content"] == "End")
			{
				if (objJson["Vertices"][i]["Type"] != 1)
				{
					std::cout << "������ � �������� Type ����� 'End'" << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Type"] == 0)
			{
				if (objJson["Vertices"][i]["Content"] != "Start")
				{
					std::cout << "������, ������� 0 � �������� Type, Id " << objJson["Vertices"][i]["Id"] << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Type"] == 1)
			{
				if (objJson["Vertices"][i]["Content"] != "End")
				{
					std::cout << "������, �������� 1 � �������� Type, Id " << objJson["Vertices"][i]["Id"] << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Type"] == 4 or objJson["Vertices"][i]["Type"] == 5) //�������� ����� ������
			{
				std::string str = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < str.size(); i++)
				{
					if (str[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (str[i] == *"+" or str[i] == *"*" or str[i] == *"/" or (str[i] == *"s" and str[i + 1] == *"q" and str[i + 2] == *"r" and str[i + 3] == *"t"))
					{
						num_of_math_sign += 1;
					}
					if (str[i] == *";" or str[i] == *"'" or str[i] == *":")
					{
						num_of_sign += 1;
					}
				}
				if (num_of_math_sign > 0)
				{
					std::cout << "������, ������� �������������� �������� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_equals > 0)
				{
					std::cout << "������, ������� ����� ����� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_sign > 0)
				{
					std::cout << "������, ������� ������������ ����� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				num_of_math_sign = 0; // �������� ��������
				num_of_equals = 0;
				num_of_sign = 0;
			}
		}

		if (err == 0) {
			std::cout << "������ ���" << std::endl;
		}

		return err;
	}
	int Check_terminal_blocks(nlohmann::json objJson)
	{
		int err = 0;

		if (objJson["Vertices"][0]["Content"] != "Start")
		{
			std::cout << "������ � ������������ ����� Start\n" << std::endl;
			err++;
		}

		for (int i = 0; i < size(objJson["Vertices"]); i++) {
			if (objJson["Vertices"][i]["Content"] == "Start")
			{
				if (objJson["Vertices"][i]["Type"] != 0)
				{
					std::cout << "������ � �������� Type ����� 'Start'" << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Content"] == "End")
			{
				if (objJson["Vertices"][i]["Type"] != 1)
				{
					std::cout << "������ � �������� Type ����� 'End'" << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Type"] == 0)
			{
				if (objJson["Vertices"][i]["Content"] != "Start")
				{
					std::cout << "������, ������� 0 � �������� Type, Id " << objJson["Vertices"][i]["Id"] << "\n" << std::endl;
					err++;
				}
			}

			if (objJson["Vertices"][i]["Type"] == 1)
			{
				if (objJson["Vertices"][i]["Content"] != "End")
				{
					std::cout << "������, �������� 1 � �������� Type, Id " << objJson["Vertices"][i]["Id"] << "\n" << std::endl;
					err++;
				}
			}
		}

		if (err == 0) {
			std::cout << "������ � ������������ ������ ���" << std::endl;
		}

		return err;
	};
	int Check_process_blocks(nlohmann::json objJson)
	{
		int err = 0;

		for (int i = 0; i < size(objJson["Vertices"]); i++) {
			if (objJson["Vertices"][i]["Type"] == 2) // �������� ����� ��������
			{
				std::string str = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < str.size(); i++)
				{
					if (str[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (str[i] == *"+" or str[i] == *"*" or str[i] == *"-" or str[i] == *"/" or (str[i] == *"s" and str[i + 1] == *"q" and str[i + 2] == *"r" and str[i + 3] == *"t"))
					{
						num_of_math_sign += 1;
					}
				}
				if (num_of_math_sign > 1)
				{
					std::cout << "������ � ���������� �������������� ��������, ��������: " << num_of_math_sign << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_equals > 1)
				{
					std::cout << "������ � ���������� ������ �����, ��������: " << num_of_equals << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				num_of_math_sign = 0; // �������� ��������
				num_of_equals = 0;
			}
		}

		if (err == 0) {
			std::cout << "������ � ������ ��������� ���" << std::endl;
		}

		return err;
	}
	int Check_solution_blocks(nlohmann::json objJson)
	{
		int err = 0;

		for (int i = 0; i < size(objJson["Edges"]); i++) {
			int from = objJson["Edges"][i]["From"];
			int to = objJson["Edges"][i]["To"];
			if (objJson["Edges"][i]["Type"] == 0 and objJson["Vertices"][from - 1]["Type"] != 3)
			{
				std::cout << "������ � �������� 'Type', ������ ���� 3, Id " << from << "\n" << std::endl;
				err++;
			}
		}

		for (int i = 0; i < size(objJson["Vertices"]); i++) {
			if (objJson["Vertices"][i]["Type"] == 3) //�������� ����� �������
			{
				int id_of_sol = objJson["Vertices"][i]["Id"];
				int total_type = 0;
				num_of_sol += 1;
				for (int i = 0; i < size(objJson["Edges"]); i++)
				{
					if (objJson["Edges"][i]["From"] == id_of_sol and (objJson["Edges"][i]["Type"] == 0 or objJson["Edges"][i]["Type"] == 1))
					{
						total_type += objJson["Edges"][i]["Type"];
						count += 1;
					}
				}

				if (count / num_of_sol != 2.0)
				{
					std::cout << "������ � ���������� ����� ����� �������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					num_of_sol = 0;
					count = 0;
					err++;
				}
				if (total_type == 2) {
					std::cout << "������, ��� ���������� Edges ����� Type = 1" << std::endl;
					std::cout << "������ � From: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (total_type == 0) {
					std::cout << "������, ��� ���������� Edges ����� Type = 0" << std::endl;
					std::cout << "������ � From: " << i + 1 << "\n" << std::endl;
					err++;
				}
				std::string s = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < s.size(); i++)
				{
					if (s[i] == *">" or s[i] == *"<")
					{
						num_of_comp += 1;
						if (s[i + 1] == *"=")
						{
							num_of_equals -= 1; // ������� <= ��� >= �� ���� ���� ���������, ������� ������� ������ �����
						}
					}
					if (s[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (s[i] == *"!")
					{
						num_of_exclam += 1;
					}
				}
				if (num_of_comp >= 2)
				{
					std::cout << "������ � ���������� ������ ���������, ��������: " << num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_exclam >= 2)
				{
					std::cout << "������ � ���������� ������ �����������, ��������: " << num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if ((num_of_equals + num_of_comp) >= 3 and (num_of_comp <= 1))
				{
					std::cout << "������ � ���������� ������ �����, ��������: " << num_of_equals << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if ((num_of_comp == 1) and ((num_of_equals) == 1 or (num_of_equals) == 2))
				{
					std::cout << "������ � ���������� ������ ����� � ���������, ��������: " << num_of_equals + num_of_comp << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				num_of_equals = 0; // �������� ��������
				num_of_comp = 0;
				num_of_exclam = 0;
			}
		}

		if (err == 0) {
			std::cout << "������ � ������ ������� ���" << std::endl;
		}

		return err;
	}
	int Check_data_blocks(nlohmann::json objJson)
	{
		int err = 0;

		for (int i = 0; i < size(objJson["Vertices"]); i++) {
			if (objJson["Vertices"][i]["Type"] == 4 or objJson["Vertices"][i]["Type"] == 5) //�������� ����� ������
			{
				std::string str = objJson["Vertices"][i]["Content"];
				for (int i = 0; i < str.size(); i++)
				{
					if (str[i] == *"=")
					{
						num_of_equals += 1;
					}
					if (str[i] == *"+" or str[i] == *"*" or str[i] == *"/" or (str[i] == *"s" and str[i + 1] == *"q" and str[i + 2] == *"r" and str[i + 3] == *"t"))
					{
						num_of_math_sign += 1;
					}
					if (str[i] == *";" or str[i] == *"'" or str[i] == *":")
					{
						num_of_sign += 1;
					}
				}
				if (num_of_math_sign > 0)
				{
					std::cout << "������, ������� �������������� �������� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_equals > 0)
				{
					std::cout << "������, ������� ����� ����� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				if (num_of_sign > 0)
				{
					std::cout << "������, ������� ������������ ����� � ����� ������" << std::endl;
					std::cout << "������ � Id: " << i + 1 << "\n" << std::endl;
					err++;
				}
				num_of_math_sign = 0; // �������� ��������
				num_of_equals = 0;
				num_of_sign = 0;
			}
		}

		if (err == 0) {
			std::cout << "������ � ������ ������ ���" << std::endl;
		}

		return err;
	}
	bool Check_structure(std::string content) {
		int counter_id = 0;
		int total_id = 0;
		int index = 0;
		int step = 0; 
		int from = 0;
		int to = 0; 
		int j = 0;
		int k = 0;
		int num_of_from = 0;
		int number = 1;
		std::string temp;
		std::string text;
		std::string type_of_vertice;

		for (k; k < content.size(); k++) {
			if (content[k] == 'I' and content[k + 1] == 'd') {
				total_id++;
			}
		}
		for (int i = 0; i < content.size(); i++) {
				if (content[i] == ' ' or content[i] == char(9)) continue;
				else {
					if (content[i] != '{' and (index == 0 or index == (5 + step))) {
						if (index == 0) {
							if (content[i] != '"') {
								std::cout << "������� �������� ������ � ������ ����-�����: " << content[i] << std::endl;
								return false;
							}
							else {
								std::cout << "������, ����������� { � ������ ����-�����" << std::endl;
								return false;
							}
						}
						if (index == (5 + step)) {
							if (content[i] != '"') {
								std::cout << "������� �������� ������ ����� {: " << content[i] << std::endl;
								std::cout << "��� Id " << counter_id + 1 << std::endl;
								return false;
							}
							else {
								std::cout << "������, ����������� { ��� Id " << counter_id + 1 << std::endl;
								return false;
							}
						}
					}
					else {
						i++;
						index++;
						while (content[i] == ' ' or content[i] == char(9)) i++;
						if ((content[i + 1] == ' ' or content[i + 1] == char(9)) and text == "End" and from == 0 and to == 0) {
							while (content[i + 1] == ' ' or content[i + 1] == char(9)) i++;
						}
						if (content[i + 1] == ']' and text == "End" and content[i] != ']' and !isalpha(content[i]) and !isdigit(content[i])) {
							j = i + 2;
							k = j;
							index = 0; // ��������� � ����� "Edges"
							step = 0;
							break;
						}
						else {
							if (content[i + 1] != ']' and text == "End" and content[i] != ']' and !isalpha(content[i]) and !isdigit(content[i]) and counter_id == total_id) {
								std::cout << "������, ����������� ] ����� }, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
								return false;
							}
						}
					}

					if (content[i] == '"' and index == 1) {
						i++;
						while ((isalpha(content[i]) or isdigit(content[i]))) {
							temp += content[i];
							i++;
						}
						if (temp != "Vertices" or content[i] == ' ') {
							std::cout << "������ � ������������ ����� Vertices" << std::endl;
							return false;
						}
						index++;
						if (content[i] == '"' and index != 1) {
							i--;
						}
						else {
							std::cout << "������, ����������� ����������� ������� ��� �����: " << std::endl;
							std::cout << temp << std::endl;
							return false;
						}
					}
					else {
						if (index == 1) {
							std::cout << "������, ����������� ����������� ������� ��� ������� �������� ������ ����� ������ Vertices" << std::endl;
							return false;
						}
					}

					if (content[i] == ':' and index == 3) {
						i++;
						index++;
						while (content[i] == char(9) or content[i] == char(32)) {
							i++;
						}
						if (content[i] == '[' and index == 4) { index++; }
						else {
							if (index == 4) {
								std::cout << "������, ����������� [ �����: " << std::endl;
								std::cout << '"' + temp + '"' + ':' << std::endl;
								return false;
							}
						}
					}
					else {
						if (index == 3) {
							std::cout << "������, ����������� ��������� �����: " << std::endl;
							std::cout << '"' + temp + '"' << std::endl;
							return false;
						}
					}

					if (content[i] == '"' and index == (6 + step))
					{
						i++;
						temp = "";
						while (isalpha(content[i]) or isdigit(content[i])) {
							temp += content[i];
							i++;
						}
						if (temp != "Id" or content[i] == ' ') {
							std::cout << "������ � ������������ ����� Id " << counter_id + 1 << std::endl;
							return false;
						}
						index++;
						if (content[i] == '"' and index != (6 + step)) {
							counter_id++;
							i--;
						}
						else {
							std::cout << "������, ����������� ����������� ������� ��� Id " << counter_id + 1 << std::endl;
							return false;
						}
					}
					else {
						if (index == (6 + step)) {
							std::cout << "������, ����������� ����������� ������� ��� Id " << counter_id + 1 << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == ':' and (index == (8 + step) or index == (12 + step))) {
						i++;
						temp = "";
						while (content[i] == ' ' or content[i] == char(9)) i++;
						while (isdigit(content[i]) or isalpha(content[i])) {
							if (isalpha(content[i])) {
								if (index == (8 + step)) {
									std::cout << "������, ������������ ����� ��� Id " << counter_id << std::endl;
									return false;
								}
								if (index == (12 + step)) {
									std::cout << "������, ������������ ����� ��� Type, Id " << counter_id << std::endl;
									return false;
								}
							}
							temp += content[i];
							i++;
						}
						if (temp == "") {
							if (index == (12 + step)) {
								std::cout << "������, ������ �������� � ����� Type, Id " << counter_id << std::endl;
								return false;
							}
						}
						if (index == 12 + step) type_of_vertice = temp;
						index++;
						while (content[i] == ' ' or content[i] == char(9)) i++;
						if (content[i] == ',' and index != (8 + step)) { // �� ����� �������� ��� ����� Type
							if (temp != std::to_string(counter_id) and index != (13 + step)) {
								std::cout << "������ � ���������� ������ Id" << std::endl;
								std::cout << "��������: " << temp << std::endl;
								std::cout << "������ ����: " << counter_id << std::endl;
								return false;
							}
							else { i--; }
						}
						else {
							if (index == (9 + step)) {
								std::cout << "������, ����������� ������� ����� ������ Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
								return false;
							}
							if (index == (13 + step)) {
								std::cout << "������, ����������� ������� ����� ����� Type ��� Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
								return false;
							}
							std::cout << "������, ����������� ������� ��� Id " << counter_id << std::endl;
							return false;
						}
					}
					else {
						if (index == (8 + step)) {
							std::cout << "������, ����������� ��������� ����� Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
						if (index == (12 + step)) {
							std::cout << "������, ����������� ��������� ����� Type ��� Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == '"' and index == (10 + step))
					{
						i++;
						temp = "";
						while (isalpha(content[i]) or isdigit(content[i])) {
							temp += content[i];
							i++;
						}
						if (temp != "Type" or content[i] == ' ') {
							std::cout << "������ � ������������ ����� Type ��� Id " << counter_id << std::endl;
							return false;
						}
						index++;
						if (content[i] == '"' and index != (10 + step)) {
							i--;
						}
						else {
							std::cout << "������, ����������� ����������� ������� ��� ����� Type, Id " << counter_id << std::endl;
							return false;
						}
					}
					else {
						if (index == (10 + step)) {
							std::cout << "������, ����������� ����������� ������� ��� ����� Type ��� �������� ������ ������� ����� ���, Id " << counter_id << std::endl;
							return false;
						}
					}

					if (content[i] == '"' and index == (14 + step))
					{
						i++;
						temp = "";
						while (isalpha(content[i]) or isdigit(content[i])) {
							temp += content[i];
							i++;
						}
						if (temp != "Content" or content[i] == ' ') {
							std::cout << "������ � ������������ ����� Content ��� Id " << counter_id << std::endl;
							return false;
						}
						index++;
						if (content[i] == '"' and index != (14 + step)) {
							i--;
						}
						else {
							std::cout << "������, ����������� ����������� ������� ��� ����� Content, Id " << counter_id << std::endl;
							return false;
						}
					}
					else {
						if (index == (14 + step)) {
							std::cout << "������, ����������� ����������� ������� ��� ����� Content, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == ':' and index == (16 + step)) { i--; }
					else {
						if (index == (16 + step)) {
							std::cout << "������, ����������� ��������� ����� ����� Content, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == '"' and index == (17 + step)) {
						i++;
						temp = "";
						while (content[i] != '"') {
							if (content[i] == '{' or content[i] == '}' or content[i] == ':' or content[i] == ';') {
								std::cout << "������� ������������ ������ � ����� Content: " << content[i] << ", � Id " << counter_id << std::endl;
								std::cout << "�������� ����������� ����������� ������� ��� ����������� ����� Content" << std::endl;
								return false;
							}
							temp += content[i];
							i++;
						}
						text = temp;
						index++;
						if (content[i] == '"' and index != (17 + step)) { i--; }
					}
					else {
						if (index == (17 + step)) {
							std::cout << "������, ����������� ����������� ������� ��� ����������� ����� Content, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == '}' and index == (19 + step)) { i--; }
					else {
						if (index == (19 + step)) {
							std::cout << "������, ����������� } ����� ����������� ����� Content, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}

					if (content[i] == ',' and index == (20 + step)) {
						step = step + 16;
						index++;
					}
					else {
						if (index == (20 + step)) {
							if (counter_id != total_id) {
								std::cout << "������, ����������� ������� ����� }, Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
								return false;
							}
							else {
								std::cout << "������, � ���������� ����� Content, Id " << counter_id << " ������ ���� End" << std::endl;
								return false;
							}
						}
					}
				}
		}

		for (k; k < content.size(); k++) {
			if (content[k] == 'F' and content[k + 1] == 'r' and content[k + 2] == 'o' and content[k + 3] == 'm') {
				num_of_edges++;
			}
		}

		for (j; j < content.size(); j++) {
			if (content[j] == ' ' or content[j] == char(9)) continue;
			else {
				if (content[j] != ',' and (index == 0)) {
					if (index == 0) {
						std::cout << "������, ����������� ������� ����� ], Id " << counter_id << " ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}
				else {
					j++;
					index++;
					while (content[j] == ' ' or content[j] == char(9)) j++;
				}

				if (content[j] == '"' and index == 1) {
					j++;
					temp = "";
					while (isalpha(content[j]) or isdigit(content[j])) {
						temp += content[j];
						j++;
					}
					if (temp != "Edges" or content[j] == ' ') {
						std::cout << "������ � ������������ ����� Edges";
						return false;
					}
					index++;
					if (content[j] == '"' and index != 1) {
						j--;
					}
					else {
						std::cout << "������, ����������� ����������� ������� ��� �����: " << std::endl;
						std::cout << temp << std::endl;
						return false;
					}
				}
				else {
					if (index == 1) {
						std::cout << "������, ����������� ����������� ������� ��� ������� �������� ������ ����� ������ Edges" << std::endl;
						return false;
					}
				}

				if (content[j] == ':' and index == 3) {
					j++;
					index++;
					while (content[j] == char(9) or content[j] == char(32)) {
						j++;
					}
					if (content[j] == '[' and index == 4) { 
						index++;
						j++;
						while (content[j] == char(9) or content[j] == char(32)) { j++; }
					}
					else {
						if (index == 4) {
							std::cout << "������, ����������� [ �����  " << '"' + temp + '"' << " ��� �������� ������ ������� ����� ���" << std::endl;
;							return false;
						}
					}
				}
				else {
					if (index == 3) {
						std::cout << "������, ����������� ��������� �����  " << '"' + temp + '"' << " ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}

				if (content[j] == '{' and index == (5 + step))
				{
					j++;
					index++;
					while (content[j] == char(9) or content[j] == char(32)) { j++;}
				}
				else {
					if (index == (5)) {
						std::cout << "������, ����������� { ����e [ � ������ ����� Edges ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
					
				}
				
				if (content[j] == '"' and index == (6 + step))
				{
					j++;
					temp = "";
					while (isalpha(content[j]) or isdigit(content[j])) {
						temp += content[j];
						j++;
					}
					if (temp != "From" or content[j] == ' ') {
						std::cout << "������ � ������������ " << number << " ����� From";
						return false;
					}
					index++;
					if (content[j] == '"' and index != (6 + step)) {
						j--;
						num_of_from++;
					}
					else {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� From" << std::endl;
						return false;
					}
				}
				else {
					if (index == (6 + step)) {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� From ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}

				if (content[j] == ':' and (index == (8 + step) or index == (12 + step) or index == (16 + step))) {
					j++;
					temp = "";
					while (content[j] == ' ' or content[j] == char(9)) j++;
					while (isdigit(content[j]) or isalpha(content[j])) {
						if (isalpha(content[j])) {
							if (index == (8 + step)) {
								std::cout << "������, ������������ ����� ��� From ��� ������� " << number << std::endl;
								return false;
							}
							if (index == (12 + step)) {
								std::cout << "������, ������������ ����� ��� To ��� ������� " << number << std::endl;
								return false;
							}
							if (index == (16 + step)) {
								std::cout << "������, ������������ ����� ��� Type � Edges ��� ������� " << number << std::endl;
								return false;
							}
						}
						temp += content[j];
						j++;
					}
					if (temp == "") {
						if (index == (8 + step)) {
							std::cout << "������, ������ �������� � ����� From ��� ������� " << number << std::endl;
							return false;
						}
						if (index == (12 + step)) {
							std::cout << "������, ������ �������� � ����� To ��� ������� " << number << std::endl;
							return false;
						}
						if (index == (16 + step)) {
							std::cout << "������, ������ �������� � ����� Type � Edges ��� ������� " << number << std::endl;
							return false;
						}
					}
					if (index == (8 + step)) from = std::stoi(temp);
					if (index == (12 + step)) to = std::stoi(temp);
					index++;
					while (content[j] == ' ' or content[j] == char(9)) j++;
					if (content[j] == ',' and index != (8 + step)) {
						j--;
					}
					else {
						if (index == (9 + step)) {
							std::cout << "������, ����������� ������� ����� " << number << " ����� From ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
						if (index == (13 + step)) {
							std::cout << "������, ����������� ������� ����� " << number << " ����� To ��� �������� ������ ������� ����� ���" << std::endl;
							return false;
						}
					}
				}
				else {
					if (index == (8 + step)) {
						std::cout << "������, ����������� ��������� ����� " << number << " ����� From ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
					if (index == (12 + step)) {
						std::cout << "������, ����������� ��������� ����� " << number << " ����� To ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
					if (index == (16 + step)) {
						std::cout << "������, ����������� ��������� ����� " << number << " ����� Type � Edges ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}
				
				if (content[j] == '"' and index == (10 + step))
				{
					j++;
					temp = "";
					while (isalpha(content[j]) or isdigit(content[j])) {
						temp += content[j];
						j++;
					}
					if (temp != "To" or content[j] == ' ') {
						std::cout << "������ � ������������ " << number << " ����� To";
						return false;
					}
					index++;
					if (content[j] == '"' and index != (10 + step)) {
						j--;
					}
					else {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� To" << std::endl;
						return false;
					}
				}
				else {
					if (index == (10 + step)) {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� To ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}

				if (content[j] == '"' and index == (14 + step))
				{
					j++;
					temp = "";
					while (isalpha(content[j]) or isdigit(content[j])) {
						temp += content[j];
						j++;
					}
					if (temp != "Type" or content[j] == ' ') {
						std::cout << "������ � ������������ " << number << " ����� Type � Edges";
						return false;
					}
					index++;
					if (content[j] == '"' and index != (14 + step)) {
						j--;
					}
					else {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� Type � Edges" << std::endl;
						return false;
					}
				}
				else {
					if (index == (14 + step)) {
						std::cout << "������, ����������� ����������� ������� ��� " << number << " ����� Type � Edges ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}

				if (content[j] == '}' and index == (17 + step)) { j--; }
				else {
					if (index == (17 + step)) {
						std::cout << "������, ����������� } ����� ����������� " << number << " ����� Type � Edges ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
				}

				if (content[j] == ',' and index == (18 + step) and to != counter_id) {
					step = step + 14;
					number++;
					j--;
				}
				else {
					if (index == (18 + step) and num_of_from != num_of_edges) {
						std::cout << "������, ����������� ������� ����� } ����������� " << number << " ����� Type � Edges ��� �������� ������ ������� ����� ���" << std::endl;
						return false;
					}
					if (index == (18 + step) and num_of_from == num_of_edges) {
						while (content[j] == char(9) or content[j] == char(32)) { j++; }
						if (content[j] != ']') {
							std::cout << "������, ������� �������� �������� ��� " << number << " ����� To ��� ����������� ] ��� ����� Edges" << std::endl;
							std::cout << "�������� ����������� �������� ������. ��������: " << content[j] << std::endl;
							return false;
						}
						else {
							j++;
							while (content[j] == char(9) or content[j] == char(32)) { j++; }
							if (content[j] != '}') {
								std::cout << "������, ����������� } � ����� ����-����� ��� �������� ������ ������� ����� ���" << std::endl;
								return false;
							}
							else {
								j++;
								while ((content[j] == char(9) or content[j] == char(32)) and j != content.size()) {
									j++;
								}
								if (content[j] != char(0) and content[j] != char(32)) {
									std::cout << "������, �������� ������ ������� ����� ����-�����" << std::endl;
									return false;
								}
								else {
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
};


class Data_Base
{
private:
	std::string answer;
	std::string name;

public:
	std::string utf8_to_cp866(const char* error_msg) {
		// PostgreSQL ���������� UTF-8, ������������ � CP1251 (������� Windows)
		int wchars_num = MultiByteToWideChar(CP_UTF8, 0, error_msg, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, error_msg, -1, wstr, wchars_num);

		int chars_num = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[chars_num];
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, chars_num, NULL, NULL);

		std::string result(str);
		delete[] wstr;
		delete[] str;

		return result;
	}

	void execute_composite_query(PGconn* conn, const std::string& name) {
		// �������� ���������� ����� ��� ����������� ���������� Id
		PGresult* countRes = PQexec(conn, "SELECT COUNT(*) FROM json_schema;");
		if (PQresultStatus(countRes) != PGRES_TUPLES_OK) {
			std::cerr << "�������� �� �������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
			PQclear(countRes);
			return;
		}

		int next_id = atoi(PQgetvalue(countRes, 0, 0)) + 1;
		PQclear(countRes);

		// �������������� ��������, ��� Id ������������� ��������
		std::string check_query = "SELECT 1 FROM json_schema WHERE id = " + std::to_string(next_id);
		PGresult* checkRes = PQexec(conn, check_query.c_str());

		if (PQntuples(checkRes) > 0) {
			// ���� Id �����, ���� ��������� ���������
			PGresult* gapRes = PQexec(conn,
				"SELECT min(id) + 1 FROM "
				"(SELECT id, LEAD(id) OVER (ORDER BY id) as next_id FROM json_schema) as gaps "
				"WHERE id + 1 <> next_id OR next_id IS NULL LIMIT 1");

			if (PQresultStatus(gapRes) == PGRES_TUPLES_OK && PQntuples(gapRes) > 0) {
				next_id = atoi(PQgetvalue(gapRes, 0, 0));
			}
			else {
				next_id++; // ������ �����������, ���� �� ����� �������
			}
			PQclear(gapRes);
		}

		PQclear(checkRes);

		// ��������� ������ � �����������
		const std::string insert_query =
			"INSERT INTO json_schema (id, name, \"Verification_Note\") VALUES (" +
			std::to_string(next_id) + ", " +
			PQescapeLiteral(conn, name.c_str(), name.length()) + ", 'No');";

		std::string file_path = "JSON/" + name + ".json";
		std::string filetxt_path = "JSON/" + name + ".txt";

		Converter converter;
		Checker checker;

		// �������� ������� ����� JSON
		std::ifstream file_json(file_path);
		if (!file_json.is_open()) {
			std::cerr << "���������� ���� � ����� ������� " << file_path << std::endl;
			return;
		}

		file_json.close();

		// ��������� ��������� JSON

		converter.To_txt(file_path);

		// �������� ����� �� ������
		std::ifstream file(filetxt_path);
		if (!file.is_open()) {
			std::cerr << "�� ������� ������� ���� " << filetxt_path << std::endl;
			return;
		}

		// ������ ����������� ����� � ������
		std::string line;
		std::string allContent;
		while (getline(file, line)) {
			allContent += line;
		}

		if (allContent == "") {
			std::cout << "������� ���� ����" << std::endl;
			converter.To_json(filetxt_path);
			return;
		}
		// �������� �����
		file.close();

		converter.To_json(filetxt_path);

		if (!checker.Check_structure(allContent))
		{
			std::cout << "������ � ��������� ����-�����" << std::endl;
			return;
		}

		// 3. ��������� ��������� ������
		std::string query = insert_query + R"(
    CREATE TEMPORARY TABLE temp_json_schema(
        id INTEGER,
        json_column json
    );
    
    COPY temp_json_schema (json_column) FROM ')" + file_path + R"(' DELIMITER E'\x01';
    
    UPDATE json_schema
    SET data = temp.json_column
    FROM temp_json_schema temp
    WHERE json_schema.id = )" + std::to_string(next_id) + R"(;
    
    DROP TABLE temp_json_schema;
)";

		PGresult* res = PQexec(conn, query.c_str());

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "�������� �� �������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;

			std::string error_msg = PQerrorMessage(conn);
			if (error_msg.find("unique_name") != std::string::npos)
			{
				bool flag = true;
				while (flag) {
					std::cout << "��������� �������� ��� ����-����� '" << name << "'\n"
						<< "�������� ������������ ������? (��/���): ";
					std::cin >> answer;
					if (answer == "��")
					{
						const char* param_values[] = { name.c_str() };  // ������ C-�����
						PGresult* delete_res = PQexecParams(conn,
							"DELETE FROM json_schema WHERE name = $1",
							1,              // 1 ��������
							NULL,           // ��������������� �����
							param_values,   // ��������� �� ������ ����������
							NULL,           // ����� (NULL ��� ����� � ����-������������)
							NULL,           // ������� (0 - �����)
							0               // ������ ���������� (0 - �����)
						);

						if (PQresultStatus(delete_res) != PGRES_COMMAND_OK) {
							std::cerr << "������ ��������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
							PQclear(delete_res);
							PQclear(res);
							return;
						}
						PQclear(delete_res);

						PGresult* retry_res = PQexec(conn, query.c_str());

						if (PQresultStatus(retry_res) == PGRES_COMMAND_OK) {
							std::cout << "������ ������� ���������." << std::endl;
						}
						else {
							std::cerr << "������ ��� ������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
						}

						PQclear(retry_res);
						flag = false;
					}
					else if (answer == "���") {
						flag = false;
					}

					else {
						std::cout << "�������� ����. ���������� ���������� ������." << std::endl;
					}
				}
			}

			PQclear(res);

			try {
				// ������� �������� ����� � ������ �������
				std::string single_line;
				single_line.reserve(allContent.size()); // ����������� ������

				bool prev_was_space = false;
				for (char c : allContent) {
					if (c == '\n' || c == '\r') {
						// �������� �������� ����� �� �������
						if (!prev_was_space) {
							single_line += ' ';
							prev_was_space = true;
						}
					}
					else if (std::isspace(c)) {
						// ��������� ������ ���� ������ ����� �������
						if (!prev_was_space) {
							single_line += ' ';
							prev_was_space = true;
						}
					}
					else {
						single_line += c;
						prev_was_space = false;
					}
				}

				// ���������� ��������� � �������� ����
				std::ofstream output_file(filetxt_path);
				if (!output_file.is_open()) {
					throw std::runtime_error("�� ������� ������� �������� ����: " + filetxt_path);
				}

				output_file << single_line;
				output_file.close();

				std::cout << "���� ������� ������������ � ���� ������: " << file_path << std::endl;

				converter.To_json(filetxt_path);
			}
			catch (const std::exception& e) {
				std::cerr << "������: " << e.what() << std::endl;
				throw; // ������������� ���������� ������
			}

			return;
		}

		PQclear(res);
		std::cout << "����-����� ������� ��������� ��� Id: " << next_id << std::endl;
	}

	void add_to_DB()
	{
		bool flag = true;
		while (flag) {
			std::cout << "������ ��������� ����-����� � ���� ������? (��/���)" << std::endl;
			std::cin >> answer;
			if (answer == "��")
			{	
				PGconn* conn = connect_to_DB();

				if (PQstatus(conn) != CONNECTION_OK) {
					std::cerr << "�� ������� ���������� ����������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
					PQfinish(conn);
					return;
				}

				// �������� ��� ��� ��������
				std::cout << "������� �������� ����-�����" << std::endl;
				std::cin >> name;
				execute_composite_query(conn, name);

				PQfinish(conn);
			}

			else if (answer == "���") {
				flag = false;
			}

			else {
				std::cout << "�������� ����. ����������, ���������� ������." << std::endl;
			}
		}

		return;
	}

	void update_verification_status(PGconn* conn, int record_id) {
		// ���������� ����������
		const char* paramValues[2];
		std::string yes_value = "Yes";
		std::string id_str = std::to_string(record_id);  // �������������� �� ������ ������

		paramValues[0] = yes_value.c_str();
		paramValues[1] = id_str.c_str();

		// ��������� ����������������� ������
		const char* query = "UPDATE json_schema SET \"Verification_Note\" = $1 WHERE id = $2::integer";

		// ��������� ������
		PGresult* res = PQexecParams(conn,
			query,
			2,          // ���������� ����������
			NULL,       // ���� ���������� (NULL - ���������������)
			paramValues,
			NULL,       // ����� ���������� (NULL - ������ ����������� \0)
			NULL,       // ������� ���������� (NULL - �����)
			0);         // ������ ���������� (0 - �����)

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "��������� ������� �������� ���������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
		}
		else {
			std::cout << "������� ����-����� � Id " << record_id << " ������� ��������" << std::endl;
		}

		PQclear(res);
	}

	void log_user(PGconn* conn, const std::string& username, const std::string& verification_date, int& user_id) {
		// ��������� ������ ������������
		const char* insert_params[2] = { verification_date.c_str(), username.c_str() };
		PGresult* insert_res = PQexecParams(conn,
			"INSERT INTO \"User\" (\"Date_of_verification\", name) VALUES ($1, $2) RETURNING id",
			2, NULL, insert_params, NULL, NULL, 0);

		if (PQresultStatus(insert_res) != PGRES_TUPLES_OK) {
			std::cerr << "������ ���������� ������������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
			PQclear(insert_res);
			return;
		}
		else {
			// �������� ������������� ��������������� Id
			user_id = atoi(PQgetvalue(insert_res, 0, 0));
			std::cout << "�������� ������������� " << username << " ������� ���������, Id: " << user_id << std::endl;
		}
		PQclear(insert_res);
	}

	void log_verification(PGconn* conn, int found_id, int user_id)
	{
		std::string insert_query =
			"INSERT INTO \"Verification\" (\"json_schema_ID\", \"User_ID\") VALUES (" + std::to_string(found_id)
			+ ", " + std::to_string(user_id) + "); ";

		PGresult* res = PQexec(conn, insert_query.c_str());

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			std::cerr << "�������� �� �������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
			PQclear(res);
			return;
		}

		PQclear(res);
	}

	nlohmann::ordered_json get_from_DB(const std::string& name_to_find, int& out_id) {
		// ������������� ���������� � PostgreSQL
		PGconn* conn = connect_to_DB();

		if (PQstatus(conn) != CONNECTION_OK) {
			std::cerr << "������ ����������� � ���� ������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
			PQfinish(conn);
			return nlohmann::ordered_json(); // ������ JSON � ������ ������
		}

		// ������ �������� � Id, � data
		const char* query = "SELECT id, data FROM json_schema WHERE name = $1;";
		const char* param_values[1] = { name_to_find.c_str() };
		int param_lengths[1] = { static_cast<int>(name_to_find.length()) };
		int param_formats[1] = { 0 }; // 0 = �����, 1 = ��������

		PGresult* res = PQexecParams(conn, query,
			1,       // ���������� ����������
			NULL,    // ���� ���������� (���������������)
			param_values,
			param_lengths,
			param_formats,
			0);      // ������ ���������� (0 = �����)

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			std::cerr << "������ ����������� � ���� ������: " << utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
			PQclear(res);
			PQfinish(conn);
			return nlohmann::ordered_json();
		}

		// ������������ ����������
		nlohmann::ordered_json result;
		int row_count = PQntuples(res);

		if (row_count == 0) {
			std::cout << "��� ����-���� � ��������� ������: " << name_to_find << std::endl;
			out_id = -1; // ���������, ��� ������ �� �������
		}
		else {
			// �������� Id �� ������ �������
			const char* id_str = PQgetvalue(res, 0, 0);
			out_id = std::stoi(id_str); // ��������� Id � ���������� ����������

			// �������� JSON ������ �� ������ �������
			const char* json_str = PQgetvalue(res, 0, 1);

			try {
				nlohmann::ordered_json parsed_json = nlohmann::ordered_json::parse(json_str);
				result["Vertices"] = parsed_json["Vertices"];  // ������� Vertices
				result["Edges"] = parsed_json["Edges"];         // ����� Edges
			}
			catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON parse error: " << e.what() << std::endl;
			}
		}

		// ����������� �������
		PQclear(res);
		PQfinish(conn);

		return result;
	}

	PGconn* connect_to_DB()
	{
		PGconn* conn = PQconnectdb("host=localhost dbname=postgres user=postgres password=12345678");

		return conn;
	}
};


class User_Interface
{
private:
	std::string name;
	std::string answer;
	std::string filename_json;
	std::string filename_txt;
	std::string filename_check;
public:
	void clear_Console() {
		// \033[2J - ������� ������, \033[H - ����������� ������� � ������
		std::cout << "\033[2J\033[H";
	}

	std::string get_current_date() {
		std::time_t t = std::time(nullptr);
		if (t == -1) {
			throw std::runtime_error("������ � ��������� ������� ����.");
		}

		std::tm tm = {};
#if defined(_WIN32) || defined(_WIN64)
		if (localtime_s(&tm, &t) != 0) {
			throw std::runtime_error("������ ����������� ����.");
		}
#else
		if (localtime_r(&t, &tm) == nullptr) {
			throw std::runtime_error("������ ����������� ����.");
		}
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d");
		return oss.str();
	}

	int Show(nlohmann::json objJson)
	{
		Data_Base DB;

		DB.add_to_DB();

		std::cout << "������� �������� ����-����� ��� ��������:" << std::endl;
		std::cin >> name;

		int found_id, user_id;

		auto result = DB.get_from_DB(name, found_id);

		if (!result.is_null()) {
			// ��������� � ����
			std::ofstream out("JSON/" + name + ".json");
			out << result.dump(4); // ���������� � �������� ����
			out.close();
			std::cout << "������ ����-����� ��������� �� ���� ������." << std::endl;
		}
		else {
			std::cout << "������ JSON �� ����������." << std::endl;
			return 1;
		}

		filename_json = "JSON/" + name + ".json";
		filename_txt = "JSON/" + name + ".txt";
		filename_check = filename_json;

		std::ifstream file_check(filename_check);
		if (!file_check.is_open()) {
			std::cerr << "�� ������� ������� ���� " << filename_check << std::endl;
			return 1;
		}
		file_check.close();

		Checker checker;

		Converter converter;

		converter.To_txt(filename_json);

		// �������� ����� �� ������
		std::ifstream file(filename_txt);
		if (!file.is_open()) {
			std::cerr << "�� ������� ������� ���� " << filename_txt << std::endl;
			return 1;
		}

		// ������ ����������� ����� � ������
		std::string line;
		std::string allContent;
		while (getline(file, line)) {
			allContent += line;
		}

		if (allContent == "") {
			std::cout << "������� ���� ����" << std::endl;
			converter.To_json(filename_txt);
			return 1;
		}
		// �������� �����
		file.close();

		converter.To_json(filename_txt);

		if (checker.Check_structure(allContent)) {
			std::fstream fileInput;
			fileInput.open(filename_json);

			fileInput >> objJson;
			bool flag = true;
			std::string choice;
			do {
				std::cout << "1. ��������� ����-����� ���������.\n";
				std::cout << "2. ��������� ������������ �����.\n";
				std::cout << "3. ��������� ����� ���������.\n";
				std::cout << "4. ��������� ����� �������.\n";
				std::cout << "5. ��������� ����� ������.\n";
				std::cout << "�������� ��������: ";
				std::cin >> choice;
			
				if (choice == "1") {
					clear_Console();
					int num_of_err = checker.Check_logic(objJson);
					if (num_of_err == 0)
					{
						PGconn* conn = DB.connect_to_DB();

						if (PQstatus(conn) != CONNECTION_OK) {
							std::cerr << "�� ������� ���������� ����������: " << DB.utf8_to_cp866(PQerrorMessage(conn)) << std::endl;
							PQfinish(conn);
							return 1;
						}

						DB.update_verification_status(conn, found_id);

						DB.log_user(conn, "postgres", get_current_date(), user_id);

						DB.log_verification(conn, found_id, user_id);

						PQfinish(conn);
					}
					flag = false;
				}
				else if (choice == "2") {
					clear_Console();
					checker.Check_terminal_blocks(objJson);
					flag = false;
				}
				else if (choice == "3") {
					clear_Console();
					checker.Check_process_blocks(objJson);
					flag = false;
				}
				else if (choice == "4") {
					clear_Console();
					checker.Check_solution_blocks(objJson);
					flag = false;
				}
				else if (choice == "5") {
					clear_Console();
					checker.Check_data_blocks(objJson);
					flag = false;
				}
				else {
					clear_Console();
					std::cout << "�������� ������ ����� ������. ����������, ���������� �����.\n";
				}
			} while (flag);

			fileInput.close();
		}
	}
};


int main()
{
	nlohmann::json objJson;

	setlocale(LC_ALL, "Russian");

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	User_Interface user_interface;

	user_interface.Show(objJson);
}
