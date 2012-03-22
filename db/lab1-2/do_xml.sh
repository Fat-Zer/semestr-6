#!/bin/sh
XML_DIR="/tmp/lab1-2_xml"
CSS_DIR="$XML_DIR/css"
XML1N="1.xml"
XML2N="2.xml"
XML3N="3.xml"
XML4N="4.xml"
XML1="$XML_DIR/$XML1N"
XML2="$XML_DIR/$XML2N"
XML3="$XML_DIR/$XML3N"
XML4="$XML_DIR/$XML4N"
XML_HEADER='<?xml version="1.1"?>'
mkdir -p $(dirname "$XML1");


echo "$XML_HEADER" > "$XML1"
psql -Atf - >>"$XML1" <<EOF
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
sed -e 's/></>\n</g' -i "$XML1"

echo "$XML_HEADER" > "$XML2"
psql -Atf - >>"$XML2" <<EOF
SELECT XMLSERIALIZE( CONTENT xmlelement(NAME root,xmlagg(xml_cli)) AS TEXT)
FROM ( SELECT xmlelement(NAME clients, 
			xmlelement(NAME surname, c.surname), 
			xmlelement(NAME name, c.name),
			xmlelement(NAME fathername, c.fathername), 
			bx.xml_bor) AS xml_cli
		FROM clients c 
			JOIN ( SELECT b.clientid AS id, xmlagg(xmlelement(NAME borrows,
					xmlelement(NAME startDate, b.startDate),
					xmlelement(NAME payment, CAST(b.payment AS NUMERIC(18,2))) )) AS xml_bor
				FROM borrows b
				GROUP BY b.clientID ) AS bx USING (id)
		LIMIT 20) AS cx;
EOF
sed -e 's/></>\n</g' -i "$XML2"

# echo "$XML_HEADER" > "$XML2"
# psql -Atf - >>"$XML2" <<EOF
# SELECT query_to_xml( 
# 		'SELECT c.surname, c.name, c.fathername, b.startDate, 
# 			CAST(b.payment AS NUMERIC(18,2)) AS payment
# 		FROM clients c NATURAL JOIN borrows b 
# 		LIMIT 10;',
# 		FALSE, FALSE, '');
# EOF

echo "$XML_HEADER" > "$XML3"
echo "SELECT xmlelement(NAME root, table_to_xml('cars', FALSE, TRUE, ''));" | psql -Atf - >>"$XML3" 

############################################################
#               CSS                                        #
############################################################
mkdir -p "$CSS_DIR"

for I in {1..3}; do
	CSS="$I.css"
	CSS_STR='<?xml-stylesheet href="'"$CSS"'" type="text/css"?>'
	sed -e "1a$CSS_STR" "$XML2" >"$CSS_DIR/$I.xml"
done

cat >"$CSS_DIR/1.css" << EOF
root {	
	display: table;
	border: 2px double black; 
	border-collapse: collapse;
	padding: 5px;
}

clients {
	display: table-row;
}

name, surname, fathername {
	display: table-cell;
	border: 1px solid black;
	padding: 2px;
}

borrows {
	display:table-row-group;
	border-lef: 1px solid black;
	width=100%;
}

EOF


cat >"$CSS_DIR/2.css" << EOF
clients {
	display: block;
	border: solid;
	border-width: 10px;
	border-color:white;
	width: 545pt;
}

name, surname, fathername {
	display: inline;
	width:20%;
	border:double; 
	border-width:2px; 
	border-color:#CCCCCC;
	width: 526pt;
	text-align: center;
	font-family: Comic Sans Ms, Arial, Helvetica;
	font-size: 18pt;
	color: #F0F01F;
	background-color:#013101
}

borrows {
	display: block;
}

startdate {
	font-family: Arial, Helvetica;
	font-size: 14pt;
	color: #10D010;
	display: inline;
	border:solid; 
	border-width:0px; 
	border-color:#CCCCCC;
	width: 340pt;
}

payment {
	font-family: Arial, Helvetica;
	font-size: 14pt;
	color: #10D010;
	display: inline;
	border:solid; 
	border-width:0px; 
	border-color:#CCCCCC;
	width: 100pt
}
EOF

# sed -e '2s/+$//;s/ *+$//' -i "$XML2"

# echo "$XML_HEADER" > "$XML1"
# psql -Atf - >>"$XML1" <<EOF
# 
# EOF
# sed -e 's/></>\n</g' -i "$XML1"
