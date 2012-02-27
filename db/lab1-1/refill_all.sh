#!/bin/sh
DB_NAME="$USER"
SYS_DB_NAME="postgres"
CARS_FILE="/tmp/cars"
CLIENTS_FILE="/tmp/clients"
BORROWS_FILE="/tmp/borrows"

die() {
	echo "!!! ERROR !!!"
	exit 100500
}

echo "### Droping old DB"
psql -d $SYS_DB_NAME -c "DROP DATABASE IF EXISTS \"$DB_NAME\";" || die
echo "### Creating DB: $DB_NAME"
createdb "$DB_NAME" || die
echo "### Creating tables and rules for DB:$DB_NAME"
psql -d "$DB_NAME" -f createtbl.sql || die

echo "### Generating data for DB:$DB_NAME"
python generate_data.py || die

echo "### Filling DB:$DB_NAME"
psql -d "$DB_NAME" -c "COPY cars FROM STDIN; \
		 SELECT setval('cars_id_seq', max(id)) FROM cars;" <"$CARS_FILE" || die
psql -d "$DB_NAME" -c "COPY clients FROM STDIN; \
		 SELECT setval('clients_id_seq', max(id)) FROM clients;" <"$CLIENTS_FILE" || die
psql -d "$DB_NAME" -c "COPY borrows FROM STDIN; \
		 SELECT setval('borrows_id_seq', max(id)) FROM borrows;" <"$BORROWS_FILE" || die

