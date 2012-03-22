
select query_to_xml( 'select * from clients LIMIT 10' , TRUE, FALSE, '');
select query_to_xml( 'select * from clients LIMIT 10' , TRUE, TRUE, '');


select table_to_xml_and_xmlschema( 'clients', FALSE, FALSE, '');
select table_to_xml_and_xmlschema( 'clients', FALSE, TRUE, '');

SELECT XMLSERIALIZE( CONTENT xmlelement(NAME root,xmlagg(xml_cli)) AS TEXT)
FROM ( SELECT xmlelement(NAME clients, 
			xmlattributes(c.surname, c.name, c.fathername), 
			bx.xml_bor) AS xml_cli
		FROM clients c 
			JOIN ( SELECT b.clientid AS id, xmlagg(xmlelement(NAME borrow,
					xmlattributes(b.startDate, CAST(b.payment AS NUMERIC(18,2)) AS payment ))) AS xml_bor
				FROM borrows b
				GROUP BY b.clientID ) AS bx USING (id)
		LIMIT 10) AS cx;

SELECT query_to_xml_and_xmlschema( 
		$$SELECT c.surname, c.name, c.fathername, b.startDate, 
			CAST(b.payment AS NUMERIC(18,2)) AS payment
		FROM clients c NATURAL JOIN borrows b 
		LIMIT 10;$$,
		TRUE, FALSE, '');

SELECT table_to_xml('cars', FALSE, TRUE, '');
