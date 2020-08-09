/*
 * Data Structures - Simple stack implementation
 *
 * Copyright (C) Kadir Yanık - <kdrynkk@gmail.com>, 2020
 */
#include "stack.h"

#include <QDebug>

/*---------------------------------------------------------------------------*/
template <class T>
Stack<T>::Stack(int size)
{
    this->stack = new T[size];
    if(this->stack == NULL){
        qDebug() << "Stack allocation failed!";
        exit(-1);
    }

    this->stackSize = size;
    this->stackOffset = 0;
}

/*---------------------------------------------------------------------------*/
template <class T>
Stack<T>::~Stack()
{
    if(this->stack != NULL){
        delete [] this->stack;
    }
}

/*---------------------------------------------------------------------------*/
template <class T> bool
Stack<T>::push(T val)
{
    if(!isFull()){
        stack[++(this->stackOffset)] = val;
        return true;
    }

    qDebug() << "Stack full!";
    return false;
}

/*---------------------------------------------------------------------------*/
template <class T> bool
Stack<T>::pop(T *ret)
{
    if(!isEmpty() && ret != NULL){
        (*ret) = stack[this->stackOffset--];
        return true;
    }

    return false;
}

/*---------------------------------------------------------------------------*/
template <class T> bool
Stack<T>::peek(T *ret)
{
    if(!isEmpty() && ret != NULL){
        (*ret) = stack[this->stackOffset];
        return true;
    }

    return false;
}

/*---------------------------------------------------------------------------*/
template <class T> bool
Stack<T>::isEmpty()
{
    if(this->stackOffset == 0){
        return true;
    }

    return false;
}

/*---------------------------------------------------------------------------*/
template <class T> bool
Stack<T>::isFull()
{
    if(this->stackOffset < this->stackSize){
        return false;
    }

    qDebug() << "Stack full!";
    return true;
}

/*---------------------------------------------------------------------------*/
template <class T> void
Stack<T>::clear()
{
    this->stackOffset = 0;
}
