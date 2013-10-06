CREATE TABLE IF NOT EXISTS blobstream_test(
         id     SERIAL PRIMARY KEY
        ,data   bytea NOT NULL);

INSERT INTO blobstream_test(data) VALUES(E'\\x738200936F728A37BCD90D');


