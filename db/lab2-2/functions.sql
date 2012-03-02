-- Разработать и тестировать 10 модулей
-- A. Четыре функции
-- 1) Скалярную функцию
CREATE OR REPLACE FUNCTION is_document_num_correct(TPersonalDoc, varchar) RETURNS BOOLEAN AS $$
SELECT 
($1='паспорт' )	      AND ($2 ~'^[0-9]{2} [0-9]{2} [0-9]{6}$' ) OR
($1='права' )         AND ($2 ~'^[0-9]{2} [0-9]{2} [0-9]{6}$' ) OR
($1='загранпаспорт' ) AND ($2 ~'^[0-9]{2} [0-9]{7}$' );
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION model_avgpay(varchar(30)) RETURNS MONEY AS $$
	SELECT cast(AVG( cast(payment AS NUMERIC(32,16)) /
				( CASE enddate-startdate 
				  WHEN 0 THEN 1 
				  ELSE (enddate-startdate) 
				  END)) AS MONEY ) AS "Cost Per Day" 
		FROM borrows INNER JOIN cars ON carID=cars.id
		WHERE cars.model=$1;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION fibonachi(num INT) RETURNS INTEGER AS $$
DECLARE
	f1 INTEGER:=0;
	f2 INTEGER:=1;
	i INTEGER:=1;
BEGIN
	WHILE i<num LOOP
		f1:=f2+f1;
		f2:=f2+f1;
		i:=i+2;
	END LOOP;
	IF num%2=0 THEN 
		RETURN f1;
	ELSE
		RETURN f2;
	END IF;
END
$$ LANGUAGE plpgsql;

-- 2) Подставляемую табличную функцию

CREATE OR REPLACE FUNCTION model_avgpay_tbl() RETURNS TABLE(model cars.model%TYPE, "avg payed" MONEY) AS $$
BEGIN
	RETURN QUERY  SELECT cars.model, 
	  	cast(AVG( cast(payment AS NUMERIC(32,16)) /
				( CASE enddate-startdate 
				  WHEN 0 THEN 1 
				  ELSE (enddate-startdate) 
				  END)) AS MONEY ) AS "Cost Per Day" 
	FROM borrows INNER JOIN cars ON carID=cars.id
	GROUP BY cars.model;
END
$$ LANGUAGE plpgsql;

-- 3) Многооператорную табличную функцию
--CREATE OR REPLACE FUNCTION () 
--RETURNS TABLE(name AS , surname AS , fathername AS ) 
--AS $$
--	
--$$

-- 4) Рекурсивную функцию или функцию с рекурсивным ОТВ
CREATE OR REPLACE FUNCTION allReferals(clientID Clients.ID%TYPE) 
RETURNS TABLE(model cars.model%TYPE, "avg payed" MONEY) AS $$
BEGIN
	RETURN QUERY WITH RECURSIVE AllReferalsCTE (id, surname, name, fathername, referalClient, level) AS 
	( -- Определение закрепленного элемента
	    SELECT clients.ID, name, fathername, surname, referalClient, 0 as level
	    FROM Clients
	    WHERE ID=ClientID
	    UNION ALL
	-- Определение рекурсивного элемента
	    SELECT c.ID, c.name, c.fathername, c.surname, c.referalClient, r.level+1
	    FROM Clients AS c INNER JOIN AllReferalsCTE AS r ON c.ID = r.referalClient )
	-- Инструкция, использующая ОТВ
	SELECT * FROM AllReferalsCTE;
END
$$ LANGUAGE plpgsql;

-- B. Четыре хранимых процедуры
-- 1) Хранимую процедуру без параметров
-- 2) Хранимую процедуру с входными и выходными параметрами
-- 3) Хранимую процедуру с курсором
-- 4) Рекурсивную хранимую процедуру или хранимую процедуру с рекурсивним ОТВ
-- C. Два триггера
-- 1) Триггер AFTER
-- 2) Триггер INSTEAD OF
