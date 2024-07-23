#include "api.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <map>

using namespace std;

ostream& operator<< (ostream& out, map<pair<string, string>, string> m)
{
  for(auto i: m)
  {
    out << "{<" << i.first.first << "," << i.first.second << "> " << i.second << "}" << endl;
  }
  return out;
}

map<pair<string, string>, string> change_dfa(DFA d)
{
  map<pair<string, string>, string> res;
  set<string> st = d.get_states();
  string alph = d.get_alphabet().to_string();
  for(const string &s: st)
  {
    for(char c: alph)
    {
      // cout << c << endl;
      if(d.has_trans(s, c))
      {
        // cout << c << endl;
        string l = "";
        string tmp = d.get_trans(s, c);
        // cout << tmp << "-tmp" << endl;
        l.push_back(c);
        // cout << l << "-l" << endl;
        if (res.count({s, tmp}))
          l = "(" + l + "|" + res[{s, tmp}] + ")";
        res[{s, tmp}] = l;
      }
    }
    if(d.is_final(s))
        {
          res[{s, "_fin_"}] = "";
        }
  }
  return res;
}

string dfa2re(DFA &d) 
{
  map<pair<string, string>, string> buf = change_dfa(d);
  cout << buf << endl;
  set<string> st = d.get_states();
  set<string> with_fin = st;
  st.erase(d.get_initial_state());
  with_fin.insert("_fin_");
  for (string s: st)
  {
    set<pair<string, string>> for_delete;
    cout << s << endl;
    string loop = "";
    if(buf.count({s, s}))
    {
      loop += "(" + buf[{s, s}]+ ")*";
      buf.erase({s, s});
      cout << "(" + s + "," + s + ")" << " loop " << endl;
    }
    set<string> arr_in;
    set<string>arr_out;
    for(auto i: with_fin)
    {
      if(s!=i && buf.count({i, s}))
      {
        for(auto j: with_fin)
        {
          if(s!=j && buf.count({s, j}))
          {
            string rule = buf[{i, s}] + loop + buf[{s, j}];
            if(buf.count({i, j}) && buf[{i, j}] != rule)
            {
              rule = "(" + rule + "|" + buf[{i, j}] + ")";
            }
            buf[{i, j}] = rule;
            cout << "(" + s + "," + j + ")" << endl;
            for_delete.insert({s, j});
          }
        }
        for_delete.insert({i, s});
        cout << "(" + i + "," + s + ")" << " is" << endl;
      }
      
    }
    for(auto &i: for_delete)
    {
      buf.erase(i);
    }
    // cout << buf;
  }
  string res = buf[{d.get_initial_state(), "_fin_"}];
  if(buf.count({d.get_initial_state(), d.get_initial_state()}))
  {
    res = "(" + buf[{d.get_initial_state(), d.get_initial_state()}] + ")" + "*" + res;
  }
  return res;
}
