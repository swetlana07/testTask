// Context 

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
#include <iterator>
#include <list>
//������� ����� ��� ���������� �������
class Scalar {
public:
    Scalar() : my_type("-") {}
    virtual ~Scalar() {}
    std::string my_type;//��� ���������� �������
};
//����������� ��������� ����� ���������� �������
// ��������� ������
template <typename T>
class Scalar_ :public Scalar
{
public:
    std::vector<T> data;
    Scalar_(){ my_type = typeid(T).name(); }
    Scalar_(const T& data_){ data.push_back(data_);  my_type = typeid(T).name();}
    ~Scalar_(){ data.clear();}
};

// ��������� ������������� ��� initializer_list
template <typename T>
class Scalar_<std::initializer_list<T> > :public Scalar
{
public:
    std::vector<T> data;//��������� ������ ����� ���� ��������, ������� ��������� � ����������
    Scalar_(std::initializer_list<T> data_) {
        for (auto t : data_)
            data.push_back(t);
        my_type = typeid(T).name();
    }
    ~Scalar_() { data.clear(); }
};
// ��������� ������������� ��� ��������
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

//����� Context, ���������� ��������� ������, ������ ��������� �������� Context � ����� ����-�������� ������� � ������
class Context {
    using MAP = std::map<std::string, std::list<Context>::iterator >;
    std::list<Context> mas;//������ ��� �������� �������� Context
    MAP map;//����� ��� ������ �������� � ������ �� �����
    Scalar* scl;//��������� �� ��������� ������(�������)
public:
    Context() :mas(NULL), scl(NULL) { }//����������� �� ���������
    template <typename T>
    Context(const T& data_) { //��������� ����������� � ����� ���������� - ��������� �������
        scl = new Scalar_<T>(data_);//std::cout <<typeid(T).name()<< " Context - " << scl->my_type << std::endl;
    }
    template <typename T>
    Context(std::initializer_list<T>  data_) {//��������� ����������� � ����� ���������� - ��������� �������-�������
        scl = new Scalar_<std::initializer_list<T>>(data_);
    }
    Context(const Context& cnt) {// ����������� �����������
        this->map = cnt.map;
        this->mas = cnt.mas;
        if (cnt.scl)
            this->scl = cpy(cnt.scl);
    }
    Context& operator=(const Context& cnt) {//�������� ������������
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
    Scalar* cpy(Scalar* src) {//��������������� ������� ����������� ��� ���������� �������, �� ������� rtti

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
    void add_element(std::string key, Context value) {//������� ��� ���������� � ������ ������� Context � ���� ��� ���� ����-�������� � �����
        mas.push_back(value);
        map[key] = --mas.end();
        std::list<Context>::iterator it = map[key];        //std::cout << "addelement - " << (*it).scl->my_type<< std::endl;
    }
    void add_element(Context value) {//���������� �������- ������ ���������� � ������ ������� Context
        mas.push_back(value);
    }
    void del_element(std::string key) {//������� ��� �������� �� ������ � �� �����(���� ����) ������� Context
        std::list<Context>::iterator it = map[key];
        mas.erase(it);
    }
    Context& get_element(std::string key) {//������� ���������� ������ �� ������ �� ������ �� �����
        std::list<Context>::iterator it = map[key];
        if (map.find(key) == map.end()) {
            throw "Key not found!";// not found
        }
        else {//std::cout << "getelement - " << (*it).scl->my_type<< std::endl;
            return  *it;// found
        }

    }
    std::list<Context> get_array() { return mas; }//������� ���������� ������ ��������
    template <typename T>
    void set_scalar(const T& data_) {//��������� ������� ��������� ���������� �������
        if (scl) delete scl;
        scl = new Scalar_<T>(data_);

    }
    int get_int() {//���������� ��������� ������ ���� int
        if (!scl->my_type.compare(0, 3, "int")) {
            Scalar_<int>* p = static_cast<Scalar_<int>*>(scl);       //if (p = dynamic_cast<>(scl))
            return p->data.at(0);
        }
        else throw "This scalar is not int!";
    }
    double get_double() {  //���������� ��������� ������ ���� double     /* Scalar_<double>* p; if (p = dynamic_cast<Scalar_<double>*>(scl));return p->data.at(0);*/   //  std::cout << "type double:::" << scl->my_type << " "  << std::endl;
        if (!scl->my_type.compare(0, 6, "double")) {
            Scalar_<double>* p = static_cast<Scalar_<double>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not double!";
    }
    std::string  get_string() {//���������� ��������� ������ ���� string// std::cout << "type:" << scl->my_type << " "<<scl->my_type.substr(17,5)<<std::endl;
        size_t pos = scl->my_type.find("string");
        if (pos != std::string::npos){
            Scalar_<std::string>* p = static_cast<Scalar_<std::string>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not string!";;
    }
    bool get_bool() {//���������� ��������� ������ ���� bool
        if (!scl->my_type.compare(0, 4, "bool")) {
            Scalar_<bool>* p = static_cast<Scalar_<bool>*>(scl);
            return p->data.at(0);
        }
        else throw "This scalar is not bool!";
    }
    void get_scalar(std::string left="") {//��������� � ���� �����  �������� ���� ��������� ��������, � ��� ����� �������� Context �� ������
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
    bool is_int() {//�������� ���� ���������� ������� �� int
        if (scl && (!scl->my_type.compare(0, 3, "int")))
            return true;
        else return false;
    }
    bool is_bool() {//�������� ���� ���������� ������� �� bool
        if (scl && (!scl->my_type.compare(0, 4, "bool")))
            return true;
        else return false;
    }
    bool is_string() {//�������� ���� ���������� ������� �� string
        size_t pos = scl->my_type.find("string");
        if (pos != std::string::npos) 
            return true;
        else return false;
    }
    bool is_double() {//�������� ���� ���������� ������� �� double
        if (scl && (!scl->my_type.compare(0, 6, "double")))
            return true;
        else return false;
    }
};