USE TaxiCompany;  
GO 

-- ���������� ����������.
DECLARE @hDoc INT
-- C������������� ������ ������ XML.
EXEC sp_xml_preparedocument @hDoc OUTPUT,
N'<ROOT>
<tblClients ClientID="10000" ClientName="������� �������� ������" ClientPhone="(123)456-78-90"/>
<tblClients ClientID="10001" ClientName="�������� �������� �������" ClientPhone="(123)456-78-91"/>
</ROOT>'
-- �������� SELECT � �������� OPENXML.
SELECT *
FROM OPENXML(@hDoc, N'/ROOT/tblClients')
WITH ( 
	ClientID INT,
	ClientName VARCHAR (100),
	ClientPhone CHAR(15) )

INSERT INTO tblClients
	(ClientID, ClientName, ClientPhone)
SELECT * FROM OPENXML(@hDoc, N'/ROOT/tblClients')
WITH ( 
	ClientID INT,
	ClientName VARCHAR (100),
	ClientPhone CHAR(15) )

SELECT * FROM tblClients
WHERE tblClients.ClientID>9999;

DELETE FROM tblClients
WHERE tblClients.ClientID>9999;

-- �������� ����������� ������������� ��������� XML.
EXEC  sp_xml_removedocument @hDoc
GO
