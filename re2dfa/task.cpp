#include "api.hpp"
#include <cctype>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>

using namespace std;

ostream& operator<<(ostream &out, set<int> i)
{
	out << '{';
	for(int j: i)
		out << j << ",";
	out << "}\n";
	return out;
}

void add_in_set(set<int> &s, set<int> &dest)
{
	for(int i: s)
	{
		dest.insert(i);
	}
}

struct node 
{
	char value;
	int position = 0;
	bool nullable = false;
	set<int> firstpos;
	set<int> lastpos;
	node* left = nullptr;
	node* right = nullptr;
};

string add_eps(string s)
{
	s = '(' + s + ')' + '#';
	set<char> eps_set1 = {'|', '('};
	set<char> eps_set2 = {'|', ')', '*'};
	set<char> con_set1 = {')', '*'};
	set<char> con_set2 = {'('};
	set<char> op = {'|', '(', ')', '*'};
	auto p1 = s.begin();
	auto p2 = p1+1;
	string res;
	while(p2 != s.end())
	{
		res.push_back(*p1);
		if (eps_set1.count(*p1) && eps_set2.count(*p2))
			res.push_back('_');
		if ((con_set1.count(*p1) && con_set2.count(*p2)) || (!op.count(*p1) && con_set2.count(*p2)) || (con_set1.count(*p1) && !op.count(*p2)) || (!op.count(*p1) && !op.count(*p2)))
			res.push_back('.');
		p1++;
		p2++;
	}
	res.push_back(*p1);
	return res;
}
string make_poliz(string s)
{
	string rez;
	string stack;
	map<char, int> priority = {{'|', 1}, {'.', 2}, {'*', 3}};
	set<char> op = {'(', ')', '.', '*', '|'};
	for (auto c: s)
	{
		if(!op.count(c))
			rez.push_back(c);
		else
			if(c == '(')
				stack.push_back(c);
			else if (c == ')')
			{
				while (stack.back() != '(') {
					rez.push_back(stack.back());
					stack.pop_back();
				}
				stack.pop_back();
			}
			else {
					while (!stack.empty() && stack.back() != '(' && priority[stack.back()] >= priority[c]) {
						rez.push_back(stack.back());
						stack.pop_back();
					}

					stack.push_back(c);
			}
	}
	while (!stack.empty()) 
	{
		rez.push_back(stack.back());
		stack.pop_back();
	}
	return rez;
}

pair<node*, map<int, pair<char, set<int>>>> make_tree(string poliz)
{
	list<node*> stack;
	map<int, pair<char, set<int>>> followpos;
	set<char> op = {'|', '*', '.'};
	int n = 1;
	for(char c: poliz)
	{
		node* top = new node;
		switch (c) {
			case '|':
			{
				top->value = c;
				node* op2 = stack.back();
				stack.pop_back();
				node* op1 = stack.back();
				stack.pop_back();
				top->left = op1;
				top->right = op2;
				top->nullable = op1->nullable || op2->nullable;
				add_in_set(op1->firstpos, top->firstpos);
				add_in_set(op2->firstpos, top->firstpos);
				add_in_set(op1->lastpos, top->lastpos);
				add_in_set(op2->lastpos, top->lastpos);
				break;
			}
			case '.':
			{
				node* op2 = stack.back();
				stack.pop_back();
				node* op1 = stack.back();
				stack.pop_back();
				top->left = op1;
				top->right = op2;
				top->value = c;
				top->nullable = op1->nullable && op2->nullable;
				top->firstpos = op1->firstpos;
				if(op1->nullable)
				{
					add_in_set(op2->firstpos, top->firstpos);
				}
				top->lastpos = op2->lastpos;
				if(op2->nullable)
				{
					add_in_set(op1->lastpos, top->lastpos);
				}
				for(int i: op1->lastpos)
				{
					add_in_set(op2->firstpos, followpos[i].second);
				}
				break;
			}
			case '*':
			{
				node* op1 = stack.back();
				stack.pop_back();
				top->left = op1;
				top->nullable = true;
				top->firstpos = op1->firstpos;
				top->lastpos = op1->lastpos;
				for(int i: op1->lastpos)
				{
					add_in_set(op1->firstpos, followpos[i].second);
				}
				break;
			}
			default:
			{
				top->value = c;
				top->position = n;
				top->firstpos.insert(n);
				top->lastpos.insert(n);
				followpos[n].first = c;
				n++;
				if (c == '_')
					top->nullable = true;
				break;
			}
		}
		stack.push_back(top);
	}
	return {stack.back(), followpos};
}
DFA re2dfa(const std::string &s) {
	if (s == "")
	{
		DFA res = DFA(Alphabet("ababaa"));
		res.create_state("1");
		res.set_initial("1");
		res.make_final("1");
		return res;
	}
	DFA res = DFA(Alphabet(s));
	string alph = res.get_alphabet().to_string();
	map<set<int>, string> res_map;
	string poliz = add_eps(s);
	poliz = make_poliz(poliz);
	cout << poliz << endl;
	pair<node*, map<int, pair<char, set<int>>>> tree = make_tree(poliz);
	node* res_tree = tree.first;
	map<int, pair<char, set<int>>> flp = tree.second;
	int end_num;
	for(const auto& i: flp)
	{
		if(i.second.first == '#')
			end_num = i.first;
	}
	for(auto& i: flp)
	{
		cout << i.first << "-" << i.second.second;
	}
	map<set<int>, string> marked;
	int n = 1;
	res_map[res_tree->firstpos] = to_string(n);
	res.create_state(to_string(n));
	res.set_initial(to_string(n));
	if(res_map.begin()->first.count(end_num))
	{
		res.make_final(to_string(n));
	}
	n++;
	while(!res_map.empty())
	{
		pair<set<int>, string> cur_st = *res_map.begin();
		for(char c: alph)
		{
			set<int> temp;
			for(int i: cur_st.first)
			{
				if(flp[i].first != c)
					continue;
				temp.insert(flp[i].second.cbegin(), flp[i].second.cend());
			}

			if(temp.empty())
				continue;
			string dest;
			if(marked.count(temp))
				dest = marked[temp];
			else if(res_map.count(temp))
				dest = res_map[temp];
			else
			{
				dest = res_map[temp] = to_string(n);
				n++;
				res.create_state(dest);
			}
			
			res.set_trans(cur_st.second, c, dest);
			if(temp.count(end_num))
				res.make_final(dest);


		}
		marked[cur_st.first] = cur_st.second;
		res_map.erase(cur_st.first);
	}
	return res;
}