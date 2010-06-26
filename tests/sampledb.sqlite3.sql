
CREATE TABLE customers
(
	 customer_id 	INTEGER PRIMARY KEY AUTOINCREMENT
	,name		TEXT NOT NULL
	,street		TEXT
	,birthday	REAL
);

INSERT INTO customers(name, street, birthday) VALUES('Jessie Mayer', 'Wallstreet', NULL);

