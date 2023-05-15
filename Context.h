// Context 

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
#include <iterator>
#include <list>
//Базовый класс для скалярного объекта
class Scalar {
public:
    Scalar() : my_type("-") {}
    virtual ~Scalar() {}
    std::string my_type;//тип скалярного объекта
};
//Производный шаблонный класс скалярного объекта
// первичный шаблон
template <typename T>
class Scalar_ :public Scalar
{
public:
    std::vector<T> data;
    Scalar_(){ my_type = typeid(T).name(); }
    Scalar_(const T& data_){ data.push_back(data_);  my_type = typeid(T).name();}
    ~Scalar_(){ data.clear();}
};

// частичная специализация для initializer_list
template <typename T>
class Scalar_<std::initializer_list<T> > :public Scalar
{
public:
    std::vector<T> data;//скалярный объект может быть массивом, поэтому находится в контейнере
    Scalar_(std::initializer_list<T> data_) {
        for (auto t : data_)
            data.push_back(t);
        my_type = typeid(T).name();
    }
    ~Scalar_() { data.clear(); }
};
// частичная специализация для массивов
template <typename T>
class Scalar_<const T[]> :public Scalar
{
public:
    std::vector<T> data;
    Scalar_(const T data_[]) {
        for (auto t : data_)
            data.push_back(t);
        my_type = typeid(T).name();
    }
    ~Scalar_() { data.clear(); }
};

//Класс Context, содержащий скалярный объект, список вложенных объектов Context и карту ключ-итератор объекта в списке
class Context {
    using MAP = std::map<std::string, std::list<Context>::iterator >;
    std::list<Context> mas;//список для хранения объектов Context
    MAP map;//карта для поиска объектов в списке по ключу
    Scalar* scl;//указатель на скалярный объект(базовый)
public:
    Context() :mas(NULL), scl(NULL) { }//конструктор по умолчанию
    template <typename T>
    Context(const T& data_) { //шаблонный конструктор с одним параметром - значением скаляра
        scl = new Scalar_<T>(data_);//std::cout <<typeid(T).name()<< " Context - " << scl->my_type << std::endl;
    }
    template <typename T>
    Context(std::initializer_list<T>  data_) {//шаблонный конструктор с одним параметром - значением скаляра-массива
        scl = new Scalar_<std::initializer_list<T>>(data_);
    }
    Context(const Context& cnt) {// конструктор копирования
        this->map = cnt.map;
        this->mas = cnt.mas;
        if (cnt.scl)
            this->scl = cpy(cnt.scl);
    }
    Context& operator=(const Context& cnt) {//оператор присваивания
        if (this != &cnt) {
            this->mas.clear(); this->mas = cnt.mas;
            this->map.clear(); this->map = cnt.map;
            delete this->scl;
            if (cnt.scl)
                this->scl = cpy(cnt.scl);
            else
                this->scl = NULL;
        }
        return *this;
    }
    Scalar* cpy(Scalar* src) {//вспомогательная функция копирования для скалярного объекта, не требует rtti

        if (!src->my_type.compare("-")) {
            return new Scalar();
        }
        if (!src->my_type.compare(0, 3, "int")) {
            Scalar_<int>* src_ = static_cast<Scalar_<int>*> (src);
            Scalar_<int>* res = new Scalar_<int>;
            res->data = src_->data;
            return static_cast<Scalar*>(res);
        }
        if (!src->my_type.compare(0, 4, "bool")) {
            Scalar_<bool>* src_ = static_cast<Scalar_<bool>*> (src);
            Scalar_<bool>* res = new Scalar_<bool>;
            res->data = src_->data;

            return static_cast<Scalar*>(res);
        }
        if (!src->my_type.compare(0, 6, "double")) {
            Scalar_<double>* src_ = static_cast<Scalar_<double>*> (src);
            Scalar_<double>* res = new Scalar_<double>;
            res->data = src_->data;
            return static_cast<Scalar*>(res);
        }
       // std::cout << "cpy - " << src->my_type << std::endl;
        size_t pos = src->my_type.find("string");
        if (pos != -1) {
            Scalar_<std::string>* src_ = static_cast<Scalar_<std::string>*> (src);
            Scalar_<std::string>* res = new Scalar_<std::string>;
            res->data = src_->data;
            return static_cast<Scalar*>(res);
        }
        return NULL;
    }
    void add_element(std::string key, Context value) {//Функция для добавления в список объекта Context и пары для него ключ-значение в карту
        mas.push_back(value);
        map[key] = --mas.end();
        std::list<Context>::iterator it = map[key];        //std::cout << "addelement - " << (*it).scl->my_type<< std::endl;
    }
    void add_element(Context value) {//Перегрузка функции- только добавление в список объекта Context
        mas.push_back(value);
    }
    void del_element(std::string key) {//Функция для удаления из списка и из карты(если есть) объекта Context
        std::list<Context>::iterator it = map[key];
        mas.erase(it);
    }
    Context& get_element(std::string key) {//функция возвращает ссылку на объект из списка по ключу
        std::list<Context>::iterator it = map[key];
        if (map.find(key) == map.end()) {
            throw "Key not found!";// not found
        }
        else {//std::cout << "getelement - " << (*it).scl->my_type<< std::endl;
            return  *it;// found
        }

    }
    std::list<Context> get_array() { return mas; }//Функция возвращает список объектов
    template <typename T>
    void set_scalar(const T& data_) {//шаблонная функция установки скалярного объекта
        if (scl) delete scl;
        scl = new Scalar_<T>(data_);

    }
    int get_int() {//возвращает скалярный объект типа int
        if (!scl->my_type.compare(0, 3, "int")) {
            Scalar_<int>* p = static_cast<Scalar_<int>*>(scl);       //if (p = dynamic_cast<>(scl))
            return p->data.at(0);
        }
        else throw "This scalar is not int!";
    }
    double get_double() {  //возвращает скалярный объект типа double     /* Scalar_<double>* p; if (p = dynamic_cast<Scalar_<double>*>(scl));return p->data.at(0);*/   //  std::cout << "type double:::" << scl->my_type << " "  << std::endl;
        if (!scl->my_type.compare(0, 6, "double")) {
            Scalar_<double>* p = static_cast<Scalar_<double>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not double!";
    }
    std::string  get_string() {//возвращает скалярный объект типа string// std::cout << "type:" << scl->my_type << " "<<scl->my_type.substr(17,5)<<std::endl;
        size_t pos = scl->my_type.find("string");
        if (pos != std::string::npos){
            Scalar_<std::string>* p = static_cast<Scalar_<std::string>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not string!";;
    }
    bool get_bool() {//возвращает скалярный объект типа bool
        if (!scl->my_type.compare(0, 4, "bool")) {
            Scalar_<bool>* p = static_cast<Scalar_<bool>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not bool!";
    }
    void get_scalar(std::string left="") {//вовращает в виде строк  значения всех скалярных объектов, в том числе объектов Context из списка
        std::string res="";
        if (scl) {
            if (!scl->my_type.compare(0, 3, "int")) {
                Scalar_<int>* p = static_cast<Scalar_<int>*>(scl);
                for (int t : p->data)
                    res += std::to_string(t) + " ";

            }
            else {
                if (!scl->my_type.compare(0, 6, "double")) {
                    Scalar_<double>* p = static_cast<Scalar_<double>*>(scl);
                    for (double t : p->data)
                        res += std::to_string(t) + " ";

                }
                else {
                    if (!scl->my_type.compare(0, 4, "bool")) {
                        Scalar_<bool>* p = static_cast<Scalar_<bool>*>(scl);
                        for (bool t : p->data)
                            if (t)
                                res += "true ";
                            else
                                res += "false ";
                    }
                    else {
                        size_t pos = scl->my_type.find("string");
                        if (pos != std::string::npos) {
                            Scalar_<std::string>* p = static_cast<Scalar_<std::string>*>(scl);
                            for (std::string t : p->data)
                                res += t + " ";
                        }

                    }
                }
            }
        }
        std::cout << left+res << std::endl;
        
        for (Context cnt : mas)
            cnt.get_scalar(left+"  ");
        
    }
    bool is_int() {//проверка типа скалярного объекта на int
        if (scl && (!scl->my_type.compare(0, 3, "int")))
            return true;
        else return false;
    }
    bool is_bool() {//проверка типа скалярного объекта на bool
        if (scl && (!scl->my_type.compare(0, 4, "bool")))
            return true;
        else return false;
    }
    bool is_string() {//проверка типа скалярного объекта на string
        size_t pos = scl->my_type.find("string");
        if (pos != std::string::npos) 
            return true;
        else return false;
    }
    bool is_double() {//проверка типа скалярного объекта на double
        if (scl && (!scl->my_type.compare(0, 6, "double")))
            return true;
        else return false;
    }
};