#!/bin/sh
XML_DIR="/tmp/lab1-2_xml/"
FIRST_XML="$XML_DIR/1.xml"

mkdir -p $(dirname "$FIRST_XML");
echo '<?xml version="1.1"?>' > "$FIRST_XML"
psql -tf - >>"$FIRST_XML" <<EOF
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
EOF
sed -e 's/></>\n</g' -i "$FIRST_XML"
