
CREATE TYPE TPersonalDoc AS ENUM ('паспорт', 'права', 'загранпаспорт');

CREATE OR REPLACE FUNCTION is_document_num_correct(TPersonalDoc, varchar) RETURNS BOOLEAN AS $$
SELECT 
($1='паспорт' )	      AND ($2 ~'^[0-9]{2} [0-9]{2} [0-9]{6}$' ) OR
($1='права' )         AND ($2 ~'^[0-9]{2} [0-9]{2} [0-9]{6}$' ) OR
($1='загранпаспорт' ) AND ($2 ~'^[0-9]{2} [0-9]{7}$' );
$$ LANGUAGE SQL;


CREATE TABLE cars  -- Создать таблицу 
(
	id SERIAL PRIMARY KEY, 			-- 
	num VARCHAR (6) 
		NOT NULL CHECK (num~'^[АВЕКМНОРСТУХ][0-9]{3}[АВЕКМНОРСТУХ]{2}$'),  	-- номер авто
	model VARCHAR(30),
	color VARCHAR(50),
	monthPrice money CHECK (monthPrice>=MONEY(0)));

CREATE TABLE clients -- Создать таблицу "Клиенты"
(
	id SERIAL PRIMARY KEY,  		--
	surname VARCHAR (100) NOT NULL,
	name VARCHAR (100) NOT NULL,
	fatherName VARCHAR (100) NOT NULL,
	docType  TPersonalDoc,
	docNum VARCHAR(12) CHECK (is_document_num_correct(docType, docNum)),
	balance MONEY NOT null);

CREATE TABLE borrows  -- Создать таблицу "Выплаты"
(
	id SERIAL PRIMARY KEY,
	carID INT REFERENCES cars,
	clientID INT REFERENCES clients,
 	startDate DATE NOT NULL,
 	endDate DATE NOT NULL CHECK (endDate >= startDate), 
	payment money CHECK (payment>=MONEY(0)),
	payedOff boolean DEFAULT False CHECK ((payment IS not NULL) or  (payedOff = TRUE)) );
