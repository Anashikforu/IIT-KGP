#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Feb 13 03:23:21 2022

@author: ashikkhan
"""

import requests
import re
import ply.lex as lex
import ply.yacc as yacc

# GLOBAL VARIABLES

countrylist = []
continentlist = []

country_info = {}
continent_info = {}
world_info = {}



# lexical analyzer
tokens = (
            'LINK',
            'ANCHOR',
            'TD_OPEN',
            'TD_CLOSE',
            'TD_NULL',
            'TR_WORLD',
            'TR_CONTINENT',
            'NOBR_CLOSE',
            'NOBR',
            'ALL'
          )


# TOKEN DEFINITION
t_TD_CLOSE                  =   r'''</td>+'''
t_TD_OPEN                   =   r'''<td\sstyle="[\d\w() -.'\"()\s\:;=]+">+'''
t_TR_WORLD                  =   r'''<tr\sclass="total_row_world">'''
t_TR_CONTINENT              =   r'''<tr\sclass="total_row_world\srow_continent"\sdata-continent="[\w\s\d /:-]+"\sstyle="[\w\s\d:]+">'''
t_TD_NULL                   =   r'''<td>+'''
t_NOBR                      =   r'''<nobr>'''
t_NOBR_CLOSE                =   r'''</nobr>'''
t_LINK						= 	r'''<a\sclass="[\d\w()\-\]"\shref="country/[\d\w()\-\ _ "=]+>'''
t_ANCHOR					=	r'''<\/a>'''
t_ALL						= 	r'''[\d\w() -.'\/\"()\s\$]+'''
t_ignore_COMMENT            =   r'\#.*'
t_ignore                    =   ' \t'    # ignores spaces and tabs

def t_error (t):
#   print("Illegal character '%s'" % t.value[0])
    t.lexer.skip(1)

def p_start(p):
    '''Start : S1
            | S2
            | S3
            | S4
            | S5
            | S6
            | S7
            | S8
            | S9
            '''

def p_country_data(p):
    'S1 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), p[10].strip(), p[13].strip(), p[16].strip(), p[19].strip(), p[22].strip(),p[25].strip(), p[28].strip(), p[31].strip(), p[34].strip(), p[37].strip(), p[40].strip())

def p_new_death_less_country_data(p):
    'S2 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), p[10].strip(), p[13].strip(), 0, p[18].strip(),p[21].strip(), p[24].strip(), p[27].strip(), p[30].strip(), p[33].strip(), p[36].strip(),p[39].strip())

def p_new_recover_less_country_data(p):
    'S3 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), p[10].strip(), p[13].strip(), 0, p[18].strip(), 0,p[23].strip(), p[26].strip(), p[29].strip(), p[32].strip(), p[35].strip(), p[38].strip())

def p_new_case_less_country_data(p):
    'S4 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), 0, p[12].strip(), 0, p[17].strip(), 0, p[22].strip(),p[25].strip(), p[28].strip(), p[31].strip(), p[34].strip(), p[37].strip())

def p_critical_case_less_country_data(p):
    'S5 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), 0, p[12].strip(), 0, p[17].strip(), 0, p[22].strip(), 0,p[27].strip(), p[30].strip(), p[33].strip(), p[36].strip())

def p_new_death_critical_case_less_country_data(p):
    'S6 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), p[10].strip(), p[13].strip(), 0, p[18].strip(), p[21].strip(), p[24].strip(), 0,p[29].strip(), p[32].strip(), p[35].strip(), p[38].strip())

def p_only_critical_case_less_country_data(p):
    'S7 : TD_OPEN LINK ALL ANCHOR TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_OPEN ALL TD_CLOSE'
    store_country_data(p[3].strip(), p[7].strip(), p[10].strip(), p[13].strip(), p[16].strip(), p[19].strip(), p[22].strip(), p[25].strip(), 0,p[30].strip(), p[33].strip(), p[36].strip(), p[39].strip())

def p_world_data(p):
    'S8 : TR_WORLD TD_NULL TD_CLOSE TD_OPEN ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE'
    world_info['total_cases'] = p[8]
    world_info['new_cases'] = p[11]
    world_info['total_death'] = p[14]
    world_info['new_death'] = p[17]
    world_info['total_recovered'] = p[20]
    world_info['new_recovered'] = p[23]
    world_info['active_cases'] = p[26]
    world_info['critical'] = p[29]
    world_info['cases_million'] = p[32]
    world_info['death_million'] = p[35]
    # print(world_info)

def p_continent_data(p):
    'S9 : TR_CONTINENT TD_NULL TD_CLOSE TD_OPEN NOBR ALL NOBR_CLOSE TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE TD_NULL ALL TD_CLOSE'
    continent_info['continent_name'] = p[6]
    continent_info['total_cases'] = p[10]
    continent_info['new_cases'] = p[13]
    continent_info['total_death'] = p[16]
    continent_info['new_death'] = p[19]
    continent_info['total_recovered'] = p[22]
    continent_info['new_recovered'] = p[25]
    continent_info['active_cases'] = p[28]
    continent_info['critical'] = p[30]
    # print(continent_info)

def store_country_data(country_name,total_cases,new_cases,total_death,new_death,total_recovered,new_recovered,active_cases,critical,cases_million,death_million,total_test,test_million):
    country_info[country_name] = {}
    country_info[country_name]['country_name'] = country_name
    country_info[country_name]['total_cases'] = total_cases
    country_info[country_name]['new_cases'] = new_cases
    country_info[country_name]['total_death'] = total_death
    country_info[country_name]['new_death'] = new_death
    country_info[country_name]['total_recovered'] = total_recovered
    country_info[country_name]['new_recovered'] = new_recovered
    country_info[country_name]['active_cases'] = active_cases
    country_info[country_name]['critical'] = critical
    country_info[country_name]['cases_million'] = cases_million
    country_info[country_name]['death_million'] = death_million
    country_info[country_name]['total_test'] = total_test
    country_info[country_name]['test_million'] = test_million
    country_info[country_name]['test_million'] = test_million
    # print(country_info[country_name], len(country_info))

def p_error(p):
    pass

def getYesterdayData(data):
    delim = '<table id="main_table_countries_yesterday" class="table table-bordered table-hover main_table_countries" style="width:100%;margin-top: 0px !important;display:none;">'
    data = data.partition(delim)[2]
    delim_before = '<table id="main_table_countries_yesterday2" class="table table-bordered table-hover main_table_countries" style="width:100%;margin-top: 0px !important;display:none;">'
    data = data.split(delim_before)[0]
    return data

def read_html():
    url = "https://www.worldometers.info/coronavirus/"
    data = get_page_content(url)
    data = getYesterdayData(data)
    # print(data)
    lexer = lex.lex()
    parser = yacc.yacc()
    parser.parse(data)
    print("Parsing Done!")

def get_page_content(url):
    r = requests.get(url)
    html_text = r.text
    html_text = re.sub("\s+", " ", html_text)

    return html_text

#---------------FIle Wise----------------------------------------
def extract_countrylist():
    inputFileName = "worldometers_countrylist.txt"
    with open(inputFileName) as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    stand = 0

    for country in content:
        if country.endswith(":"):
            country.strip()
            country = country[:-1]
            continentlist.append(country)
        elif not country:
            stand = 1
        elif country.endswith("-"):
            stand = 0
        elif stand == 1:
            country.strip()
            continentlist.append(country)
        else:
            country.strip()
            countrylist.append(country)

#-----------------------------------------------------

def checkOptionNo(element,lenth):
    try:
        if(int(element) and int(element) <= lenth):
            return True
        else:
            return False
    except ValueError:
        return False

def subOptionMenu(menu):
    print("                                                ")
    print("  {} COVID-19 Coronavirus Pandemic Information  ".format(menu))
    print("                                                ")
    print("------------------------------------------------")
    print(":                                              :")

def optionMenu(optionList,menu):
    print("                           ")
    print("       {} List        ".format(menu))
    print("                           ")
    print(" ---------------------------")
    for (i, option) in enumerate(optionList, start=1):
        print(":   {}. {}".format(i, option))
    print(" ---------------------------")

    while(1):
        option_no = input("\nEnter {} name/no (Enter (back) for going back):\n".format(menu))

        if (option_no == "back"):
            break
        elif (checkOptionNo(option_no, len(optionList))):
            country_name = optionList[int(option_no) - 1]
            # print(country_name)

        elif (option_no in optionList):
            country_name = option_no
            # print(country_name)

        else:
            print("Invalid option.\n")

def main():

    url = "https://www.worldometers.info/coronavirus/"
    page = "coronavirus"
    content = get_page_content(url)

    with open(page + ".html", 'w') as input_file:
        input_file.write(content)
        input_file.close()

    extract_countrylist()
    read_html()

    i = 0
    for country in  countrylist :
        if(country not in country_info):
            print(country)
            i=i+1
            print(i)

    while (1):
        print("                                             ")
        print("  COVID-19 Coronavirus Pandemic Information  ")
        print("                                             ")
        print(" --------------------------------------------")
        print(" :        1. Country                        :")
        print(" :        2. Continent                      :")
        print(" :        3. World                          :")
        info_type = input("Enter option name/no (Enter (exit) for the exit):\n")
        info_type = info_type.strip()

        if(info_type == "Country" or info_type == "country" or info_type == '1'):
            info_type = "Country"
            optionMenu(countrylist,"Country")

        elif(info_type == "Continent" or info_type == "continent" or info_type == '2'):
            info_type = "Continent"
            optionMenu(continentlist,"Continent")

        elif (info_type == "World" or info_type == "world" or info_type == '3'):
            info_type = "World"
            print(info_type)
        elif(info_type == "exit"):
            break
        else:
            print("Invalid option.\n")

if __name__=="__main__":
    main()


