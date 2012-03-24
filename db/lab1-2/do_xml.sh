#!/bin/sh
XML_DIR="/tmp/lab1-2_xml"
XMLN[1]="1.xml"
XMLN[2]="2.xml"
XMLN[3]="3.xml"
XMLN[4]="3.xml"
XML[1]="$XML_DIR/${XMLN[1]}"
XML[2]="$XML_DIR/${XMLN[2]}"
XML[3]="$XML_DIR/${XMLN[3]}"
XML[4]="$XML_DIR/${XMLN[4]}"
XML_HEADER='<?xml version="1.1"?>'
SAXON="saxon8"
XMLTPROC="xmltproc"
mkdir -p $(dirname "${XML[1]}");


echo "$XML_HEADER" > "${XML[1]}"
psql -Atf - >>"${XML[1]}" <<EOF
SELECT XMLSERIALIZE( CONTENT xmlelement(NAME root,xmlagg(xml_cli)) AS TEXT)
FROM ( SELECT xmlelement(NAME clients, 
			xmlattributes(c.surname, c.name, c.fathername), 
			bx.xml_bor) AS xml_cli
		FROM clients c 
			JOIN ( SELECT b.clientid AS id, xmlagg(xmlelement(NAME borrows,
					xmlattributes(b.startDate, CAST(b.payment AS NUMERIC(18,2)) AS payment ))) AS xml_bor
				FROM borrows b
				GROUP BY b.clientID ) AS bx USING (id)
		LIMIT 10) AS cx;
EOF
sed -e 's/></>\n</g' -i "${XML[1]}"

echo "$XML_HEADER" > "${XML[2]}"
psql -Atf - >>"${XML[2]}" <<EOF
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
sed -e 's/></>\n</g' -i "${XML[2]}"

# echo "$XML_HEADER" > "${XML[2]}"
# psql -Atf - >>"${XML[2]}" <<EOF
# SELECT query_to_xml( 
# 		'SELECT c.surname, c.name, c.fathername, b.startDate, 
# 			CAST(b.payment AS NUMERIC(18,2)) AS payment
# 		FROM clients c NATURAL JOIN borrows b 
# 		LIMIT 10;',
# 		FALSE, FALSE, '');
# EOF

echo "$XML_HEADER" > "${XML[3]}"
echo "SELECT xmlelement(NAME root, table_to_xml('cars', FALSE, TRUE, ''));" | psql -Atf - >>"${XML[3]}" 

############################################################
#               CSS                                        #
############################################################
CSS_DIR="$XML_DIR/css"
mkdir -p "$CSS_DIR"

for I in {1..3}; do
	CSS="$I.css"
	CSS_STR='<?xml-stylesheet href="'"$CSS"'" type="text/css"?>'
	sed -e "1a$CSS_STR" "${XML[2]}" >"$CSS_DIR/$I.xml"
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

for I in {1..3}; do
	XSL="$I.xsl"
	XSL_STR='<?xml-stylesheet href="'"$XSL"'" type="text/xsl"?>'
	sed -e "1a$XSL_STR" "${XML[$I]}" >"$XSL_DIR/$I.xml"
done


cat >$XSL_DIR/1.xsl <<EOF
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
							<xsl:value-of select="count(borrows)+1"/>
						</xsl:attribute>
						<xsl:value-of select="@surname"/><xsl:text> </xsl:text>
						<xsl:value-of select="@name"/><xsl:text> </xsl:text>
						<xsl:value-of select="@fathername"/>
					</td>
					<xsl:for-each select="borrows">
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

cat >$XSL_DIR/2.xsl <<EOF
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
							<xsl:value-of select="count(borrows)+1"/>
						</xsl:attribute>
						<xsl:value-of select="surname"/><xsl:text> </xsl:text>
						<xsl:value-of select="name"/><xsl:text> </xsl:text>
						<xsl:value-of select="fathername"/>
					</td>
					<xsl:for-each select="borrows">
						<tr>
							<td>
								<xsl:value-of select="startdate"/>
							</td>
							<td>
								$ <xsl:value-of select="payment"/>
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

cat >$XSL_DIR/3.xsl <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" />
<xsl:template match="/">
<html>
	<title>Clients and borrows</title>
	<body>
		<h2>Наши машинки.</h2>
		<table border="1">
			<tr bgcolor="#FEACDB">
				<th align="left">Номер</th>
				<th align="left">Модель</th>
				<th align="left">Цвет</th>
			</tr>
			<xsl:for-each select="root/cars">
				<tr>             
					<td valign="top">
						<xsl:value-of select="num"/>
					</td>
					<td>
					<a>
						<xsl:attribute name="href">
							http://yandex.ru/yandsearch?text=<xsl:value-of select="model"/>
						</xsl:attribute>
						<xsl:value-of select="model"/>
					</a>
					</td>
					<td>
						<xsl:value-of select="color"/>
					</td>
				</tr>
			</xsl:for-each> 
		</table>
	</body>
</html>
</xsl:template>
</xsl:stylesheet>
EOF

for I in {1..3}; do
	XSL="$XSL_DIR/$I.xsl"
	XML="$XSL_DIR/$I.xml"
	HTM="$XSL_DIR/$I.htm"
	if env ${SAXON} --help >/dev/null 2>&1;then 
		${SAXON} -o "$HTM" "$XML" "$XSL"
	elif env ${XMLTPROC} --help >/dev/null 2>&1;then 
		${XMLTPROC} -o "$HTM" "$XSL" "$XML" 
	else
		echo "NO PARSER FOUND!!!"
	fi
done


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
# 						<xsl:for-each select="borrows">
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
