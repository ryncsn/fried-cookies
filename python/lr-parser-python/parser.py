#!/usr/bin/env python3
# encoding: utf-8
"""
An LR parser

Parses statements like:

while (a3+15) > 0xa do
    if x2 = 07 then
        while y<z do
            y = x * y / z;
c = b * c + d;

Parse into AST then generate pseudo asm code
"""

import sys
import collections
import re

"V"

Token = collections.namedtuple('Token', ['name', 'value', 'line', 'column'])

def tokenize(s):
    s = s+"#"
    keywords = {'if', 'then', 'else', 'while', 'do'}
    token_specification = [
        ('END',     r'\#'),
        ('FLOAT',   r'\d+\.\d*'),
        ('HEX',     r'0x[a-f0-9]+'),
        ('OCT',     r'0[0-7]+'),
        ('DEC',     r'0|\d+'),
        ('ID',      r'[A-Za-z]+[0-9]*'),
        ('ADD',     r'\+'),
        ('MIN',     r'-'),
        ('MUL',     r'\*'),
        ('DIV',     r'/'),
        ('Lb',      r'\('),
        ('Rb',      r'\)'),
        ('LES',     r'<'),
        ('BIG',     r'>'),
        ('EQU',     r'='),
        ('SEMI',    r'\;'),
        ('LB',      r'{'),
        ('RB',      r'}'),
        ('NEWLINE', r'\n'),
        ('SKIP',    r'[ \t]'),
    ]
    tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)
    get_token = re.compile(tok_regex).match
    line, pos, line_start = 1, 0, 0
    mo = get_token(s)
    while mo is not None:
        typ = mo.lastgroup
        if typ == 'NEWLINE':
            line_start = pos
            line += 1
        elif typ != 'SKIP':
            val = mo.group(typ)
            if typ == 'ID' and val in keywords:
                typ = val.upper()
            yield Token(typ, val, line, mo.start()-line_start)
        pos = mo.end()
        mo = get_token(s, pos)
    if pos != len(s):
        raise RuntimeError('Unexpected character %r on line %d' %(s[pos], line))


"P"

class L(object):

    "Grammer Rule"
    rule = collections.namedtuple('rule', ['name', 'prod', 'func'])

    "Node indicate a production"
    node = collections.namedtuple('node', ['token', 'rule', 'children', 'id' ])
    
    def __init__(self,vt,vn,rl):
        self.rules,self.Vt,self.Vn=rl,vt,vn 
        self.V    =self.Vn+self.Vt

        "Check for error"
        if any(self.Vn) in self.Vt:
            raise RuntimeError("Symbol name conflict!")
        r = 1
        while r < len(self.rules):
            if not self.rules[r].name in self.Vn:
                raise RuntimeError("Unexpected symbol: %s!"%self.rule[r].name)
            for s in self.rules[r].prod:
                if not s in self.V:
                    raise RuntimeError("Unexpected symbol: %s!"%s)
            r = r+1

    def First(self,symb):
        if symb in self.Vt:
            return [symb,] 
        first=[]
        for name,prod,func in self.rules:
            if name == symb and name != prod[0]:
                first.extend(self.First(prod[0]))
        return list(set(first)) 
 
    def genTable(self):
        raise RuntimeError("Class L is not a analyzer")
 
class LR0(L):
 
    "LR0 item"
    item = collections.namedtuple('item', ['name', 'left', 'rest'])

    def __init__(self,vt,vn,rl):
        L.__init__(self,vt,vn,rl)

class LR1(L):
 
    def __init__(self,vt=None,vn=None,rl=None):
        L.__init__(self,vt,vn,rl)

    def Closure(self,I):
        while True:
            changed = False
            for itei,fore in I:
                if itei.rest and itei.rest[0] in self.Vn:
                    for name,repl,func in self.rules:
                        if name==itei.rest[0]:
                            x=LR0.item(name,[],repl)
                            for u in I:
                                if x==u[0]:
                                    if len(itei.rest) > 1:
                                        nfore=list(set(u[1]+(self.First(itei.rest[1]))))
                                        if not set(nfore).issubset(set(u[1])):
                                            u[1] = nfore;
                                            changed = True
                                    elif not set(fore).issubset(set(u[1])):
                                        u[1]=list(set(u[1]+fore))
                                        changed = True
                                    break
                            else:
                                I.append([x,self.First(itei.rest[1]) if len(itei.rest)>1 else fore])
                                changed = True
            if not changed: break
        return I

    def Goto(self,I,x):
        J=[]
        for g,h in I:
            if g.rest and g.rest[0]==x:
                J.append([item(g.name,g.left+g.rest[:1],g.rest[1:]),h])
        return self.Closure(J)

    def genTable(self):
        I0=self.Closure([ [LR0.item(self.rules[0].name,[],self.rules[0].prod),['END']] ])

        "Gen C"
        self.C = [I0]

        while True:
            changed = False
            for I in self.C:
                for x in self.V:
                    J=self.Goto(I,x)
                    if J == [] or  J in self.C: continue
                    else:
                        self.C.append(J)
                        changed = True
            if not changed: break

        self.action = {};
        self.goto = {};

        for i,I in enumerate(self.C):
            for itei,fore in I:
                if itei.rest and itei.rest[0] in self.Vt:
                    x = self.Goto(I,itei.rest[0])
                    for y,z in enumerate(self.C):
                        if x == z:
                            self.action[(i,itei.rest[0])]=['S',y]

                for B in self.Vn:
                    x = self.Goto(I,B)
                    for y,z in enumerate(self.C):
                        if x == z:
                            self.goto[(i,B)]=y

                if itei.rest == []:
                    if itei.name==self.rules[0].name:
                        self.action[(i,'END')]=['acc']
                        continue
                    for j,R in enumerate(self.rules):
                        if itei.name==R.name and itei.left==R.prod:
                            for k in fore:
                                self.action[(i,k)]=['r',j]
             
        self.action = collections.defaultdict(lambda: None, self.action)
        self.goto = collections.defaultdict(lambda: None, self.goto)       
        pass

    def analyze(self, tokens):
        stack = [0]
        stack_code = []

        node = L.node

        if not self.action or not self.goto:
            raise RuntimeError("")

        action = self.action
        goto = self.goto
        node_count = 0

        for token in tokens:
            Unhandle = True
            while Unhandle:
                now = action[(stack[-1],token.name)]
                if not now:
                    raise RuntimeError("Unexpected Token %s at %d:%d (%d)"%(token.name,token.line,token.column,stack[-1]))
                if now[0] == 'S':
                    stack.append(now[1])
                    stack_code.append(node(token,None,None,node_count))
                    node_count += 1
                    Unhandle = False
                    continue
                elif now[0] == 'r':
                    now_child = []
                    now_rule = self.rules[now[1]]
                    
                    for i in now_rule.prod:
                        now_child.insert(0,stack_code.pop())
                        stack.pop()
                    stack_code.append(node(None,now_rule,now_child,node_count))
                    node_count += 1
                    stack.append(goto[stack[-1],now_rule.name])
                    pass
                elif now[0] == 'acc':
                    self.top = stack_code.pop()
                    Unhandle = False
                    break
                else:
                    raise RuntimeError("Fatal Error")

    

ids = {}
idnum = 0
def getPlace(ident):
    global idnum
    if ids.get(ident):
        return ids[ident]
    else:
        ids[ident] = "memory%d"%idnum
        idnum += 1
        return ids[ident]

def turnCode(this,child):
    ids[child.id] = getPlace(this.id)
    return genCode(child)

def genCode(node):
    if node.rule:
        return node.rule.func(node,node.children)
    else:
        return node.token.value

lbs = {}
def getLable(ifnode):
    if lbs.get(ifnode):
        return lbs[ifnode]
    else:
        lbs[ifnode] = ['TRUE%d'%len(lbs),'FALSE%d'%len(lbs),'OUT%d'%len(lbs),'LOOP%d'%len(lbs)]
        return lbs[ifnode]

item=LR0.item
rule=L.rule

lr=LR1(['WHILE','IF','DO','THEN','ELSE',
        'EQU','ADD','MIN','MUL','DIV',
        'BIG','LES','Lb','Rb','LB','RB',
        'ID','DEC','OCT','HEX','SEMI','END'],
['P','L','S','C','E','T','F'],
[
    rule('P*',['P'],
        lambda this,child:genCode(child[0])),
    rule('P',['L'],
        lambda this,child:genCode(child[0])),
    rule('L',['L','P'],
        lambda this,child:genCode(child[0])+genCode(child[1])),
    rule('L',['S','SEMI'],
        lambda this,child:genCode(child[0])),
    rule('S',['ID','EQU','E'],
        lambda this,child:genCode(child[2])+"\n"+genCode(child[0])+" = "+getPlace(child[2].id)+"\n"),
    rule('S',['IF','C','THEN','S'],
        lambda this,child:genCode(child[1])+" goto "+getLable(this.id)[0]+"\n"+"goto "+getLable(this.id)[1]+"\n"+getLable(this.id)[0]+":\n"+genCode(child[3])+getLable(this.id)[1]+":\n"),
    rule('S',['IF','C','THEN','S','ELSE','S'],
        lambda this,child:genCode(child[1])+" goto "+getLable(this.id)[0]+"\n"+"goto "+getLable(this.id)[1]+"\n"+getLable(this.id)[0]+":\n"+genCode(child[3])+"goto "+getLable(this.id)[2]+"\n"+getLable(this.id)[1]+":\n"+genCode(child[5])+getLable(this.id)[2]+":\n"),
    rule('S',['WHILE','C','DO','S'],
        lambda this,child:getLable(this.id)[3]+":\n"+genCode(child[1])+" goto "+getLable(this.id)[0]+"\n"+"goto "+getLable(this.id)[1]+"\n"+getLable(this.id)[0]+":\n"+genCode(child[3])+"goto "+getLable(this.id)[3]+"\n"+ getLable(this.id)[1]+":\n"),
    rule('S',['LB','P','RB'],
        lambda this,child:genCode(child[1])),
    rule('C',['E','LES','E'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+"if "+getPlace(child[0].id)+" < "+getPlace(child[2].id)),
    rule('C',['E','BIG','E'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+"if "+getPlace(child[0].id)+" > "+getPlace(child[2].id)),
    rule('C',['E','EQU','E'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+"if "+getPlace(child[0].id)+" = "+getPlace(child[2].id)),
    rule('E',['E','ADD','T'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+getPlace(this.id)+" = "+getPlace(child[0].id)+" + "+getPlace(child[2].id)),
    rule('E',['E','MIN','T'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+getPlace(this.id)+" = "+getPlace(child[0].id)+" - "+getPlace(child[2].id)),
    rule('E',['T'],
        lambda this,child:turnCode(this,child[0])),
    rule('T',['T','MUL','F'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+getPlace(this.id)+" = "+getPlace(child[0].id)+" * "+getPlace(child[2].id)),
    rule('T',['T','DIV','F'],
        lambda this,child:genCode(child[0])+"\n"+genCode(child[2])+"\n"+getPlace(this.id)+" = "+getPlace(child[0].id)+" / "+getPlace(child[2].id)),
    rule('T',['F'],
        lambda this,child:turnCode(this,child[0])),
    rule('F',['Lb','E','Rb'],
        lambda this,child:turnCode(this,child[1])),
    rule('F',['ID'],
        lambda this,child:getPlace(this.id)+" = "+genCode(child[0])),
    rule('F',['DEC'],
        lambda this,child:getPlace(this.id)+" = "+genCode(child[0])),
    rule('F',['OCT'],
        lambda this,child:getPlace(this.id)+" = "+genCode(child[0])),
    rule('F',['HEX'],
        lambda this,child:getPlace(this.id)+" = "+genCode(child[0])),
])

lr.genTable()

ostatements = '''
    0  92+data>  0x3f  00  while a+acc>xx do x=x-1;
        a=6.2+a*0X88.80;l
            if a>b then a=b else a=b-1+c;
'''

statements = '''
while (a3+15)>0xa do
    if x2 = 07 then 
        while y<z do
            y = x * y / z;
c=b*c+d;
'''

if len(sys.argv) == 2:
    input = open(sys.argv[1])
    statements = ""
    for line in input:
        statements+=line

output = None
if len(sys.argv) > 2:
    output = open(sys.argv[2],'w')

for token in tokenize(statements):
    print(token)

print("Sets:")
for n,I in enumerate(lr.C):
    print("I%d"%n)
    for i in I:
        print(i[0].name, end = "")
        print(" -> ",end = "")
        for s in i[0].left:
            print(s,end = " ")
        print(".",end = " ")
        for s in i[0].rest:
            print(s,end = " ")
        print(", ",end = " ")
        for s in i[1]:
            print(s,end = " ")
        print()
    print()

print("\tAction Table | Goto Table")
for j in range(len(lr.Vt)):
    print (lr.Vt[j],end = "\t")
for j in range(len(lr.Vn)):
    print (lr.Vn[j],end = "\t")
print("")

for i in range(len(lr.C)):
    for j in range(len(lr.Vt)):
        if lr.action[(i,lr.Vt[j])]:
            for s in lr.action[(i,lr.Vt[j])]:
                print(s,end="")
        else:
            print("-",end = "")
        print("\t",end="")
    for j in range(len(lr.Vn)):
        if lr.goto[(i,lr.Vn[j])]:
            print(lr.goto[(i,lr.Vn[j])],end="")
        else:
            print("-",end = "")
        print("\t",end="")
    print("")

lr.analyze(tokenize(statements))

print(lr.top)

Code = genCode(lr.top)

print(Code)

if output:
    output.write(Code)
