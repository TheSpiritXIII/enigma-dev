/*********************************************************************************\
**                                                                              **
**  Copyright (C) 2008 Josh Ventura                                             **
**                                                                              **
**  This file is a part of the ENIGMA Development Environment.                  **
**                                                                              **
**                                                                              **
**  ENIGMA is free software: you can redistribute it and/or modify it under the **
**  terms of the GNU General Public License as published by the Free Software   **
**  Foundation, version 3 of the license or any later version.                  **
**                                                                              **
**  This application and its source code is distributed AS-IS, WITHOUT ANY      **
**  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS   **
**  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more       **
**  details.                                                                    **
**                                                                              **
**  You should have recieved a copy of the GNU General Public License along     **
**  with this code. If not, see <http://www.gnu.org/licenses/>                  **
**                                                                              **
**  ENIGMA is an environment designed to create games and other programs with a **
**  high-level, fully compilable language. Developers of ENIGMA or anything     **
**  associated with ENIGMA are in no way responsible for its users or           **
**  applications created by its users, or damages caused by the environment     **
**  or programs made in the environment.                                        **
**                                                                              **
\*********************************************************************************/

#include <iostream>
#include <stdlib.h>
#include "references.h"

rf_stack refstack;

referencer::referencer(char s): symbol(s), count(0), completed(1) {}
referencer::referencer(char s,unsigned short c): symbol(s), count(c), completed(1) {}
referencer::referencer(char s,unsigned short c,char complete): symbol(s), count(c), completed(complete) {}
referencer::referencer(char s,unsigned short c,unsigned short cmin,char complete): symbol(s), count(c), countmin(cmin), completed(complete) {}
referencer::referencer(const referencer &r): symbol(r.symbol), count(r.count), completed(r.completed) {}

rf_node::rf_node(): next(NULL), prev(NULL), ref(0,0,0) {}
rf_node::rf_node(const referencer &r): next(NULL), prev(NULL), ref(r) {}
rf_node::rf_node(rf_node* Prev,const referencer &r): next(NULL), prev(Prev), ref(r) {}
rf_node::rf_node(const referencer &r,rf_node* Next): next(Next), prev(NULL), ref(r) {}
rf_node::rf_node(rf_node* Prev,const referencer &r,rf_node* Next): next(Next), prev(Prev), ref(r) {}

rf_stack::rf_stack(int x): first(NULL), last(NULL), now(NULL) {};
rf_stack::rf_stack(): first(NULL), last(NULL), now(NULL) {};

char rf_stack::currentsymbol()
{
  if (now == NULL) return 0;
  return now->ref.symbol;
}
char rf_stack::topmostsymbol()
{
  if (last == NULL) return 0;
  return last->ref.symbol;
}
char rf_stack::nextsymbol()
{
  if (now == NULL or now->next == NULL) return 0;
  return now->next->ref.symbol;
}
char rf_stack::prevsymbol()
{
  if (now == NULL or now->prev == NULL) return 0;
  return now->prev->ref.symbol;
}

unsigned short rf_stack::currentcount()
{
  if (now == NULL) return 0;
  return now->ref.count;
}
unsigned short rf_stack::topmostcount()
{
  if (last == NULL) return 0;
  return last->ref.count;
}
bool rf_stack::currentcomplete()
{
  if (now == NULL) return 0;
  return now->ref.completed;
}
bool rf_stack::topmostcomplete()
{
  if (last == NULL) return 0;
  return last->ref.completed;
}
unsigned short rf_stack::nextcount()
{
  if (now == NULL or now->next == NULL) return 0;
  return now->next->ref.count;
}
unsigned short rf_stack::prevcount()
{
  if (now == NULL or now->prev == NULL) return 0;
  return now->prev->ref.count;
}

void rf_stack::inc_current()
{
  if (now == NULL) return;
  now->ref.count++;
  now->ref.countmin++;
}
//Increment the current minimum arg count
void rf_stack::inc_current_min()
{
  if (now == NULL) return;
  now->ref.count++;
  now->ref.countmin++;
}

rf_stack &rf_stack::operator += (referencer r)
{
  if (first == NULL)
    first=last=now=new rf_node(r);
  else
  {
    if (now->ref.completed)
    {
      if (last == now) last = now = new rf_node(last,r);
      else if (now->next != NULL) now = now->next = now->next->prev = new rf_node(now,r,now->next);
      else now = now->next = new rf_node(now,r,now->next);
    }
  }
  return *this;
}
rf_stack &rf_stack::operator += (const rf_stack &r)
{
  for (rf_node* i = r.first; i != NULL; i = i->next)
    *this += i->ref;
  return *this;
}

rf_stack &rf_stack::operator++ (int nothing)
{
  if (now != NULL)
    now->ref.completed = 1;
  return *this;
}

//This will complete a set of function parameters,
//Or move past the last opened set of parentheses
rf_stack &rf_stack::operator-- (int nothing)
{
  if (now != NULL)
  {
    while (now->prev != NULL)
    {
      if (now->ref.completed)
      {
        if (now->ref.symbol == '(' or now->ref.symbol == ')')
        {
          now = now->prev;
          return *this;
        }
        now = now->prev;
      }
      else
      {
        now->ref.completed = 1;
        return *this;
      }
    }
    if (now->ref.symbol == '(' or now->ref.symbol == ')')
      first = now = now->prev = new rf_node(NULL, referencer(')'), now);
  }
  return *this;
}


//This is what STD::String does when freeing after return.
//We'll be doing that manually, though.
rf_stack rf_stack::dissociate()
{
  rf_stack r = *this;
  first = last = now = NULL;
  return r;
}

//This destroys the stack. It is what STD::String does on free,
//if the contents of the string have not been referenced elsewhere.
void rf_stack::dump()
{
  rf_node* n = NULL;
  for (rf_node* i = first; i != NULL; i = n)
  {
    n = i->next;
    delete i;
  }
  first = last = now = NULL;
}

bool rf_stack::empty()
{
  return first == NULL;
}