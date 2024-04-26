#include "val.h"
#include <stdexcept>
#include <cmath>

Value Value::operator*(const Value &op) const
{
    if (IsInt())
    {
        if (op.IsInt())
            return Value(GetInt() * op.GetInt());
        else if (op.IsReal())
            return Value((double)GetInt() * op.GetReal());
        else
            return Value();
    }
    else if (IsReal())
    {
        if (op.IsInt())
            return Value(GetReal() * (double)op.GetInt());
        else if (op.IsReal())
            return Value(GetReal() * op.GetReal());
        else
            return Value();
    }
    return Value();
}

// Value Value::operator+(const Value &op) const
// {
// }
Value Value::operator==(const Value &op) const
{
    if (IsInt())
    {
        if (op.IsInt())
            return Value(GetInt() == op.GetInt());
        else if (op.IsReal())
            return Value((double)GetInt() == op.GetReal());
        else
            return Value();
    }
    else if (IsReal())
    {
        if (op.IsInt())
            return Value(GetReal() == (double)op.GetInt());
        else if (op.IsReal())
            return Value(GetReal() == op.GetReal());
        else
            return Value();
    }
    return Value();
}

Value Value::operator<(const Value &op) const
{
    if (IsInt())
    {
        if (op.IsInt())
            return Value(GetInt() < op.GetInt());
        else if (op.IsReal())
            return Value((double)GetInt() < op.GetReal());
        else
            return Value();
    }
    else if (IsReal())
    {
        if (op.IsInt())
            return Value(GetReal() < (double)op.GetInt());
        else if (op.IsReal())
            return Value(GetReal() < op.GetReal());
        else
            return Value();
    }
    return Value();
}

Value Value::operator>(const Value &op) const
{
    if (IsInt())
    {
        if (op.IsInt())
            return Value(GetInt() > op.GetInt());
        else if (op.IsReal())
            return Value((double)GetInt() > op.GetReal());
        else
            return Value();
    }
    else if (IsReal())
    {
        if (op.IsInt())
            return Value(GetReal() > (double)op.GetInt());
        else if (op.IsReal())
            return Value(GetReal() > op.GetReal());
        else
            return Value();
    }
    return Value();
}

Value Value::Catenate(const Value &op) const
{
    if (IsString() && op.IsString())
    {
        return Value(GetString() + op.GetString());
    }
    else
    {
        Value errorValue;
        errorValue.SetType(VERR);
        return errorValue;
    }
}

Value Value::Power(const Value &op) const
{
    if (GetType() == op.GetType())
    {
        if (IsInt())
        {
            return Value((int)pow(Itemp, op.GetInt()));
        }
        else if (IsReal())
            return Value(pow(Rtemp, op.GetReal()));
    }
    else if (IsReal() && op.IsInt())
        return Value(pow(Rtemp, (double)op.GetInt()));
    else if (IsInt() && op.IsReal())
        return Value(pow((double)Itemp, op.GetReal()));
    else if (IsString() || op.IsString())
        return Value();
    return Value();
}

Value Value::operator+(const Value &op) const
{
    if (GetType() == op.GetType())
    {
        if (IsInt())
            return Value(Itemp + op.GetInt());
        else if (IsReal())
            return Value(Rtemp + op.GetReal());
    }
    else if (IsReal() && op.IsInt())
        return Value(Rtemp + (double)op.GetInt());
    else if (IsInt() && op.IsReal())
        return Value((double)Itemp + op.GetReal());
    return Value();
}

// numeric overloaded subtract op from this
Value Value::operator-(const Value &op) const
{
    double oper, oper_2;
    if (GetType() == op.GetType())
    {
        if (IsInt())
            return Value(Itemp - op.GetInt());
        else if (IsReal())
            return Value(Rtemp - op.GetReal());
        else if (IsString())
        { // handles both strings
            try
            {
                oper = stod(GetString());
                oper_2 = stod(op.GetString());
            }
            catch (...)
            {
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
            return Value(oper - oper_2);
        }
    }
    else if (IsReal() && op.IsInt())
        return Value(Rtemp - (double)op.GetInt());
    else if (IsInt() && op.IsReal())
        return Value((double)Itemp - op.GetReal());
    else if (op.IsString())
    {
        if (!(IsReal() || IsInt()))
            return Value();
        try
        {
            oper = stod(op.GetString());
        }
        catch (...)
        {
            cout << "Invalid conversion from string to double." << endl;
            return Value();
        }
        if (IsInt())
            return Value((double)GetInt() - oper);
        else if (IsReal())
            return Value(GetReal() - oper);
    }
    else if (IsString())
    {
        if (!(op.IsReal() || op.IsInt()))
            return Value();
        try
        {
            oper = stod(GetString());
        }
        catch (...)
        {
            cout << "Invalid conversion from string to double." << endl;
            return Value();
        }
        if (op.IsInt())
            return Value(oper - (double)op.GetInt());
        else if (op.IsReal())
            return Value(oper - op.GetReal());
    }
    return Value();
}

Value Value::operator/(const Value &op) const
{
    double oper, oper_2;
    if ((op.IsInt() && op.GetInt() == 0) || (op.IsReal() && op.GetReal() == 0.0))
    {
        // cout << "Illegal operand type for the operation." << endl;
        return Value();
    }
    if (GetType() == op.GetType())
    {
        if (IsInt())
            return Value(Itemp / op.GetInt());
        else if (IsReal())
            return Value(Rtemp / op.GetReal());
        else if (IsString())
        { // handles both strings
            try
            {
                oper = stod(GetString());
                oper_2 = stod(op.GetString());
            }
            catch (...)
            {
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
            return Value(oper / oper_2);
        }
    }
    else if (IsReal() && op.IsInt())
        return Value(Rtemp / (double)op.GetInt());
    else if (IsInt() && op.IsReal())
        return Value((double)Itemp / op.GetReal());
    else if (op.IsString())
    {
        if (!(IsReal() || IsInt()))
            return Value();
        try
        {
            oper = stod(op.GetString());
        }
        catch (...)
        {
            cout << "Invalid conversion from string to double." << endl;
            return Value();
        }
        if (IsInt())
            return Value((double)GetInt() / oper);
        else if (IsReal())
            return Value(GetReal() / oper);
    }
    else if (IsString())
    {
        if (!(op.IsReal() || op.IsInt()))
            return Value();
        try
        {
            oper = stod(GetString());
        }
        catch (...)
        {
            cout << "Invalid conversion from string to double." << endl;
            return Value();
        }
        if (op.IsInt())
            return Value(oper / (double)op.GetInt());
        else if (op.IsReal())
            return Value(oper / op.GetReal());
    }
    return Value();
}