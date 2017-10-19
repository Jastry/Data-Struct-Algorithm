#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class Date
{
public:
    Date()      /*构造函数*/
    {
        struct tm* ptime;
		time_t tid;
		tid = time( NULL );
		ptime = localtime( &tid );
		char ch[10] = {0};
		char cm[10] = {0};
		char cs[10] = {0};
		char year[10] = {0};
		char mth[10] = {0};
		char day[10] = {0};

		if (ptime->tm_hour < 10)
			sprintf(ch, "0%d", ptime->tm_hour);
		else
			sprintf(ch, "%d", ptime->tm_hour);
		
		if (ptime->tm_min < 10)
			sprintf(cm, "0%d", ptime->tm_min);
		else
			sprintf(cm, "%d", ptime->tm_min);
	
		if (ptime->tm_sec < 10)
			sprintf(cs, "0%d", ptime->tm_sec);
		else
			sprintf(cs, "%d", ptime->tm_sec);
		
		sprintf(year, "%d年", ptime->tm_year + 1900);
		sprintf(mth, "%d月", ptime->tm_mon + 1);
		sprintf(day, "%d日", ptime->tm_mday);

		_year = ptime->tm_year + 1900;
		_month = ptime->tm_mon + 1;
		_day = ptime->tm_mday;
		if (!JudgeDay()){
			cout<<"day is error"<<endl;
		}
    }

    Date(const Date& d)/*拷贝构造函数*/
    {
        _year = d._year;
        _month = d._month;
        _day = d._day;
        cout<<"Date(const Date& d)"<<endl;
    }

    Date& operator=(const Date& d)/*赋运('=')算符重载*/
    {
        if(this != &d)
        {
            _year = d._year ;
            _month = d._month;
            _day = d._day;
        }
        cout<<"operator=()"<<endl;
        return *this;
    }

    bool JudgeDay()/*判断这一天是否存在*/
    {
        if ( _year >= 1900
            && _month > 0 && _month < 12
            && _day > 0 && _day <= GetDay(_year, _month) )
        {
            return true;
        }
        return false;
    }

    int GetDay(int year, int month)/*判断这个月有几天*/
    {
        int numberOfDays;  
        if (month == 4 || month == 6 || month == 9 || month == 11)  
          numberOfDays = 30;  
        else if (month == 2)  
        { bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);  
          if (isLeapYear)  
            numberOfDays = 29;  
          else  
            numberOfDays = 28;  
        }  
        else  
          numberOfDays = 31;                                                       //不是二月
        return numberOfDays ;
    }

    Date operator+(int day)/*'+'号运算符重载*/
    {
        if(day < 0)
        {
            return *this - (-day);
        }
        Date tmp(*this);
        tmp._day += day;
        while (tmp.JudgeDay() == false)
        {
            tmp._day -= tmp.GetDay(tmp._year, tmp._month);
            if (12 == tmp._month)                                                //加一年，变成一月
            {
                tmp._year++;
                tmp._month = 1;
            }
            else                                                                                  //加一个月
            {
                tmp._month++;
            }
        }
        return tmp;
    }

    Date operator-(int day)/*'-'号算符重载*/
    {
        if (day < 0)
        {
            return *this + (-day);
        }
        Date tmp(*this);
        tmp._day -= day;
        while (tmp.JudgeDay() == false)
        {
            if (tmp._month == 1)
            {
                tmp._year--;
                tmp._month = 12;
            }
            else
            {
                tmp._month--;
            }
            tmp._day += tmp.GetDay(tmp._year, tmp._month);
        }
        return tmp;
    }
    inline Date& operator -=(int day) /*'-='算符重载*/
    {
        if (day < 0)
        {
            return *this +=(-day);
        }
        this->_day -= day;
        while (this->JudgeDay() == false)
        {
            if (1 == this->_month)
            {
                this->_year--;
                this->_month = 12;
            }
            else
            {
                this->_month--;
            }
            this->_day += this->GetDay(this->_year, this->_month);
        }
        return *this;
    }
    inline Date& operator +=(int day)/*+=运算符重载*/
     {
         if (day < 0)
         {
             return *this -= (-day);
         }
         this->_day += day;
         while (this->JudgeDay() == false)
         {
             this->_day -= this->GetDay(this->_year, this->_month);
             if (12 == this->_month)
             {
                 this->_year++;
                 this->_month = 1;
             }
             else
             {
                 this->_month++;
             }
         }
         return *this;
     }
    inline Date& operator++()/*前置++*/
    {
        return *this+=1;
    }
    inline Date operator++(int)/*后置++*/
    {
        //
    }
    inline Date& operator--()/*前置--*/
    {
        return *this -= 1;
    }
    int operator-(const Date& date) /*'-'运算符重载*/
    {
        if (this->_year >= date._year)
        {
            int addYear = this->_year - date._year;
            if (this->_month >= date._month)
            {
                Date newDate(date);
                int sum = 0;
                while (newDate._month != this->_month)
                {
                    sum += newDate.GetDay(newDate._year, newDate._month);
                    newDate._month++;
                }
                return ( sum += addYear * 365 + this->_day - date._day );
            }
            else if (this->_month < date._month)
            {
                Date newDate(*this);
                int sum = 0;
                while (newDate._month != date._month)
                {
                    sum += newDate.GetDay(newDate._year, newDate._month);
                    newDate._month++;
                }
                return ( sum = addYear*365 - sum + this->_day - date._day );
            }
        }
    }
    bool operator<(const Date& date)/*<运算符重载*/
    {
        if (this->_year < date._year || 
            (this->_year == date._year && this->_month < date._month) || 
            (this->_month == date._month && this->_day < date._day)
            )
        {
            return true;
        }
        return false;
    }
    bool operator <=(const Date& date)/*'<'运算符重载*/
    {
        if ( *this < date || 
            (this->_year == date._year && 
            this->_month == date._month&&
            this->_day == date._day) )
        {
            return true;
        }
        return false;
    }

    bool operator>(const Date& date)/*'>'运算符重载*/
    {
        if ( this->_year > date._year||
            (this->_year == date._year && this->_month > date._month) ||
            (this->_month == date._month && this->_day > date._day) )
        {
            return true;
        }
        return false;
    }
    bool operator >=(const Date& date) /*'>='运算符重载*/
    {
        if (*this > date ||
            (this->_year == date._year &&
            this->_month == date._month &&
            this->_day == date._day) )
        {
            return true;
        }
        return false;
    }
    bool operator==(const Date& date)
    {
        if (this->_year == date._year &&
            this->_month == date._month &&
            this->_day == date._day)
        {
            return true;
        }
        return false;
    }
    bool operator!=(const Date& date)
    {
        if ( this->_year != date._year ||
            (this->_year == date._year && 
            this->_month != date._month )||
            (this->_month == date._month &&
            this->_day != date._day) )
        {
            return true;
        }
        return false;
    }
    ~Date()
    {
    }
    void Display()/*打印*/
    {
    cout<<_year<<"-"<<_month<<"-"<<_day<<endl;
    }
private:
    int _year;
    int _month;
    int _day;
};
