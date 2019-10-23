#include "rpn.h"
#include <QtMath>
#define M_E		2.7182818284590452354
#define M_PI    3.14159265358979323846

rpn::rpn(QObject *parent) : QObject(parent)
{
}

rpn::~rpn()
{

}
//void rpn::messageError(QString err)
//{
//    QMessageBox::warning(this,"Ошибка", err);
//}

void rpn::setModel(QStandardItemModel *value)
{
    model = value;
}
bool equal(double a, double b) {
    return (a < b + 0.0000000000001) && (b < a + 0.0000000000001);
}




bool cel(long double x){
    if (x < 0)
        x = -x;
    x -= floor(x);
    return (equal(0, x) || equal(1, x));
}












QString rpn::logConvertloglog(QString input)
{
//    try{
    for (int i = 0; i < input.length(); i++) //Для каждого символа в входной строке
    {
        int startPos=0;
        int stopPos=0;
        if(QString(input[i])=="l" || input.count()>(i+2))
            if(QString(QString(input[i]) + QString(input[i+1]) + QString(input[i+2]))=="log")
            {
                startPos=i;
                i=i+4;
                int o=1;
                int z=0;
                QString input1 = "";
                while(o!=z)
                {
                    if(QString(input[i]) == "(" || QString(input[i]) == "[")
                        o++;
                    if(QString(input[i]) == ")" || QString(input[i]) == "]")
                        z++;

                    if(o!=z)
                        input1+=QString(input[i]);
                    i++;
                }
                stopPos=i;
                //Нашли log спарсили до конца в input1 работаем с input1

                int log1=1;
                int zap=0;
                QString a="";
                QString x = "";
                for (int j = 0; j < input1.length(); j++) //Для каждого символа в входной строке
                {
                    if(QString(input1[j])==",")
                        zap++;

                    if(QString(input1[j])=="l" && input1.length()>(j+2))
                        if(QString(QString(input1[j]) + QString(input1[j+1]) + QString(input1[j+2]))=="log")
                        {
                            log1++;
                        }

                    if(log1==zap)
                    {
                        a=input1.mid(0,j);
                        x=input1.mid(j+1,input1.length()-1);
                        break;
                    }
                }
                input1="(lg(" + a + ")" + "/lg(" + x + "))";

                input.remove(startPos,stopPos-startPos);
                input.insert(startPos,input1);
                return input;
            }
    }
//    }
//    catch(...){

//    }
}

//Метод, преобразующий входную строку с выражением в постфиксную запись
QString rpn::GetExpression(QString input)
{
//    try{
    QString output = ""; //Строка для хранения выражения
    QStack<QString> operStack; //Стек для хранения операторов


    while(input.contains("log"))//Раскладываем логарифмы с log(a,x) на log(x)/log(a)
        input=logConvertloglog(input);
    qDebug() << input;

    for (int i = 0; i < input.length(); i++) //Для каждого символа в входной строке
    {
        //Разделители пропускаем
        if(IsDelimeter(QString(input[i])))
            continue; //Переходим к следующему символу
        if(QString(input[i])=="~"){
            if (operStack.count() > 0) //Если в стеке есть элементы
                if (GetPriority(QString(input[i])) <= GetPriority(QString(operStack.top()))){ //И если приоритет нашего оператора меньше или равен приоритету оператора на вершине стека
                    while (operStack.count() > 0)
                    {
                        if(operStack.top()=="(" || operStack.top()=="[" ) //до ближайшей открывающей скобки
                            break;
                        if(GetPriority(QString(input[i])) > GetPriority(QString(operStack.top())))// или до операции с приоритетом меньшим, чем у новой операции
                            break;
                        output += QString(operStack.pop() + " "); //То добавляем последний оператор из стека в строку с выражением
                    }
                }
            operStack.push(QString(input[i])); //Если стек пуст, или же приоритет оператора выше - добавляем операторов на вершину стека
        }
        else
            //Если символ - цифра, то считываем все число
            if(input[i].isDigit()) //Если цифра
            {
                bool oper=false;
                for(;i<input.length();i++)
                {
                    if(IsDelimeter(QString(input[i])))
                        break;
                    if(IsOperator(QString(input[i])) && !oper)
                        break;
                    if(IsOperator(QString(input[i])))
                        oper=false;
                    if(QString(input[i])=="e")
                        oper=true;
                    output += input[i]; //Добавляем каждую цифру числа к нашей строке
                }
                //                //Читаем до разделителя или оператора, что бы получить число
                //                while (!IsDelimeter(QString(input[i])) && !IsOperator(QString(input[i])))
                //                {
                //                    output += input[i]; //Добавляем каждую цифру числа к нашей строке
                //                    i++; //Переходим к следующему символу

                //                    if (i == input.length()) break; //Если символ - последний, то выходим из цикла
                //                }

                output += " "; //Дописываем после числа пробел в строку с выражением
                i--; //Возвращаемся на один символ назад, к символу перед разделителем
            }
            else
            {
                if (IsOperator(QString(input[i])))   //Если символ - оператор
                {
                    if (QString(input[i]) == "(" || QString(input[i]) == "[") //Если символ - открывающая скобка
                        operStack.push(QString(input[i])); //Записываем её в стек
                    else if(QString(input[i]) == ")" || QString(input[i]) == "]") //Если символ - закрывающая скобка
                    {
                        //Выписываем все операторы до открывающей скобки в строку
                        QString s = QString(operStack.pop());

                        while (s != "(")
                        {
                            if(s=="[")
                                break;
                            output += QString(s + " ");
                            s = operStack.pop();
                        }
                    }
                    else //Если любой другой оператор
                    {
                        if (operStack.count() > 0) //Если в стеке есть элементы
                            if (GetPriority(QString(input[i])) <= GetPriority(QString(operStack.top()))){ //И если приоритет нашего оператора меньше или равен приоритету оператора на вершине стека
                                while (operStack.count() > 0)
                                {
                                    if(operStack.top()=="(" || operStack.top()=="[") //до ближайшей открывающей скобки
                                        break;
                                    if(GetPriority(QString(input[i])) > GetPriority(QString(operStack.top())))// или до операции с приоритетом меньшим, чем у новой операции
                                        break;
                                    output += QString(operStack.pop() + " "); //То добавляем последний оператор из стека в строку с выражением
                                }
                            }
                        operStack.push(input[i].toLower()); //Если стек пуст, или же приоритет оператора выше - добавляем операторов на вершину стека
                    }
                }
                else
                    if(input.count()>(i+2)) // && QString(input[i])=="s" || QString(input[i]) == "c" || QString(input[i]) == "t" || QString(input[i])=="l"|| QString(input[i])=="e"
                    {
                        QString operat = QString(QString(input[i]) + QString(input[i+1])+ QString(input[i+2]));
                        if(operat=="sin" || operat=="cos" || operat=="ctg" || operat=="exp"){
                            i=i+2;

                            if (operStack.count() > 0) //Если в стеке есть элементы
                                if (GetPriority(operat) <= GetPriority(QString(operStack.top()))){ //И если приоритет нашего оператора меньше или равен приоритету оператора на вершине стека
                                    while (operStack.count() > 0)
                                    {
                                        if(operStack.top()=="(" || operStack.top()=="[") //до ближайшей открывающей скобки
                                            break;
                                        if(GetPriority(operat) > GetPriority(QString(operStack.top())))// или до операции с приоритетом меньшим, чем у новой операции
                                            break;
                                        output += QString(operStack.pop() + " "); //То добавляем последний оператор из стека в строку с выражением
                                    }
                                }
                            operStack.push(operat); //Если стек пуст, или же приоритет оператора выше - добавляем операторов на вершину стека
                        }else
                            if(operat=="tg(" || operat=="lg(")
                            {
                                i=i+1;
                                operat.remove(2,1);

                                if (operStack.count() > 0) //Если в стеке есть элементы
                                    if (GetPriority(operat) <= GetPriority(QString(operStack.top()))){ //И если приоритет нашего оператора меньше или равен приоритету оператора на вершине стека
                                        while (operStack.count() > 0)
                                        {
                                            if(operStack.top()=="(" || operStack.top()=="[") //до ближайшей открывающей скобки
                                                break;
                                            if(GetPriority(operat) > GetPriority(QString(operStack.top())))// или до операции с приоритетом меньшим, чем у новой операции
                                                break;
                                            output += QString(operStack.pop() + " "); //То добавляем последний оператор из стека в строку с выражением
                                        }
                                    }
                                operStack.push(operat); //Если стек пуст, или же приоритет оператора выше - добавляем операторов на вершину стека
                            }
                            else {
                                if(QString(input[i])+ QString(input[i+1]) !="pi")
                                {
                                    QString str1="";
                                    bool find = false;
                                    while(i < input.length()){
                                        str1 += QString(input[i]);
                                        if(IsOperator(QString(input[i])) && IsDelimeter(QString(input[i]))){
                                            emit signError(QString("Неизвестный параметр "/* + str1*/));
                                            throw std::invalid_argument("Неизвестная переменная! \n ъеъ");
                                            return output;
                                        }

                                        if(!model->findItems(str1).isEmpty()){
                                            output += str1;
                                            output += " ";
                                            find=true;
                                            break;
                                        }

                                        i++;
                                    }
                                    if(!find)
                                        emit signError(QString("Неизвестный параметр "/* + str1*/));
                                        throw std::invalid_argument("Неизвестная переменная! \n ъеъ");
                                        return output+str1;
                                } else {
                                    output+=QString(QString("pi")+ QString(" "));
                                    i=i+1;
                                }
                            }
                    }
                    else {
                        if(QString(QString(input[i])+ QString(input[i+1])) !="pi")
                        {
                            QString str1="";
                            bool find = false;
                            while(i < input.length()){
                                str1 += QString(input[i]);
                                if(IsOperator(QString(input[i])) && IsDelimeter(QString(input[i]))){
                                    emit signError(QString("Неизвестный параметр "/* + str1*/));
                                    throw std::invalid_argument("Неизвестная переменная! \n ъеъ");
                                    return output+str1;
                                }

                                if(!model->findItems(str1).isEmpty()){
                                    output += str1;
                                    output += " ";
                                    find=true;
                                    break;
                                }

                                i++;
                            }
                            if(!find)
                                emit signError(QString("Неизвестный параметр "/* + str1*/));
                                throw std::invalid_argument("Неизвестная переменная! \n ъеъ");
                                return output+str1;
                        } else {
                            output+=QString(QString("pi")+ QString(" "));
                            i=i+1;
                        }
                    }
            }
    }

    //Когда прошли по всем символам, выкидываем из стека все оставшиеся там операторы в строку
    while (operStack.count() > 0)
        output += QString(operStack.pop() + " ");
    return output; //Возвращаем выражение в постфиксной записи
//}
//    catch(...)
//    {

//    }
}

//Метод, вычисляющий значение выражения, уже преобразованного в постфиксную запись
double rpn::Counting(QString input)
{
    try
    {
    double result = 0; //Результат
    QStack<double> temp; //Cтек для решения

    QStringList List = input.split(' ',QString::SkipEmptyParts);
    qDebug() << List;

    int i = 0;
    foreach (const QString &str, List) {

        bool ok=false;
        str.toDouble(&ok);
        if(ok)//Если символ - цифра, то читаем все число и записываем на вершину стека
            temp.push(str.toDouble()); //Записываем в стек
        else
            if (IsOperator(str)) //Если символ - оператор
            {
                if(temp.isEmpty())
                {
                    emit signError("Ошибка");
                    return 0;
                }

                //Берем два последних значения из стека
                double a = temp.pop();


                //И производим над ними действие, согласно оператору
                if(str=="+"){
                    if(temp.isEmpty())
                    {
                        emit signError("Ошибка");
                        return 0;
                    }
                    double b = temp.pop();
                    result = b + a;
                }
                if(str=="-"){
                    if(temp.isEmpty())
                    {
                        emit signError("Ошибка");
                        return 0;
                    }
                    double b = temp.pop();
                    result = b - a;
                }
                if(str=="~"){
                    result = -a;
                }
                if(str=="*"){
                    if(temp.isEmpty())
                    {
                        emit signError("Ошибка");
                        return 0;
                    }
                    double b = temp.pop();
                    result = b * a;
                }
                if(str=="/"){
                    if(temp.isEmpty())
                    {
                        emit signError("Ошибка");
                        return 0;
                    }
                    double b = temp.pop();
                    if(a==0)
                    {
                        emit signError("Деление на ноль");
                        return 0;
                    }
                    result = b / a;
                }
                if(str=="^"){
                    if(temp.isEmpty())
                    {
                        emit signError("Ошибка");
                        return 0;
                    }
                    double b = temp.pop();
                    result = powMy(b, a);
                }

                temp.push(result); //Результат вычисления записываем обратно в стек
            }
            else if(str=="sin" || str == "cos" || str == "tg" ||str=="lg"|| str=="exp")
            {
                if(temp.isEmpty())
                {
                    emit signError("Ошибка");
                    return 0;
                }
                double a = temp.pop();
                if(str=="sin"){
                    result = this->sinn(a);
                }
                if(str=="cos"){
                    result = this->coss(a);
                }
                if(str=="tg"){
                    result = this->tgg(a);
                }
                if(str=="ctg"){
                    result = this->ctgg(a);
                }
                if(str=="exp"){
                    result = this->exp1(a);
                    //result = exp(a);
                }
                if(str=="lg")
                {
                    if(a<0)
                    {
                        emit signError("Ошибка: отрицательное число в логарифме");
                        return 0;
                    }
                    result = this->my_log(a);
                    // result = log(a);
                }

                temp.push(result); //Результат вычисления записываем обратно в стек
            }
            else {
                if(str=="pi")
                    temp.push(M_PI); //Записываем в стек
                else {
                    if(model->findItems(str).isEmpty())
                    {
                        emit signError(QString("Ошибка: Неизвестное значение " + str));
                        return 0;
                    }
                    int row = model->findItems(str).at(0)->row();
                    temp.push(model->data(model->index(row,1)).toDouble()); //Записываем в стек;
                }
            }
        i++;
    }
    return temp.top(); //Забираем результат всех вычислений из стека и возвращаем его
    }
    catch(...){

    }
}



bool rpn::IsOperator(QString c)
{
    if (QString("+-/*^()[]~").indexOf(c) != -1)
        return true;
    return false;
}

//Метод возвращает true, если проверяемый символ - разделитель ("пробел" или "равно")
bool rpn::IsDelimeter(QString c)
{
    if (QString(" =").indexOf(c) != -1)
        return true;
    return false;
}

int rpn::GetPriority(QString s)
{
    if(s=="(")
        return 0;
    if(s==")")
        return 0;
    if(s=="[")
        return 0;
    if(s=="]")
        return 0;
    if(s=="+")
        return 1;
    if(s=="-")
        return 1;
    if(s=="*")
        return 2;
    if(s=="/")
        return 2;
    if(s=="^")
        return 3;

    if(s=="sin")
        return 4;
    if(s=="cos")
        return 4;
    if(s=="tg")
        return 4;
    if(s=="ctg")
        return 4;
    if(s=="lg")
        return 4;
    if(s=="~")
        return 5;

    // return 4;
    return 6;
}

double rpn::sinn(double x)
{

    try
    {
    if(!radianGrad)
        x = x * 0.0175;

    if (x==M_PI/2)
        return 1;
    if (x==-M_PI/2)
        return -1;
    if(x==M_PI)
        return 0;
    // qDebug() << x;

    double factorial = 1.;      //факториал в знаменателе, вещественное, т.к. число очень большое
    double e=0.00000000000001;        //погрешность вычисления
    double res = x;             //значение суммы ряда (синуса)
    double arg = x;             //числитель
    double next = x;            //очередное слагаемое, первое равно х
    int i = 1;                  //для подсчета факториала
    int sign = 1;               //знак слагаемого

    while(fabs_my(next) > e)       //считаем, пока очередное слагаемое не станет меньше погрешности
    {
        arg *= x*x;             //числитель слагаемого
        factorial *= (i+1) * (i+2); //знаменатель слагаемого
        sign *= -1;             //знак слагаемого
        i += 2;                 //для следующего слагаемого
        next = sign * arg / factorial; //очередное слагаемое
        res += next;            //накапливаем сумму, значение синуса
    }

    return res;                 //возвращаем значение синуса
    }
    catch(...){

    }
}

double rpn::coss(double x)
{
    try{
    if(!radianGrad)
        x = x * 0.0175;
    if(x==M_PI/2)
        return 0;
    if(x==-M_PI/2)
        return 0;
    if(x==M_PI)
        return -1;

    double n = 1.0;
    double sum = 0.0;
    int i = 1;

    do
    {
        sum += n;
        n *= -1.0 * x * x / ((2 * i - 1) * (2 * i));
        i++;
    }
    while (fabs_my(n) > 0.00000000000001);

    return sum;
    }
    catch(...)
    {

}
}

double rpn::tgg(double x)
{
    if (x==M_PI/2 || x==-M_PI/2){ signError("Делим на 0, да?"); return 0;}
    return sinn(x)/coss(x);
}

double rpn::ctgg(double x)
{
    if (x==M_PI || x==-M_PI){ signError("Делим на 0, да?"); return 0;}
    return coss(x)/sinn(x);
}

//Функция, которая считает exp(x) с точностью eps разложением в ряд Тейлора:
double rpn::exp1(double x)
{
    bool minus=false;
    if(x<0){
        minus = true;
        x=-x;
    }
    double eps=0.0000000001;
    double a = 1, s = 1;
    for(int n=1; a>eps; n++)
    {
        a *= x/n;
        s += a;
    }

    if(minus)
        s=1/s;
    return s;
}

//Модуль
double rpn::fabs_my(double x)
{
    if(x<0)
        return -x;
    else return x;
}










//Расчет логарифма
long double rpn::my_log(long double a)
{

    double k = 2;
    while(a > 100){
        k *= 2;
       a = sqrt(a);
    }
    double x = (a - 1) / (a + 1);
    double ans = x, sans = 0.0, pow = x * x * x, nn = 3;
    int i = 0;
    for (; i < 1000 && abs(ans - sans) >M_E;  sans = ans, ans += pow / nn, pow *= x * x, nn += 2);
    return k*ans;
}

//long double my_log_2(long double a)
//{

//    double k = 2;
//    while(a > 100){
//        k *= 2;
//       a = sqrt(a);
//    }
//    double x = (a - 1) / (a + 1);
//    double ans = x, sans = 0.0, pow = x * x * x, nn = 3;
//    int i = 0;
//    for (; i < 1000 && abs(ans - sans) > M_E; sans = ans, ans += pow / nn, pow *= x * x, nn += 2);
//    return k*ans;
//}


template<typename T>
T binpow(T a, int st) {
    if (st == 0)
        return 1;
    if (st < 0)
        return (long double)(1) / binpow(a, -st);
    if (st & 1)
        return (binpow(a, st - 1) * a);
    T res = binpow(a, st >> 1);
    return (res * res);
}
long double rpn::powMy(long double x, long double st){
    if (cel(st))
        return binpow(x, (int)qCeil(st));
    return exp(st*(my_log(x)));
    return 1;
}
