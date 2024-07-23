#include "api.hpp"
#include <ostream>
#include <string>
#include <map>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

ostream& operator<<(ostream &out, set<string> i)
{
	out << '{';
	for(string j: i)
		out << j << ",";
	out << "}";
	return out;
}

void delete_unreach(DFA &d)
{
  string alph = d.get_alphabet().to_string();
  set<string> reachable;
  reachable.insert(d.get_initial_state());
  int prev_sz = 0;
  int curr_sz = reachable.size();
  while (prev_sz != curr_sz) 
  {
    set<string> buf;
    for(string s: reachable)
    {
      for(char c: alph)
      {
        if(d.has_trans(s, c))
          buf.insert(d.get_trans(s, c));
      }
    }
    reachable.insert(buf.begin(), buf.end());
    prev_sz = curr_sz;
    curr_sz = reachable.size();
  }
  for(string st: d.get_states())
  {
    if(!reachable.count(st))
    {
      d.delete_state(st);
    }
  }
}

void delete_deadend(DFA& d)
{
  set<string> fin = d.get_final_states();
  ////cout << fin << endl;
  set<string> not_de = fin;
  string alph = d.get_alphabet().to_string();
  for(string s: d.get_states())
  {
    bool flag = false;
    // if(d.is_final(s))
    //   continue;
    for(char c: alph)
    {
      if(d.has_trans(s, c))
        flag = flag || fin.count(d.get_trans(s, c));
    }
    if(flag)
      not_de.insert(s);
  }
  int prev_sz = 0;
  int cur_sz = not_de.size();
  while(prev_sz != cur_sz)
  {
    set<string> buf;
    for(string s: d.get_states())
    {
      bool flag = false;
      for(char c: alph)
      {
        if(d.has_trans(s, c))
          flag = flag || not_de.count(d.get_trans(s, c));
      }
      if(flag)
        buf.insert(s);
    }
    not_de.insert(buf.begin(), buf.end());
    prev_sz = cur_sz;
    cur_sz = not_de.size();
    not_de.insert(fin.begin(), fin.end());
  }
  for(string s: d.get_states())
  {
    if(!not_de.count(s))
      d.delete_state(s);
  }
}

void add_de(DFA& d)
{
  string alph = d.get_alphabet().to_string();
  // d.create_state("_de_");
  string de = "_de_";
  for(string s: d.get_states())
  {
    for(char c: alph)
    {
      if(!d.has_trans(s, c))
      {
        if(!d.has_state(de))
          {
            d.create_state(de);
            for(char c1: alph){
              d.set_trans(de, c1, de);
            }
          }
          d.set_trans(s, c, de);
      }
    }
  }
}

DFA dfa_minim(DFA &d1) {
  DFA d = d1;
  ////cout << d.get_states() << " without change" << endl;
  delete_unreach(d);
  //cout << d.get_states() << " without ubreach" << endl;
  delete_deadend(d);
  // //cout << d.to_string() << endl;
  //cout << d.get_states() << " without deadend" << endl;
  if(d.get_states().empty())
  {
    DFA new_d(d.get_alphabet());
    new_d.create_state("0");
    return new_d;
  }
  add_de(d);
  cout << d.get_states() << " new deadend" << endl;
  // //cout << d.to_string() << endl;
  // //cout << "!\n";
  set<string> finals = d.get_final_states();
  set<string> no_finals = d.get_states();
  string alph = d.get_alphabet().to_string();
  for(string s: finals)
  {
    no_finals.erase(s);
  }
  set<set<string>> p = {finals, no_finals};
  for(auto ptr: p)
      {
        cout << ptr << ":p1" << endl;
      }
  set<set<string>> s = p;
  while (!s.empty()) {
    set<string> c = *s.begin();
    s.erase(c);
    for(char letter: alph)
    {
      set<set<string>> buf;
      for(auto &r: p)
      {
        set<string> new_state;
        // set<string> old_state;
        // if(rules.count({r, c}))
        //   rules.erase({r, c});
        set<string>r1;
        set<string>r2;
        for(string str: r)
        {
          if(!c.count(d.get_trans(str, letter)))
          {
            r1.insert(str);
            new_state.insert(d.get_trans(str, letter));
          }
          else if (c.count(d.get_trans(str, letter)))
            r2.insert(str);
        }
        // p.erase(r);
        if(!r1.empty())
        {
          buf.insert(r1);
          // //cout <<"11\n" << r1 << "\n\n";          
          // rules.insert({{r1, new_state}, letter});
        }
        if(!r2.empty())
        {
          buf.insert(r2);
          // //cout <<"22\n" << r2 << "\n\n";
          // rules.insert({{r2, c}, letter});
        }
        if(!r1.empty() && !r2.empty())
        {
          s.insert(r1);
          s.insert(r2);
        }
      }
      p = buf;
      for(auto ptr: p)
      {
        cout << ptr << ":p" << endl;
      }
      cout << endl;
    }
    // //cout << "*\n";
  }
  char n = '0';
  map<set<string>, string> names;
  DFA new_d(d.get_alphabet()); 
  for(auto i: p)
  {
    names[i] = string(1, n);
    //cout << i << "-" << n << endl;
    new_d.create_state(string(1, n));
    if(i.count(d.get_initial_state()))
    {
      new_d.set_initial(string(1, n));
    }
    for(string s: d.get_final_states())
    {
      if(i.count(s))
      {
        new_d.make_final(string(1, n));
        break;
      }
    }
    n++;
  }
  for(auto ptr: p)
  {
    for(char c: alph)
    {
      if(d.has_trans(*ptr.begin(), c))
      {
        string dest = d.get_trans(*ptr.begin(), c);
        for(auto ptr2: p)
        {
          if(ptr2.count(dest))
          {
            new_d.set_trans(names[ptr], c, names[ptr2]);
          }
        }
      }
    }
  }
  delete_deadend(new_d);
  return new_d;
}