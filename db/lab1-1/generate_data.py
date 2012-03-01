#!/usr/bin/env python
# -*-  coding: utf-8 -*- 
import random
import datetime
import sys

def unsplit(lst, sep='\t'):
	rv=u""
	for el in lst[:-1]:
		rv+=el
		rv+=sep
	rv+=lst[-1]
	return rv


def loadStrings(file_name):
	f=open(file_name)
	str_list= [istr.strip().decode("utf-8") for istr in f.readlines()]
	f.close()
	return str_list

def putStrings(file_name, lst):
	f=open(file_name, mode='w')
	for tup in lst:
		f.write((unsplit(tup) + '\n').encode('utf-8'))
	f.close()

def constructStreat(istr):
	l=istr.split('\t')
	if (l[1]=='append'):
		return ' '.join([l[2], l[0]])
	else:
	 	return ' '.join([l[0], l[2]])

def generateCarNumber():
	number_chars=u'АВЕКМНОРСТУХ'
	return random.choice(number_chars) + str(random.randint(0,9)) + \
			str(random.randint(0,9)) + str(random.randint(0,9)) + \
			random.choice(number_chars) + random.choice(number_chars);

def generateDocumentNumber(doctype):
	if(doctype==u'паспорт'):
		return str(random.randint(0,9)) + str(random.randint(0,9)) + ' ' + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + ' ' + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9)) + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9))
	elif(doctype==u'права'):
		return str(random.randint(0,9)) + str(random.randint(0,9)) + ' ' + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + ' ' + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9)) + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9))

	elif(doctype==u'загранпаспорт'):
		return str(random.randint(0,9)) + str(random.randint(0,9)) + ' ' + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9)) + \
				str(random.randint(0,9)) + str(random.randint(0,9)) + str(random.randint(0,9)) + \
				str(random.randint(0,9))
	else:
		return ""

def generateDate(mind = datetime.date.today() - 10 * datetime.timedelta(days=365), \
		dtime =  datetime.timedelta(days=365) * 12):
	return mind + datetime.timedelta(days=random.randint(0,dtime.days))

def isPeriodsIntersects(per1, per2):
	return per1[0] < per2[0] < per1[1] or per1[0] < per2[1] < per1[1] or per2[0] < per1[0] < per2[1]

# constants
gender_woman=0;
gender_man=1;
generate_cars_n = 10000
generate_clients_n = 10000
generate_borrows_n = 10000
min_balance = -100
max_balance = 10000
min_car_monthPrice = 1
max_car_monthPrice = 100
discount_widths = [0]*400 + [10]*100 + [20]*25 + [30]*13 + [50]*7 + [60]*4;
# load data
print "loading data..."
car_models = loadStrings('./data/car')
color =  loadStrings('./data/colors')
man_names = loadStrings('./data/man_names')
woman_names = loadStrings('./data/woman_names')
familys = [  tuple(istr.strip().split('\t')) for istr in loadStrings('./data/familys') ]
father_names = [ tuple(istr.strip().split('\t')) for istr in loadStrings('./data/father_names') ]
streats = [ constructStreat(istr) for istr in loadStrings('./data/streats') ]
documents = [ u'паспорт', u'права', u'загранпаспорт' ]
# print sys.stdout.encoding
"""

print man_names[len(man_names) - 1]
print woman_names[len(woman_names) - 1]
print family[len(family) - 1][gender_man], family[len(family) - 1][gender_woman]
print car_model[len(car_model) - 1]
print father_name[len(father_name) - 1][0], father_name[len(father_name) - 1][1]
print streat[len(streat) - 1]
"""


# generate cars list
print "generating cars..."
cars=[]
cars_number_set=set()
# first we generate a car_dict to assure that car nember is uniq.
for i in xrange(0, generate_cars_n ):
	num = generateCarNumber()
	while(num in cars_number_set):
		num = generateCarNumber()
	cars_number_set.add(num)
	monthPrice=random.randint(min_car_monthPrice, max_car_monthPrice)
	cars.append((str(i), num, random.choice(car_models), random.choice(color), str(monthPrice)))
cars_number_set=set()
   
# generate client list
print "generating clients..."
clients=[]
document_set=set()
for i in xrange(1,generate_clients_n):
	document = random.choice(documents)
	number = generateDocumentNumber(document)
	while ((document, number) in document_set):
		document = random.choice(documents)
		number = generateDocumentNumber(document)
	document_set.add((document, number))
	if( random.choice([gender_man, gender_woman]) == gender_man):
		name = random.choice(man_names)
		family = random.choice(familys)[gender_man]
		father_name = random.choice(father_names)[gender_man]
	else:
		name = random.choice(woman_names)
		family = random.choice(familys)[gender_woman]
		father_name = random.choice(father_names)[gender_woman]
	balance=random.randint(min_balance, max_balance)
	if( random.randint(0,100)==0 ):
		referal = str(random.randint(0, i-1))
	else:
		referal = '\N'
	clients.append((str(i), family, name, father_name, document, number, str(balance), referal))
	
document_set=set()
	
# generate orders
print "generating lists..."
borrows=[]
car_reserve_dict={}
for i in xrange(1,generate_borrows_n):
	borrow_start = generateDate()
	borrow_dates = ( borrow_start, generateDate(mind=borrow_start, dtime=datetime.timedelta(days=365)) )
	car = random.choice(cars)[0]
	if(car in car_reserve_dict):
		reserved = True;
		while(reserved):
			reserved = False;
			for res_dates in car_reserve_dict[car]:
				if(isPeriodsIntersects(borrow_dates,res_dates)):
					borrow_start = generateDate()
					borrow_dates = ( borrow_start, generateDate(mind=borrow_start, \
									dtime=datetime.timedelta(days=365)) )
					break;
		car_reserve_dict[car].append(borrow_dates)
	else:
		car_reserve_dict[car]=[(borrow_dates)]

	if (borrow_dates[0] > datetime.date.today()):
		payed = random.choice([True,False])
	else:
		payed=True
	
	if (random.randint(0,1000) == 0):
		payment = '\\N'
		payed = True
	else:
	 	payment='{:.2f}'.format(float((cars[int(car)])[4]) * \
				(1-float(random.choice(discount_widths))/100) * \
				(borrow_dates[1] - borrow_dates[0]).days/30.);
	
	borrows.append((str(i), car, random.choice(clients)[0], str(borrow_dates[0]), str(borrow_dates[1]), payment, str(payed)))


print "writing output..."

putStrings('/tmp/cars',cars)
putStrings('/tmp/clients', clients)
putStrings('/tmp/borrows', borrows)

print "Generation done."
