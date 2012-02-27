-- 1. Инструкция SELECT, использующая предикат сравнения.
-- клиент захотел две одинаковых машины, но одну фиолетовую. а вторую - любую
SELECT C1.model, C1.num, C1.color, C2.num, C2.color
FROM Cars C1 INNER JOIN Cars C2 USING (model)
WHERE C1.id<>C2.id AND C1.color = 'Фиолетовый'
ORDER BY C1.model, C1.color, C2.color ;

-- 2. Инструкция SELECT, использующая предикат BETWEEN.
-- Получить список клиентов, сделавших заказы между...
SELECT clientID, startdate
FROM borrows
WHERE startdate BETWEEN '2009-01-01' AND '2009-01-31' 
ORDER BY startdate ;

-- 3. Инструкция SELECT, использующая предикат LIKE.
-- Менеджер захотел поржать над фамилиями клиентов начинаюющимися на "ху"
SELECT DISTINCT surname
FROM clients
WHERE surname ILIKE 'ху%' ;

-- 4. Инструкция SELECT, использующая предикат IN с вложенным подзапросом.
-- Менеджер захотел посмотреть, что заказывали люди с фамилией на "ху"
SELECT DISTINCT cr.model, b.payment
FROM borrows b INNER JOIN cars cr ON cr.id=b.carID
WHERE clientID IN (
	SELECT id
	FROM clients
	WHERE surname ILIKE 'ху%' )

-- 5. Инструкция SELECT, использующая предикат EXISTS с вложенным подзапросом.
-- Получить список машин, которые никто никогда не заказывал
SELECT id, num
FROM cars cro
WHERE EXISTS (
	SELECT cri.id
    FROM cars cri LEFT OUTER JOIN borrows b
		ON cro.id = b.carID
	WHERE b.carID IS NULL );
-- можно построить нормально


-- 6. Инструкция SELECT, использующая предикат сравнения с квантором.
-- за какие машины платили больше, чем за какие-либо до 2002-го
SELECT  b.payment, cr.model, cr.color
FROM borrows b JOIN cars cr ON cr.id=b.carID
WHERE payment>ALL 
	( SELECT payment 
	FROM borrows 
	WHERE startdate<'2002-10-10' );

-- 7. Инструкция SELECT, использующая агрегатные функции в выражениях столбцов.
SELECT 	cast(AVG(cast(totalPayed as numeric(32,16))) AS NUMERIC(18,2)) AS "Actual AVG", 
		SUM(totalPayed) / COUNT(ID) AS "Calc AVG"
FROM 
	( SELECT clients.ID, SUM(payment) AS totalPayed
	FROM borrows INNER JOIN clients on clients.id=clientID
	GROUP BY clients.ID ) as q;

-- SELECT 	AVG(cast(totalPayed as numeric)) AS "Actual AVG", 
--		SUM(totalPayed) / COUNT(ID) AS "Calc AVG"
--	FROM (
--		SELECT clients.ID, SUM(payment) AS totalPayed
--			FROM borrows RIGHT OUTER JOIN clients on clients.id=clientID
--			GROUP BY clients.ID ) as q;

-- 8. Инструкция SELECT, использующая скалярные подзапросы в выражениях столбцов.
-- тоже самое, что в 6, но с картами и девочками
SELECT  b.payment, cr.model, cr.color, 
	( SELECT AVG( cast(payment AS NUMERIC(32,10)) / 
				  ( CASE enddate-startdate when 0 THEN 1 ELSE enddate-startdate END ) ) 
	  FROM borrows 
	  WHERE carID IN 
			( SELECT id 
			  FROM cars
			  WHERE cr.model=cars.model	) ) * (b.enddate-b.startdate) 
		AS "Avg price for this period for this model"
FROM borrows b 
	JOIN cars cr ON cr.id=b.carID 
WHERE payment > (
	SELECT max(payment)
	FROM borrows 
	WHERE startdate<'2002-10-10' );

-- 9. Инструкция SELECT, использующая простое выражение CASE.
SELECT id, NAME, surname, 
	CASE (docType)
		WHEN 'паспорт' THEN 'pasport' 
		WHEN 'загранпаспорт' THEN 'international pasport'
		WHEN 'права' THEN 'driver license'
		ELSE 'unknown'
	END AS "Document type"
FROM Clients 
LIMIT 10;

-- 10. Инструкция SELECT, использующая поисковое выражение CASE.
-- ???
-- см. 12


-- 11. Создание новой временной локальной таблицы из результирующего набора данных инструкции SELECT. 

CREATE TEMP TABLE Funny_names AS
	(SELECT DISTINCT surname
	FROM clients
	WHERE surname ILIKE 'ху%') 
WITH DATA;

 
-- 12. Инструкция SELECT, использующая вложенные коррелированные подзапросы в качестве производных таблиц в предложении FROM.
SELECT 'most expensive per day' AS Criteria, cars.model, "Cost Per Day" 
FROM cars INNER JOIN
	( SELECT cars.model, 
	  	cast(AVG( cast(payment AS NUMERIC(32,16)) / 
				( CASE enddate-startdate 
				  WHEN 0 THEN 1 
				  ELSE (enddate-startdate) 
				  END) ) AS NUMERIC(16,2) ) AS "Cost Per Day" 
	  FROM borrows INNER JOIN cars ON carID=cars.id
	  GROUP BY cars.model
	  ORDER BY "Cost Per Day" DESC
	  LIMIT 2) AS car_model_avg_price ON car_model_avg_price.model=cars.model
UNION
SELECT 'most expensive borrow' AS Criteria, cars.model, "Cost Per Day"
FROM cars INNER JOIN
	( SELECT cars.model, 
	  	cast(max(payment/ 
				( CASE enddate-startdate 
				  WHEN 0 THEN 1 
				  ELSE (enddate-startdate) 
				  END) ) AS NUMERIC (16,2) ) AS "Cost Per Day" 
	  FROM borrows INNER JOIN cars ON carID=cars.id
	  GROUP BY cars.model
	  ORDER BY "Cost Per Day" DESC
	  LIMIT 2) AS car_model_max_borrow_price ON car_model_max_borrow_price.model=cars.model;

-- 13. Инструкция SELECT, использующая вложенные подзапросы с уровнем вложенности 3.
SELECT *
FROM ( SELECT  b.payment, cr.model, cr.color, 
		( SELECT AVG( cast(payment AS NUMERIC(32,10))/(enddate-startdate) ) 
		  FROM borrows 
		  WHERE carID IN 
				( SELECT id 
				  FROM cars
				  WHERE cr.model=cars.model	) ) * (b.enddate-b.startdate) 
			AS "Avg price for this period for this model"
	FROM borrows b 
		JOIN cars cr ON cr.id=b.carID 
	WHERE payment > (
		SELECT max(payment)
		FROM borrows 
		WHERE startdate<'2002-10-10') 
	) AS expensive_cars
	WHERE color ILIKE '%синий%';
	
-- 14. Инструкция SELECT, консолидирующая данные с помощью предложения GROUP BY, но без предложения HAVING.
SELECT cars.model, 
	  	cast(max(payment/ 
				( CASE enddate-startdate 
				  WHEN 0 THEN 1 
				  ELSE (enddate-startdate) 
				  END) ) AS NUMERIC (16,2) ) AS "Cost Per Day" 
	  FROM borrows INNER JOIN cars ON carID=cars.id
	  GROUP BY cars.model
	  ORDER BY "Cost Per Day" DESC
	  LIMIT 20

-- 15. Инструкция SELECT, консолидирующая данные с помощью предложения GROUP BY и  предложения HAVING.
SELECT cars.model, 
	  	AVG( cast( cars.monthPrice AS NUMERIC (16,2) ) ) AS "Avg model cost per month" 
	FROM cars
	GROUP BY cars.model
	HAVING AVG( cast( cars.monthPrice  AS NUMERIC (16,2) ) ) >
		( SELECT AVG(cast( monthPrice AS NUMERIC(16,2)))
		  FROM cars )
	ORDER BY "Avg model cost per month" DESC;


-- 16. Однострочная инструкция INSERT, выполняющая вставку в таблицу одной строки значений.
INSERT INTO Clients ( surname, name, fatherName, docType, docNum)
VALUES ( 'Иванов', 'Иван', 'Иванович', 'паспорт', '12 34 123456');

-- 17. Многострочная инструкция INSERT, выполняющая вставку в таблицу результирующего набора данных вложенного подзапроса.

INSERT INTO borrows

18. Простая инструкция UPDATE.

UPDATE Products
SET UnitPrice = UnitPrice * 1.5
WHERE ProductID = 35

19. Инструкция UPDATE со скалярным подзапросом в предложении SET.

UPDATE Products
SET UnitPrice = 	
(
SELECT AVG(UnitPrice)
		FROM [Order Details]
		WHERE ProductID = 37
	)
WHERE ProductID = 37
20. Простая инструкция DELETE.

DELETE Orders
WHERE CustomerID IS NULL
21. Инструкция DELETE с вложенным коррелированным подзапросом в предложении WHERE.

-- Пример для базы данных AdventureWorks
DELETE FROM Production.Product
WHERE ProductID IN
(
     		SELECT Product.ProductID
     		FROM Production.Product LEFT OUTER JOIN Sales.SalesOrderDetail
        		ON Product.ProductID = SalesOrderDetail.ProductID
     		WHERE SalesOrderDetail.ProductID IS NULL
     			AND Product.ProductSubCategoryID = 5
  	)
22. Инструкция SELECT, использующая простое общее табличное выражение.

-- Пример для базы данных SPJ
WITH CTE (SupplierNo, NumberOfShips)
AS
(
    SELECT Sno, COUNT(*) AS Total
    FROM SPJ
    WHERE Sno IS NOT NULL
    GROUP BY Sno
)
SELECT AVG(NumberOfShips) AS 'Среднее количество поставок для поставщиков'
FROM CTE
23. Инструкция SELECT, использующая рекурсивное общее табличное выражение.
-- Создание таблицы.
CREATE TABLE dbo.MyEmployees
(
	EmployeeID smallint NOT NULL,
	FirstName nvarchar(30)  NOT NULL,
	LastName  nvarchar(40) NOT NULL,
	Title nvarchar(50) NOT NULL,
	DeptID smallint NOT NULL,
	ManagerID int NULL,
 	CONSTRAINT PK_EmployeeID PRIMARY KEY CLUSTERED (EmployeeID ASC) 
) ;
GO
-- Заполнение таблицы значениями.
INSERT INTO dbo.MyEmployees VALUES (1, N'Иван', N'Петров', N'Главный исполнительный директор',16,NULL) ;
…
GO
-- Определение ОТВ
WITH DirectReports (ManagerID, EmployeeID, Title, DeptID, Level)
AS
(
-- Определение закрепленного элемента
    SELECT e.ManagerID, e.EmployeeID, e.Title, e.DeptID, 0 AS Level
    FROM dbo.MyEmployees AS e
    WHERE ManagerID IS NULL
    UNION ALL
-- Определение рекурсивного элемента
    SELECT e.ManagerID, e.EmployeeID, e.Title, e.DeptID, Level + 1
    FROM dbo.MyEmployees AS e INNER JOIN DirectReports AS d ON e.ManagerID = d.EmployeeID
)
-- Инструкция, использующая ОТВ
SELECT ManagerID, EmployeeID, Title, DeptID, Level
FROM DirectReports ;
