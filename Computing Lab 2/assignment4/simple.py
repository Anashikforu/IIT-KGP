
import requests
import re
import ply.lex as lex
import ply.yacc as yacc

# lexical analyzer
tokens = (
            'DIV_CONTENT',
            'DIV_OPEN',
            'DIV_CLOSE',
            'DATA'
         )

# TOKEN DEFINITION
t_DIV_CONTENT       = r'''<div\sclass=content>+'''
t_DIV_OPEN          = r'''<div\sclass=[\w\d\s-]+>+'''
t_DATA              = r'''[\w\d\s-]+'''
t_DIV_CLOSE         = r'''<\/div>'''

def t_error (t):
#   print("Illegal character '%s'" % t.value[0])
    t.lexer.skip(1)

def p_start(p):
    '''Start : S1'''

def p_get_Content(p):
    'S1 : DIV_OPEN DATA DIV_CLOSE '
    print(p[2])

def p_error(p):
    pass

def read_html():
    url = "https://www.geeksforgeeks.org/"
    r = requests.get(url)
    html_text = r.text
    html_text = re.sub("\s+", " ", html_text)

    lexer = lex.lex()
    parser = yacc.yacc()
    parser.parse(html_text)


def main():
    read_html()

if __name__=="__main__":
    main()