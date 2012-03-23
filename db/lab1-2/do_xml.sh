#!/bin/sh
XML_DIR="/tmp/lab1-2_xml"
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
CSS_DIR="$XML_DIR/css"
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
	padding-right:10px;
}

borrows {
	display:table-row-group;
	width=100%;
}

startdate, payment {
	display:table-cell;
	border-top: 1px solid black;
	padding: 2px;
	padding-left:10px;
	padding-right:10px;
}

payment {
	padding-left:10px;
	border-left: 1px solid black;
	text-align:right;
	width:100px;
}

payment:before {
	content:'$ ';
}
EOF


cat >"$CSS_DIR/2.css" << EOF
clients {
	display: block;
	border: solid;
	border-width: 10px;
	border-color:white;
	background:darkgrey
}

name, surname, fathername {
	display: inline;
	font-family: Comic Sans Ms, Arial, Helvetica;
	font-size: 18pt;
}

borrows {
	display: block;
	padding-left: 20px;
	background: grey;
}

startdate, payment {
	font-family: Arial, Helvetica;
	font-size: 14pt;
	color: lightgreen;
	display: inline;
}

startdate:after {
	content:':   ';
}

payment:before {
	content:'$';
}
EOF


cat >"$CSS_DIR/3.css" << EOF
clients {
	display: block;
	background:darkgrey
}

name, surname, fathername {
	display: inline;
	font-family: Helvetica;
	font-size: 24pt;
}

borrows {
	display: table-row;
	padding-left: 20px;
	background: grey;
	border-collapse: collapse;
}

startdate, payment {
	display: table-cell;
	font-family: Helvetica;
	font-size: 12pt;
	color: blue;
}

startdate {
	padding-right: 10px;
}

payment {
	color: red;
}

payment:before {
	content:'$ ';
	color: #AA1111
}
EOF

############################################################
#               XSL                                        #
############################################################

XSL_DIR="$XML_DIR/xsl"
mkdir -p "$XSL_DIR"

I=1;

XSL="$I.xsl"
XSL_STR='<?xml-stylesheet href="'"$XSL"'" type="text/xsl"?>'
sed -e "1a$XSL_STR" "$XML1" >"$XSL_DIR/$I.xml"

cat >$XSL_DIR/$XSL <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" />
<xsl:template match="/">
<html>
	<title>Clients and borrows</title>
	<body>
		<h2>Клиенты и заказанные ими машины.</h2>
		<table border="1">
			<tr bgcolor="#9acd32">
				<th align="left">ФИО</th>
				<th align="left">Дата</th>
				<th align="left">Цена</th>
			</tr>
			<xsl:for-each select="root/clients">
				<tr>             
					<td valign="top">
						<xsl:attribute name="rowspan">
							<xsl:value-of select="count(borrow)+1"/>
						</xsl:attribute>
						<xsl:value-of select="@surname"/><xsl:text> </xsl:text>
						<xsl:value-of select="@name"/><xsl:text> </xsl:text>
						<xsl:value-of select="@fathername"/>
					</td>
					<xsl:for-each select="borrow">
						<tr>
							<td>
								<xsl:value-of select="@startdate"/>
							</td>
							<td>
								$ <xsl:value-of select="@payment"/>
							</td>
						</tr>
					</xsl:for-each>
				</tr>
			</xsl:for-each> 
		</table>
	</body>
</html>
</xsl:template>
</xsl:stylesheet>
EOF

# cat >$XSL_DIR/$XSL <<EOF
# <?xml version="1.0"?>
# <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
# <xsl:output method="html" />
# <xsl:template match="/">
# <html>
# 	<title>Clients and borrows</title>
# 	<body>
# 		<h2>Clients & their borrows.</h2>
# 		<table border>
# 			<tr bgcolor="#9acd32">
# 				<th align="left">ФИО</th>
# 				<th align="left">Дата</th>
# 				<th align="left">Цена</th>
# 			</tr>
# 			<xsl:for-each select="root/clients">
# 				<tr>             
# 					<td>
# 						<xsl:value-of select="surname"/> 
# 						<xsl:value-of select="name"/>
# 						<xsl:value-of select="fathername"/>
# 					</td>
# 					<tbody>
# 						<xsl:for-each select="borrow">
# 							<tr>
# 								<td>
# 									<xsl:value-of select="stardate"/>
# 								</td>
# 								<td>
# 									$ <xsl:value-of select="payment"/>
# 								</td>
# 							</tr>
# 						</xsl:for-each>
# 					</tbody>
# 				<tr>
# 			</xsl:for-each> 
# 		</table>
# 	</body>
# </html>
# </xsl:template>
# </xsl:stylesheet>
# EOF
# 
