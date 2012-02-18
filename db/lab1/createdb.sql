
CREATE TYPE personal_doc AS ENUM {'паспорт', 'права', 'загранпаспорт'};

CREATE OR REPLACE FUNCTION is_document_num_correct(personal_doc, varchar) RETURNS BOOLEAN AS $$
	SELECT TRUE 
		WHERE 
			($1='паспорт' )	      AND ($2 ~'^[0-9][0-9] [0-9][0-9] [0-9][0-9][0-9][0-9][0-9][0-9]$' ) OR
			($1='права' )         AND ($2 ~'^[0-9][0-9] [0-9][0-9] [0-9][0-9][0-9][0-9][0-9][0-9]$' ) OR
			($1='загранпаспорт' ) AND ($2 ~'^[0-9][0-9] [0-9][0-9][0-9][0-9][0-9][0-9][0-9]$' );
$$ LANGUAGE SQL;

CREATE TABLE cars  -- Создать таблицу 
(
	id INT NOT NULL, 			-- 
	num VARCHAR (6) NOT NULL CHECK ~'^[АВЕКМНОРСТУХ][0-9][0-9][0-9][АВЕКМНОРСТУХ][АВЕКМНОРСТУХ]$',  	-- номер авто
	model VARCHAR(30),
	color VARCHAR(50) );

CREATE TABLE clients -- Создать таблицу "Клиенты"
(
	id INT PRIMARY KEY,  		--
	surname VARCHAR (100) NOT NULL,
	name VARCHAR (100) NOT NULL,
	father_name VARCHAR (100) NOT NULL,
	doc_type  personal_doc,
	doc_num VARCHAR(12) CHECK is_document_num_correct(personal_doc, doc_num));

CREATE TABLE queries  -- Создать таблицу "Выплаты"
(
	QueryID INT NOT NULL,
	TaxistID INT NOT NULL,
	ClientID INT NOT NULL,
	QueryAdressFrom	VARCHAR (100) NOT NULL,	-- ?
	QueryAdressTo	VARCHAR (100) NOT NULL,	-- ?
	QueryDateTime DATE NOT NULL,
	QueryPay MONEY NOT NULL );
