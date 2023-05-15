// CMakeTask.cpp: определяет точку входа для приложения.
//

#include "CMakeTask.h"
#include "Context.h"
using namespace std;

int main()
{
    try {
        std::string ctx_str = "heeeeello";
        Context ctx1_a(ctx_str);
        Context my(4);
        my = ctx1_a;//std::cout << "result_int - " << ctx1_a.get_string() << std::endl; std::cout << "result_int my - " << ctx1_a.get_string() << std::endl;
        Context ctx1_b;
        Context ctx1_c;
        ctx_str = "hello";
        int ctx_int = 8;
        double ctx_double = 131.31;
        ctx1_a.set_scalar(ctx_str);
        if(ctx1_a.is_string())
            std::cout << "result_str - " << ctx1_a.get_string() << std::endl;
        if (ctx1_a.is_int())
            std::cout << "result_str - " << ctx1_a.get_int() << std::endl;
        else
            std::cout << "This scalar is not int! "  << std::endl;
        ctx1_b.set_scalar(ctx_int);
        std::cout << "result_int -  " << ctx1_b.get_int() << std::endl;
        ctx1_c.set_scalar(ctx_double);
        std::cout << "result_double -  " << ctx1_c.get_double() << std::endl;

        std::string str = "helloooo";
        Context ctx2;
        ctx2.add_element("name", Context(str));
        ctx2.add_element("age", Context(18));
        ctx2.add_element("is_student", Context(true));
        ctx2.add_element("grades", Context({ 85, 90, 95 }));
        std::cout << ctx2.get_element("name").get_string() << std::endl;
        std::cout << ctx2.get_element("grades").get_int() << std::endl;
        ctx2.get_scalar();
       //ctx2.del_element("grades");
       // std::cout << ctx2.get_element("grades").get_int() << std::endl;
        Context ctx3;
        ctx3.add_element(Context(1));
        ctx3.add_element(Context(2));
        ctx3.add_element(Context(3));
        for (auto i : ctx3.get_array())
        {
            std::cout << i.get_int() << std::endl;
        }     //   std::cout<<"***ctx2 age" << ctx2.get_element("age").get_int() << std::endl;
        ctx2 = ctx3;
        for (auto i : ctx2.get_array())
        {
            std::cout << i.get_int() << std::endl;
        }
    }
    catch (const char* error_message)
    {
        std::cout << error_message << std::endl;
    }
    return 0;
}
